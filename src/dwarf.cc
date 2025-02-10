#include "dwarf.h"
#include <unordered_map>
#include <sstream>
#include <cstring>
#include <cassert>

namespace Dwarf {

static bool IsWhite(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r';
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
  debug_str << "\t.section .debug_str,\"MS\",@progbits,1\n";
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

  for (const auto &entry : this->entries_) {
    // abbrev code
    debug_info << "\t.uleb128 " << abbrev_code << "\n";
    debug_abbrev << "\t.uleb128" << abbrev_code << "\n";

    // DW_TAG
    debug_abbrev << "\t.uleb128 " << 
      static_cast<size_t>(entry.tag_) << "\n";

    // DW_CHILDREN
    if (entry.children_) {
      debug_abbrev << "\t.byte 1\n";
    } else {
      debug_abbrev << "\t.byte 0\n";
    }

    for (const auto &attr : entry.attributes_) {
      assert(attr.attr_value != nullptr);
      debug_abbrev << "\t.uleb128 " << static_cast<size_t>(attr.attr_name) << "\n";
      attr.attr_value->Generate(&meta_data);
    }
    
    // an empty attribute
    debug_abbrev << "\t.uleb128 0\n";
    debug_abbrev << "\t.uleb128 0\n";

    abbrev_code++;
  }

  // end of debug info
  debug_info << "\t.uleb128 0\n";
  // end of debug_abbrev
  debug_abbrev << "\t.uleb128 0\n";

  // process unit length
  unit_header.unit_length = meta_data.debug_info_size +
    /* Size of unit header, exclude unit_length */
    sizeof(unit_header.version) /* 2 */ + 
    sizeof(unit_header.debug_abbrev_offset) /* 4 */ +
    sizeof(unit_header.address_size) /* 1 */;

  os << "\t.section .debug_info,\"\",@progbits\n";
  // print compilation unit header
  os << "\t.long " << unit_header.unit_length << "\n";
  os << "\t.value " << unit_header.version << "\n";
  os << "\t.long .Ldebug_abbrev0\n";
  os << "\t.byte " << unit_header.address_size << "\n";

  os << debug_info.str();
  os << debug_abbrev.str();
  os << debug_lines.str();
  os << debug_str.str();

  return os;
}

} // namespace Dwarf
