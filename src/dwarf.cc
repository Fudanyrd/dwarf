#include "dwarf.h"
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <cstring>
#include <cassert>

namespace Dwarf {

size_t DebugInfoEntry::instances_ = 0;

static bool IsWhite(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
}

size_t sizeof_uleb128(size_t value) {
    size_t size = 0;

    do {
        size++;
        value >>= 7;
    } while (value != 0);

    return size;
}

size_t sizeof_sleb128(long value) {
    size_t size = 0;
    bool more = true;
    bool negative = value < 0;

    do {
        size++;
        more = (value != 0 && value != -1) || (size < 10 && (value & 0x40));
        value >>= 7;
    } while (more);

    if (negative) {
        size_t i = 0;
        for (; i < size; i++) {
            if ((value & 0x7f) != 0x7f) {
                break;
            }
            value >>= 7;
        }
        if (i == size) {
            size++;
        }
    }

    return size;
}

auto Split(const std::string &str) -> std::vector<std::string> {
  std::vector<std::string> ret;

  size_t i = 0;
  const size_t len = str.size();

  while (i < len) {
    size_t j = i;
    while (j < len && IsWhite(str[j])) {
      ++j;
    }

    i = j;
    while (j < len && !IsWhite(str[j])) {
      j++;
    }
    if (j > i) {
      ret.push_back(str.substr(i, j - i));
      i = j;
    }
  }

  return ret;
}

std::ostream &DebugInfo::Generate(std::ostream &os) const {
  std::ostringstream debug_info;
  // debug_info << "\t.section .debug_info,\"\",@progbits\n";

  std::ostringstream debug_abbrev;
  debug_abbrev << "\t.section .debug_abbrev,\"\",@progbits\n";
  debug_abbrev << ".Ldebug_abbrev0:\n";

  std::ostringstream debug_lines;
  debug_lines << "\t.section .debug_lines,\"\",@progbits\n";
  debug_lines << ".Ldebug_lines0:\n";

  std::ostringstream debug_str;
  debug_str << "\t.section .debug_str\n";
  debug_str << ".Ldebug_str0:\n";

  static struct CompilationUnitHeader unit_header;
  memset(&unit_header, 0, sizeof(unit_header));
  unit_header.version = 4;
  unit_header.address_size = this->GetPointerSize();

  static struct MetaData meta_data;
  memset(&meta_data, 0, sizeof(meta_data));
  meta_data.debug_info = &debug_info;
  meta_data.debug_abbrev = &debug_abbrev;
  meta_data.debug_str = &debug_str;

  size_t abbrev_code = 1;

  for (auto *entry_ptr : this->entries_) {
    auto &entry = *entry_ptr;
    // abbrev code
    debug_info << entry.GetLabel() << ":\n";
    debug_info << "\t.uleb128 " << abbrev_code << "\n";
    meta_data.debug_info_size += sizeof_uleb128(abbrev_code);
    debug_abbrev << "\t.uleb128 " << abbrev_code << "\n";
    meta_data.debug_abbrev_size += sizeof_uleb128(abbrev_code);

    // DW_TAG
    debug_abbrev << "\t.uleb128 " << 
      static_cast<size_t>(entry.tag_) << "\n";
    meta_data.debug_abbrev_size += 
      sizeof_uleb128(static_cast<size_t>(entry.tag_));

    // DW_CHILDREN
    if (entry.children_) {
      debug_abbrev << "\t.byte 1\n";
    } else {
      debug_abbrev << "\t.byte 0\n";
    }
    meta_data.debug_abbrev_size += sizeof(uint8_t);

    for (const auto &attr : entry.attributes_) {
      assert(attr.attr_value != nullptr);
      auto name = attr.attr_name;
      if (name != DW_AT::DW_AT_reserved) {
        debug_abbrev << "\t.uleb128 " << static_cast<size_t>(name) << "\n";
        meta_data.debug_abbrev_size += sizeof_uleb128(static_cast<size_t>(name));
      }
      attr.attr_value->Generate(&meta_data);
    }
    
    // an empty attribute
    debug_abbrev << "\t.uleb128 0\n";
    meta_data.debug_abbrev_size += sizeof_uleb128(0);
    debug_abbrev << "\t.uleb128 0\n";
    meta_data.debug_abbrev_size += sizeof_uleb128(0);

    abbrev_code++;
  }

  // end of debug info
  debug_info << "\t.uleb128 0\n";
  meta_data.debug_info_size += sizeof_uleb128(0);
  // end of debug_abbrev
  debug_abbrev << "\t.uleb128 0\n";
  meta_data.debug_abbrev_size += sizeof_uleb128(0);

  // process unit length
  unit_header.unit_length = meta_data.debug_info_size +
    /* Size of unit header, exclude unit_length */
    sizeof(unit_header.version) /* 2 */ + 
    sizeof(unit_header.debug_abbrev_offset) /* 4 */ +
    sizeof(unit_header.address_size) /* 1 */;

  os << "\t.section .debug_info,\"\",@progbits\n";
  os << ".Ldebug_info0:\n";
  // print compilation unit header
  os << "\t.long " << unit_header.unit_length << "\n";
  os << "\t.value " << unit_header.version << "\n";
  os << "\t.long .Ldebug_abbrev0\n";
  os << "\t.byte " << static_cast<uint32_t>(unit_header.address_size) << "\n";

  os << debug_info.str();
  os << debug_abbrev.str();
  os << debug_lines.str();
  os << debug_str.str();

  return os;
}

void DwarfOperation::ComputeSize() {
  this->size_ = 1;
  if (this->num_operand_ == 0) {
    return;
  }

  // Page 163, figure 24.
  if (this->num_operand_ == 1) {
    switch(this->opcode_) {
    case (DW_OP::DW_OP_addr): {
      this->size_ += this->m64_ ? 8 : 4;
      break;
    }
    case (DW_OP::DW_OP_const1u):
    case (DW_OP::DW_OP_pick):
    case(DW_OP::DW_OP_const1s): {
      this->size_ += 1;
      break;
    }
    case (DW_OP::DW_OP_call2):
    case (DW_OP::DW_OP_const2u):
    case (DW_OP::DW_OP_skip):
    case (DW_OP::DW_OP_bra):
    case (DW_OP::DW_OP_const2s): {
      this->size_ += 2;
      break;
    }
    case (DW_OP::DW_OP_call4):
    case (DW_OP::DW_OP_call_ref):
    case (DW_OP::DW_OP_const4u):
    case (DW_OP::DW_OP_const4s): {
      this->size_ += 4;
      break;
    }
    case (DW_OP::DW_OP_const8u):
    case (DW_OP::DW_OP_const8s): {
      this->size_ += 8;
      break;
    }
    case (DW_OP::DW_OP_plus_uconst):
    case (DW_OP::DW_OP_constu): {
      size_t operand = ToSizeType(this->operands_[0]);
      this->size_ += sizeof_uleb128(operand);
      break;
    }
    case (DW_OP::DW_OP_breg0):
    case (DW_OP::DW_OP_breg1):
    case (DW_OP::DW_OP_breg2):
    case (DW_OP::DW_OP_breg3):
    case (DW_OP::DW_OP_breg4):
    case (DW_OP::DW_OP_breg5):
    case (DW_OP::DW_OP_breg6):
    case (DW_OP::DW_OP_breg7):
    case (DW_OP::DW_OP_breg8):
    case (DW_OP::DW_OP_breg9):
    case (DW_OP::DW_OP_breg10):
    case (DW_OP::DW_OP_breg11):
    case (DW_OP::DW_OP_breg12):
    case (DW_OP::DW_OP_breg13):
    case (DW_OP::DW_OP_breg14):
    case (DW_OP::DW_OP_breg15):
    case (DW_OP::DW_OP_breg16):
    case (DW_OP::DW_OP_breg17):
    case (DW_OP::DW_OP_breg18):
    case (DW_OP::DW_OP_breg19):
    case (DW_OP::DW_OP_breg20):
    case (DW_OP::DW_OP_breg21):
    case (DW_OP::DW_OP_breg22):
    case (DW_OP::DW_OP_breg23):
    case (DW_OP::DW_OP_breg24):
    case (DW_OP::DW_OP_breg25):
    case (DW_OP::DW_OP_breg26):
    case (DW_OP::DW_OP_breg27):
    case (DW_OP::DW_OP_breg28):
    case (DW_OP::DW_OP_breg29):
    case (DW_OP::DW_OP_breg30):
    case (DW_OP::DW_OP_breg31):
    case (DW_OP::DW_OP_fbreg):
    case (DW_OP::DW_OP_consts): {
      long operand = ToLong(this->operands_[0]);
      this->size_ += sizeof_sleb128(operand);
      break;
    }

    default: {
      std::cerr << "Note: opcode " << static_cast<size_t>(this->opcode_) << std::endl;
      std::cerr << "This is a bug. Please report it." << std::endl;
      throw std::runtime_error("Number of operands(1) is not compatible with opcode");
    }
    }
  } else if (this->num_operand_ == 2) {
    switch (this->opcode_) {
    case (DW_OP::DW_OP_bregx): {
      // first is uleb128, second is sleb128
      size_t operand1 = ToSizeType(this->operands_[0]);
      long operand2 = ToLong(this->operands_[1]);
      this->size_ += sizeof_uleb128(operand1) + sizeof_sleb128(operand2);
      break;
    }
    case (DW_OP::DW_OP_bit_piece): {
      // first is uleb128, second is uleb128
      size_t operand1 = ToSizeType(this->operands_[0]);
      size_t operand2 = ToSizeType(this->operands_[1]);
      this->size_ += sizeof_uleb128(operand1) + sizeof_uleb128(operand2);
      break;
    }
    case (DW_OP::DW_OP_implicit_value): {
      // ULEB128 size followed by block of that size
      size_t operand1 = ToSizeType(this->operands_[0]);
      this->size_ += sizeof_uleb128(operand1) /* sizeof uleb128 */
      + operand1 /* size of the block */;
      break;
    }
    default: {
      std::cerr << "Note: opcode " << static_cast<size_t>(this->opcode_) << std::endl;
      std::cerr << "This is a bug. Please report it." << std::endl;
      throw std::runtime_error("Number of operands(2) is not compatible with opcode");
    }
    }
  } else {
    std::cerr << "Note: opcode " << static_cast<size_t>(this->opcode_) << std::endl;
    std::cerr << "This is a bug. Please report it." << std::endl;
    throw std::runtime_error("Number of operands(>=3) is not compatible with opcode");
  }
}

} // namespace Dwarf
