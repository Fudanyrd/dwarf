#ifndef __DWARF_H__
#define __DWARF_H__

// We're using dwarf version 4
// #define VERSION 4

#include <cstdbool>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <sstream>

#include "utils.h"

// Dwarf Debugging Standard Format
// Homepage: https://dwarfstd.org/
// Manuals: https://dwarfstd.org/download.html
// Version 4 Manual: https://dwarfstd.org/doc/DWARF4.pdf
namespace Dwarf {

// Use only ' ' and '\t' as delimiter, and discard these
// whitespaces
auto Split(const std::string &str) -> std::vector<std::string>;

// [Page 161]
// returns the size of an uleb128 in bytes.
extern size_t sizeof_uleb128(size_t value);
extern size_t sizeof_sleb128(long value);

static const char *author = "Fudanyrd:" __FILE__;
static const char *date = __DATE__ ", " __TIME__;

// We're using dwarf version 4
constexpr size_t VERSION = 4;

// Page [151-155]: Dwarf Tag Names
enum class DW_TAG {
  DW_TAG_array_type = 0x01,
  DW_TAG_class_type = 0x02,
  DW_TAG_entry_point = 0x03,
  DW_TAG_enumeration_type = 0x04,
  DW_TAG_formal_parameter = 0x05,
  DW_TAG_imported_declaration = 0x08,
  DW_TAG_label = 0x0a,
  DW_TAG_lexical_block = 0x0b,
  DW_TAG_member = 0x0d,
  DW_TAG_pointer_type = 0x0f,
  DW_TAG_reference_type = 0x10,
  DW_TAG_compile_unit = 0x11,
  DW_TAG_string_type = 0x12,
  DW_TAG_structure_type = 0x13,
  DW_TAG_subroutine_type = 0x15,
  DW_TAG_typedef = 0x16,
  DW_TAG_union_type = 0x17,
  DW_TAG_unspecified_parameters = 0x18,
  DW_TAG_variant = 0x19,
  DW_TAG_common_block = 0x1a,
  DW_TAG_common_inclusion = 0x1b,
  DW_TAG_inheritance = 0x1c,
  DW_TAG_inlined_subroutine = 0x1d,
  DW_TAG_module = 0x1e,
  DW_TAG_ptr_to_member_type = 0x1f,
  DW_TAG_set_type = 0x20,
  DW_TAG_subrange_type = 0x21,
  DW_TAG_with_stmt = 0x22,
  DW_TAG_access_declaration = 0x23,
  DW_TAG_base_type = 0x24,
  DW_TAG_catch_block = 0x25,
  DW_TAG_const_type = 0x26,
  DW_TAG_constant = 0x27,
  DW_TAG_enumerator = 0x28,
  DW_TAG_file_type = 0x29,
  DW_TAG_friend = 0x2a,
  DW_TAG_namelist = 0x2b,
  DW_TAG_namelist_item = 0x2c,
  DW_TAG_packed_type = 0x2d,
  DW_TAG_subprogram = 0x2e,
  DW_TAG_template_type_parameter = 0x2f,
  DW_TAG_template_value_parameter = 0x30,
  DW_TAG_thrown_type = 0x31,
  DW_TAG_try_block = 0x32,
  DW_TAG_variant_part = 0x33,
  DW_TAG_variable = 0x34,
  DW_TAG_volatile_type = 0x35,
  DW_TAG_dwarf_procedure = 0x36,
  DW_TAG_restrict_type = 0x37,
  DW_TAG_interface_type = 0x38,
  DW_TAG_namespace = 0x39,
  DW_TAG_imported_module = 0x3a,
  DW_TAG_unspecified_type = 0x3b,
  DW_TAG_partial_unit = 0x3c,
  DW_TAG_imported_unit = 0x3d,
  DW_TAG_condition = 0x3f,
  DW_TAG_shared_type = 0x40,
  DW_TAG_type_unit = 0x41,
  DW_TAG_rvalue_reference_type = 0x42,
  DW_TAG_template_alias = 0x43,
  DW_TAG_lo_user = 0x4080,
  DW_TAG_hi_user = 0xffff
};

// Page [154] Dwarf Children encodings
#define DW_CHILDREN_yes (0x01)
// Page [154] Dwarf Children encodings
#define DW_CHILDREN_no (0x0)

// Page [155-159]: Dwarf Attribute Types
enum class DW_AT {
  // attribute encodings
  DW_AT_reserved = 0x0,
  DW_AT_sibling = 0x1,
  DW_AT_location = 0x2,
  DW_AT_name = 0x3,
  DW_AT_ordering = 0x9,
  DW_AT_byte_size = 0x0b,
  DW_AT_bit_offset = 0x0c,
  DW_AT_bit_size = 0x0d,
  DW_AT_stmt_list = 0x10,
  DW_AT_low_pc = 0x11,
  DW_AT_high_pc = 0x12,
  DW_AT_language = 0x13,
  DW_AT_discr = 0x15,
  DW_AT_discr_value = 0x16,
  DW_AT_visibility = 0x17,
  DW_AT_import = 0x18,
  DW_AT_string_length = 0x19,
  DW_AT_common_reference = 0x1a,
  DW_AT_comp_dir = 0x1b,
  DW_AT_const_value = 0x1c,
  DW_AT_containing_type = 0x1d,
  DW_AT_default_value = 0x1e,
  DW_AT_inline = 0x20,
  DW_AT_is_optional = 0x21,
  DW_AT_lower_bound = 0x22,
  DW_AT_producer = 0x25,
  DW_AT_prototyped = 0x27,
  DW_AT_return_addr = 0x2a,
  DW_AT_start_scope = 0x2c,
  DW_AT_bit_stride = 0x2e,
  DW_AT_upper_bound = 0x2f,
  DW_AT_abstract_origin = 0x31,
  DW_AT_accessibility = 0x32,
  DW_AT_address_class = 0x33,
  DW_AT_artificial = 0x34,
  DW_AT_base_types = 0x35,
  DW_AT_calling_convention = 0x36,
  DW_AT_count = 0x37,
  DW_AT_data_member_location = 0x38,
  DW_AT_decl_column = 0x39,
  DW_AT_decl_file = 0x3a,
  DW_AT_decl_line = 0x3b,
  DW_AT_declaration = 0x3c,
  DW_AT_discr_list = 0x3d,
  DW_AT_encoding = 0x3e,
  DW_AT_external = 0x3f,
  DW_AT_frame_base = 0x40,
  DW_AT_friend = 0x41,
  DW_AT_identifier_case = 0x42,
  DW_AT_macro_info = 0x43,
  DW_AT_namelist_item = 0x44,
  DW_AT_priority = 0x45,
  DW_AT_segment = 0x46,
  DW_AT_specification = 0x47,
  DW_AT_static_link = 0x48,
  DW_AT_type = 0x49,
  DW_AT_use_location = 0x4a,
  DW_AT_variable_parameter = 0x4b,
  DW_AT_virtuality = 0x4c,
  DW_AT_vtable_elem_location = 0x4d,
  DW_AT_allocated = 0x4e,
  DW_AT_associated = 0x4f,
  DW_AT_data_location = 0x50,
  DW_AT_byte_stride = 0x51,
  DW_AT_entry_pc = 0x52,
  DW_AT_use_UTF8 = 0x53,
  DW_AT_extension = 0x54,
  DW_AT_ranges = 0x55,
  DW_AT_trampoline = 0x56,
  DW_AT_call_column = 0x57,
  DW_AT_call_file = 0x58,
  DW_AT_call_line = 0x59,
  DW_AT_description = 0x5a,
  DW_AT_binary_scale = 0x5b,
  DW_AT_decimal_scale = 0x5c,
  DW_AT_small = 0x5d,
  DW_AT_decimal_sign = 0x5e,
  DW_AT_digit_count = 0x5f,
  DW_AT_picture_string = 0x60,
  DW_AT_mutable = 0x61,
  DW_AT_threads_scaled = 0x62,
  DW_AT_explicit = 0x63,
  DW_AT_object_pointer = 0x64,
  DW_AT_endianity = 0x65,
  DW_AT_elemental = 0x66,
  DW_AT_pure = 0x67,
  DW_AT_recursive = 0x68,
  DW_AT_signature = 0x69,
  DW_AT_main_subprogram = 0x6a,
  DW_AT_data_bit_offset = 0x6b,
  DW_AT_const_expr = 0x6c,
  DW_AT_enum_class = 0x6d,
  DW_AT_linkage_name = 0x6e,
  DW_AT_lo_user = 0x2000,
  DW_AT_hi_user = 0x3fff
};

// Page [160-161] Dwarf From Encoding
enum class DW_FORM {
  DW_FORM_reserved = 0x0,
  DW_FORM_addr = 0x01,
  DW_FORM_block2 = 0x03,
  DW_FORM_block4 = 0x04,
  DW_FORM_data2 = 0x05,
  DW_FORM_data4 = 0x06,
  DW_FORM_data8 = 0x07,
  DW_FORM_string = 0x08,
  DW_FORM_block = 0x09,
  DW_FORM_block1 = 0x0a,
  DW_FORM_data1 = 0x0b,
  DW_FORM_flag = 0x0c,
  DW_FORM_sdata = 0x0d,
  DW_FORM_strp = 0x0e,
  DW_FORM_udata = 0x0f,
  DW_FORM_ref_addr = 0x10,
  DW_FORM_ref1 = 0x11,
  DW_FORM_ref2 = 0x12,
  DW_FORM_ref4 = 0x13,
  DW_FORM_ref8 = 0x14,
  DW_FORM_ref_udata = 0x15,
  DW_FORM_indirect = 0x16,
  DW_FORM_sec_offset = 0x17,
  DW_FORM_exprloc = 0x18,
  DW_FORM_flag_present = 0x19,
  DW_FORM_ref_sig8 = 0x20
};

// Page [163-167] Dwarf Op Encoding
enum class DW_OP {
  DW_OP_addr = 0x03,
  DW_OP_deref = 0x06,
  DW_OP_const1u = 0x08,
  DW_OP_const1s = 0x09,
  DW_OP_const2u = 0x0a,
  DW_OP_const2s = 0x0b,
  DW_OP_const4u = 0x0c,
  DW_OP_const4s = 0x0d,
  DW_OP_const8u = 0x0e,
  DW_OP_const8s = 0x0f,
  DW_OP_constu = 0x10,
  DW_OP_consts = 0x11,
  DW_OP_dup = 0x12,
  DW_OP_drop = 0x13,
  DW_OP_over = 0x14,
  DW_OP_pick = 0x15,
  DW_OP_swap = 0x16,
  DW_OP_rot = 0x17,
  DW_OP_xderef = 0x18,
  DW_OP_abs = 0x19,
  DW_OP_and = 0x1a,
  DW_OP_div = 0x1b,
  DW_OP_minus = 0x1c,
  DW_OP_mod = 0x1d,
  DW_OP_mul = 0x1e,
  DW_OP_neg = 0x1f,
  DW_OP_not = 0x20,
  DW_OP_or = 0x21,
  DW_OP_plus = 0x22,
  DW_OP_plus_uconst = 0x23,
  DW_OP_shl = 0x24,
  DW_OP_shr = 0x25,
  DW_OP_shra = 0x26,
  DW_OP_xor = 0x27,
  DW_OP_skip = 0x2f,
  DW_OP_bra = 0x28,
  DW_OP_eq = 0x29,
  DW_OP_ge = 0x2a,
  DW_OP_gt = 0x2b,
  DW_OP_le = 0x2c,
  DW_OP_lt = 0x2d,
  DW_OP_ne = 0x2e,
  DW_OP_lit0 = 0x30,
  DW_OP_lit1 = 0x31,
  DW_OP_lit2 = 0x32,
  DW_OP_lit3 = 0x33,
  DW_OP_lit4 = 0x34,
  DW_OP_lit5 = 0x35,
  DW_OP_lit6 = 0x36,
  DW_OP_lit7 = 0x37,
  DW_OP_lit8 = 0x38,
  DW_OP_lit9 = 0x39,
  DW_OP_lit10 = 0x3a,
  DW_OP_lit11 = 0x3b,
  DW_OP_lit12 = 0x3c,
  DW_OP_lit13 = 0x3d,
  DW_OP_lit14 = 0x3e,
  DW_OP_lit15 = 0x3f,
  DW_OP_lit16 = 0x40,
  DW_OP_lit17 = 0x41,
  DW_OP_lit18 = 0x42,
  DW_OP_lit19 = 0x43,
  DW_OP_lit20 = 0x44,
  DW_OP_lit21 = 0x45,
  DW_OP_lit22 = 0x46,
  DW_OP_lit23 = 0x47,
  DW_OP_lit24 = 0x48,
  DW_OP_lit25 = 0x49,
  DW_OP_lit26 = 0x4a,
  DW_OP_lit27 = 0x4b,
  DW_OP_lit28 = 0x4c,
  DW_OP_lit29 = 0x4d,
  DW_OP_lit30 = 0x4e,
  DW_OP_lit31 = 0x4f,
  DW_OP_reg0 = 0x50,
  DW_OP_reg1 = 0x51,
  DW_OP_reg2 = 0x52,
  DW_OP_reg3 = 0x53,
  DW_OP_reg4 = 0x54,
  DW_OP_reg5 = 0x55,
  DW_OP_reg6 = 0x56,
  DW_OP_reg7 = 0x57,
  DW_OP_reg8 = 0x58,
  DW_OP_reg9 = 0x59,
  DW_OP_reg10 = 0x5a,
  DW_OP_reg11 = 0x5b,
  DW_OP_reg12 = 0x5c,
  DW_OP_reg13 = 0x5d,
  DW_OP_reg14 = 0x5e,
  DW_OP_reg15 = 0x5f,
  DW_OP_reg16 = 0x60,
  DW_OP_reg17 = 0x61,
  DW_OP_reg18 = 0x62,
  DW_OP_reg19 = 0x63,
  DW_OP_reg20 = 0x64,
  DW_OP_reg21 = 0x65,
  DW_OP_reg22 = 0x66,
  DW_OP_reg23 = 0x67,
  DW_OP_reg24 = 0x68,
  DW_OP_reg25 = 0x69,
  DW_OP_reg26 = 0x6a,
  DW_OP_reg27 = 0x6b,
  DW_OP_reg28 = 0x6c,
  DW_OP_reg29 = 0x6d,
  DW_OP_reg30 = 0x6e,
  DW_OP_reg31 = 0x6f,
  DW_OP_breg0 = 0x70,
  DW_OP_breg1 = 0x71,
  DW_OP_breg2 = 0x72,
  DW_OP_breg3 = 0x73,
  DW_OP_breg4 = 0x74,
  DW_OP_breg5 = 0x75,
  DW_OP_breg6 = 0x76,
  DW_OP_breg7 = 0x77,
  DW_OP_breg8 = 0x78,
  DW_OP_breg9 = 0x79,
  DW_OP_breg10 = 0x7a,
  DW_OP_breg11 = 0x7b,
  DW_OP_breg12 = 0x7c,
  DW_OP_breg13 = 0x7d,
  DW_OP_breg14 = 0x7e,
  DW_OP_breg15 = 0x7f,
  DW_OP_breg16 = 0x80,
  DW_OP_breg17 = 0x81,
  DW_OP_breg18 = 0x82,
  DW_OP_breg19 = 0x83,
  DW_OP_breg20 = 0x84,
  DW_OP_breg21 = 0x85,
  DW_OP_breg22 = 0x86,
  DW_OP_breg23 = 0x87,
  DW_OP_breg24 = 0x88,
  DW_OP_breg25 = 0x89,
  DW_OP_breg26 = 0x8a,
  DW_OP_breg27 = 0x8b,
  DW_OP_breg28 = 0x8c,
  DW_OP_breg29 = 0x8d,
  DW_OP_breg30 = 0x8e,
  DW_OP_breg31 = 0x8f,
  DW_OP_regx = 0x90,
  DW_OP_fbreg = 0x91,
  DW_OP_bregx = 0x92,
  DW_OP_piece = 0x93,
  DW_OP_deref_size = 0x94,
  DW_OP_xderef_size = 0x95,
  DW_OP_nop = 0x96,
  DW_OP_push_object_address = 0x97,
  DW_OP_call2 = 0x98,
  DW_OP_call4 = 0x99,
  DW_OP_call_ref = 0x9a,
  DW_OP_form_tls_address = 0x9b,
  DW_OP_call_frame_cfa = 0x9c,
  DW_OP_bit_piece = 0x9d,
  DW_OP_implicit_value = 0x9e,
  DW_OP_stack_value = 0x9f,
  DW_OP_lo_user = 0xe0,
  DW_OP_hi_user = 0xff
};

// Page [168-169] Dwarf Basic Type Encoding
// used by DW_AT_encoding
enum class DW_ATE {
  DW_ATE_address = 0x01,
  DW_ATE_boolean = 0x02,
  DW_ATE_complex_float = 0x03,
  DW_ATE_float = 0x04,
  DW_ATE_signed = 0x05,
  DW_ATE_signed_char = 0x06,
  DW_ATE_unsigned = 0x07,
  DW_ATE_unsigned_char = 0x08,
  DW_ATE_imaginary_float = 0x09,
  DW_ATE_packed_decimal = 0x0a,
  DW_ATE_numeric_string = 0x0b,
  DW_ATE_edited = 0x0c,
  DW_ATE_signed_fixed = 0x0d,
  DW_ATE_unsigned_fixed = 0x0e,
  DW_ATE_decimal_float = 0x0f,
  DW_ATE_UTF = 0x10,
  DW_ATE_lo_user = 0x80,
  DW_ATE_hi_user = 0xff
};

// Encoding of constants used in DW_AT_decimal_sign.
enum class DW_DS {
  DW_DS_unsigned = 0x01,
  DW_DS_leading_overpunch = 0x02,
  DW_DS_trailing_overpunch = 0x03,
  DW_DS_leading_separate = 0x04,
  DW_DS_trailing_separate = 0x05,
};

// Encoding of constants used in DW_AT_endianity.
enum class DW_END {
  DW_END_default = 0x00,
  DW_END_big = 0x01,
  DW_END_little = 0x02,
  DW_END_lo_user = 0x40,
  DW_END_hi_user = 0xff
};

// Encoding of constants used in DW_AT_accessibility.
enum class DW_ACCESS {
  DW_ACCESS_public = 0x01,
  DW_ACCESS_protected = 0x02,
  DW_ACCESS_private = 0x03
};

// Encoding of constants used in DW_AT_visibility.
enum class DW_VIS {
  DW_VIS_local = 0x01,
  DW_VIS_exported = 0x02,
  DW_VIS_qualified = 0x03
};

// Encoding of constants used in DW_AT_virtuality.
enum class DW_VIRTUALITY {
  DW_VIRTUALITY_none = 0x00,
  DW_VIRTUALITY_virtual = 0x01,
  DW_VIRTUALITY_pure_virtual = 0x02
};

// Encoding of constants used in DW_AT_language.
enum class DW_LANG {
  DW_LANG_C89 = 0x0001,
  DW_LANG_C = 0x0002,
  DW_LANG_Ada83 = 0x0003,
  DW_LANG_C_plus_plus = 0x0004,
  DW_LANG_Cobol74 = 0x005,
  DW_LANG_Cobol85 = 0x0006,
  DW_LANG_Fortran77 = 0x0007,
  DW_LANG_Fortran90 = 0x0008,
  DW_LANG_Pascal83 = 0x0009,
  DW_LANG_Modula2 = 0x000a,
  DW_LANG_Java = 0x000b,
  DW_LANG_C99 = 0x000c,
  DW_LANG_Ada95 = 0x000d,
  DW_LANG_Fortran95 = 0x000e,
  DW_LANG_PLI = 0x000f,
  DW_LANG_ObjC = 0x0010,
  DW_LANG_ObjC_plus_plus = 0x0011,
  DW_LANG_UPC = 0x0012,
  DW_LANG_D = 0x0013,
  DW_LANG_Python = 0x0014,
  DW_LANG_lo_user = 0x8000,
  DW_LANG_hi_user = 0xffff
};

constexpr size_t DW_ADDR_none = (0x0);

// Encodings of constants used in DW_AT_identifier_case.
enum class DW_ID {
  DW_ID_case_sensitive = 0x00,
  DW_ID_up_case = 0x01,
  DW_ID_down_case = 0x02,
  DW_ID_case_insensitive = 0x03
};

// Encodings of constants used in DW_AT_calling_convention.
enum class DW_CC {
  DW_CC_normal = 0x01,
  DW_CC_program = 0x02,
  DW_CC_nocall = 0x03,
  DW_CC_lo_user = 0x40,
  DW_CC_hi_user = 0xff
};

// Replace Dwarf Constants with their string equivalents.
// For example, DW_TAG_array_type will be replaced by 0x1
auto Compile(const std::string &fobj) -> std::string;

typedef uint64_t uint128_t[2];

// 32-Bit Dwarf Compilation Unit Header
struct CompilationUnitHeader {
  uint32_t unit_length; // .long
  uint16_t version; // .value
  uint32_t debug_abbrev_offset; // .long
  uint8_t address_size; // .byte
};

struct TypeUnitHeader {
  uint32_t unit_length;
  uint16_t version; // fixed value 4
  uint32_t debug_abbrev_offset;
  uint8_t address_size;
  uint64_t type_signature;
  uint32_t type_offset;
} __attribute__((packed));

struct DebugAttrEntry {
  uint128_t abbrev_code;
  void *attribute_values;
};

struct AttributeEntry {
  uint128_t attr_name;
  uint128_t attr_form;
};

struct AbbrevTableEntry {
  uint128_t code;
  uint8_t has_children;

  // last entry is AttributeEntry {0, 0}
  struct AttributeEntry entries[0];
} __attribute__((packed));

struct MetaData {
  std::ostringstream *debug_info;
  size_t debug_info_size;

  std::ostringstream *debug_str;
  size_t debug_str_size;
  size_t debug_str_count;

  std::ostringstream *debug_abbrev;
  size_t debug_abbrev_size;
};

class Value {
 public:
  virtual ~Value() = default;
  virtual auto GetForm() const -> DW_FORM {
    return DW_FORM::DW_FORM_reserved;
  }

  virtual auto ToString() const -> std::string {
    return "";
  }

  // dump debug info as assembly code
  virtual void Generate(MetaData *meta_data) const {
    size_t form = static_cast<size_t>(this->GetForm());
    // .debug_info
    // .uleb128 form
    *(meta_data->debug_info) << "\t.uleb128 " << form << "\n";
    meta_data->debug_info_size += sizeof_uleb128(form);

    // .debug_abbrev
  }

  // dump debug info in json format.
  virtual auto GenerateJson() const -> std::string {
    // not implemented
    throw std::runtime_error("Not implemented");
  }
};

using FormReserved = Value;

// Manual Page 150
/*
 * A string is a sequence of contiguous non-null bytes followed by one null byte. A string may 
 * be represented immediately in the debugging information entry itself (DW_FORM_string), 
 * or may be represented as an offset into a string table contained in the .debug_str section of 
 * the object file (DW_FORM_strp). In the 32-bit DWARF format, the representation of a 
 * DW_FORM_strp value is a 4-byte unsigned offset; in the 64-bit DWARF format, it is an 
 * 8-byte unsigned offset (see Section 7.4).
 */

class FormString: public Value {
 public:
  FormString(const std::string &str) : str_(str) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_string;
  }

  auto ToString() const -> std::string override {
    return EncodeString(this->str_);
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev
    *(meta_data->debug_abbrev) << "\t.uleb128 " 
      << static_cast<size_t>(DW_FORM::DW_FORM_string) << "\n";
    meta_data->debug_abbrev_size +=
      sizeof_uleb128(static_cast<size_t>(DW_FORM::DW_FORM_string));

    // .debug_info
    assert(str_.size() >= 2);
    *(meta_data->debug_info) << "\t.string \"" << EncodeString(this->str_) << "\"\n";
    meta_data->debug_info_size += str_.size() + 1;
  }

 private:
  std::string str_;
};

struct DwarfOperation {
 public:
  std::string operands_[2];
  size_t num_operand_;
  DW_OP opcode_{DW_OP::DW_OP_breg0};

  DwarfOperation(DW_OP opcode): num_operand_(0),
    opcode_(opcode) {
    this->ComputeSize();
    assert(this->size_ == 1);
  }
  DwarfOperation(DW_OP opcode, const std::string &operand):
    num_operand_(1), opcode_(opcode) {
    this->operands_[0] = operand;
    this->ComputeSize();
  }

  DwarfOperation(DW_OP opcode, const std::string &operand1, const std::string &operand2):
    num_operand_(2), opcode_(opcode) {
    this->operands_[0] = operand1;
    this->operands_[1] = operand2;
    this->ComputeSize();
  }

  auto GetSize() const -> size_t {
    // .byte opcode
    // (other operands)
    return this->size_;
  }

  // Manually set pointer size to 64-bit or 32-bit.
  // Default to 64-bit.
  auto SetM64(bool m64) -> DwarfOperation & {
    this->m64_ = m64;
    return *this;
  }

  void Generate(MetaData *metadata) const; 

  // Should be called after one of opcode_, 
  // operands_[0], operands_[1] is set.
  void ComputeSize();

 private:
  size_t size_;
  bool m64_{true};

  static auto ToSizeType(const std::string &str) -> size_t {
    size_t size = 0;
    sscanf(str.c_str(), "%lu", &size);
    return size;
  }

  static auto ToLong(const std::string &str) -> long {
    long ret = 0;
    sscanf(str.c_str(), "%ld", &ret);
    return ret;
  }
};

// [Page 148]
// This is an unsigned LEB128 length followed by 
// the number of information bytes specified by 
// the length (DW_FORM_exprloc). The information 
// bytes contain a DWARF expression (see 
// Section 2.5) or location description (see Section 2.6).
class FormExprLoc: public Value {
 public:
  FormExprLoc() = default;
  FormExprLoc(const std::vector<DwarfOperation> &operations): operations_(operations) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_exprloc;
  }

  auto ToString() const -> std::string override {
    // FIXME: FormExprLoc convertion to string.
    return "";
  }

  void Generate(MetaData *meta_data) const override {
    // A DWARF expression is stored in a block of contiguous bytes. 
    // The bytes form a sequence of operations. Each 
    // operation is a 1-byte code that identifies that operation, 
    // followed by zero or more bytes of additional data. 

    size_t length = 0;
    for (const auto &op : this->operations_) {
      length += op.GetSize();
    }
    
    // .debug_info 
    // .uleb128 length
    *(meta_data->debug_info) << "\t.uleb128 " << length << "\n";
    meta_data->debug_info_size += sizeof_uleb128(length);

    // .debug_abbrev
    size_t form = static_cast<size_t>(DW_FORM::DW_FORM_exprloc);
    *(meta_data->debug_abbrev) << "\t.uleb128 " << form << "\n";
    meta_data-> debug_abbrev_size += sizeof_uleb128(form);

    for (const auto &op : this->operations_) {
      op.Generate(meta_data);
    }
  }

 private:
  std::vector<DwarfOperation> operations_;
};

class FormRefAddr: public Value {
 public:
  FormRefAddr(const std::string &ref) : ref_(ref) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_ref_addr;
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev 
    size_t form = static_cast<size_t>(DW_FORM::DW_FORM_ref_addr);
    *(meta_data->debug_abbrev) << "\t.uleb128 " << form << "\n";
    meta_data->debug_abbrev_size += sizeof_uleb128(form);

    // .debug_info
    *(meta_data->debug_info) << "\t.long " << ref_ << " - .Ldebug_info0\n";
    meta_data->debug_info_size += sizeof(uint32_t);
  }

 private:
  std::string ref_;
};

class FormStrp: public Value {
 public:
  FormStrp(const std::string &str) : str_(str) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_strp;
  }

  auto ToString() const -> std::string override {
    return EncodeString(this->str_);
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev
    *(meta_data->debug_abbrev) << "\t.uleb128 " 
      << static_cast<size_t>(DW_FORM::DW_FORM_strp) << "\n";
    meta_data->debug_abbrev_size +=
      sizeof_uleb128(static_cast<size_t>(DW_FORM::DW_FORM_strp));

    // .debug_str
    *(meta_data->debug_str) << ".LASF" << meta_data->debug_str_count << ":\n";
    *(meta_data->debug_str) << "\t.string \"" << str_ << "\"\n";

    // .debug_info
    *(meta_data->debug_info) << "\t.long " << ".LASF" << meta_data->debug_str_count << " - "
      << ".Ldebug_str0\n";
    meta_data->debug_info_size += 4;

    meta_data->debug_str_count++;
  }

  auto GenerateJson() const -> std::string override {
    // .debug_abbrev
    std::ostringstream oss;
    oss << "\t\"form\": " << "DW_FORM_strp" << ",\n";
    oss << "\t\"data\": " << str_ << ",\n";

    return oss.str();
  }

 private:
  std::string str_;
};

class FormData1: public Value {
 public:
  FormData1(uint8_t data) : data_(std::to_string(static_cast<uint32_t>(data))) {}
  FormData1(const std::string &data) : data_(data) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_data1;
  }

  auto ToString() const -> std::string override {
    return data_;
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev
    *(meta_data->debug_abbrev) << "\t.uleb128 " << static_cast<size_t>(DW_FORM::DW_FORM_data1) << "\n";
    meta_data->debug_abbrev_size +=
      sizeof_uleb128(static_cast<size_t>(DW_FORM::DW_FORM_data1));

    // .debug_info
    *(meta_data->debug_info) << "\t.byte " << data_ << "\n";
    meta_data->debug_info_size += sizeof(uint8_t);
  }

 private:
  std::string data_;
};

class FormData2: public Value {
 public:
  FormData2(uint16_t data) : data_(std::to_string(data)) {}
  FormData2(const std::string &data) : data_(data) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_data2;
  }

  auto ToString() const -> std::string override {
    return (data_);
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev
    size_t form = static_cast<size_t>(DW_FORM::DW_FORM_data2);
    *(meta_data->debug_abbrev) << "\t.uleb128 " << form << "\n";
    meta_data->debug_abbrev_size += sizeof_uleb128(form);

    // .debug_info
    *(meta_data->debug_info) << "\t.value " << data_ << "\n";
    meta_data->debug_info_size += sizeof(uint16_t);
  }

 private:
  std::string data_;
};

class FormData4: public Value {
 public:
  FormData4(uint32_t data) : data_(std::to_string(data)) {}
  FormData4(const std::string &data): data_(data) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_data4;
  }

  auto ToString() const -> std::string override {
    return data_;
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev
    size_t form = static_cast<size_t>(DW_FORM::DW_FORM_data4);
    *(meta_data->debug_abbrev) << "\t.uleb128 " << form << "\n";
    meta_data->debug_abbrev_size += sizeof_uleb128(form);

    // .debug_info
    *(meta_data->debug_info) << "\t.long " << data_ << "\n";
    meta_data->debug_info_size += sizeof(uint32_t);
  }

 private:
  std::string data_;
};

class FormData8: public Value {
 public:
  FormData8(uint64_t data) : data_(std::to_string(data)) {}
  FormData8(const std::string &data) : data_(data) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_data8;
  }

  auto ToString() const -> std::string override {
    return data_;
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev
    size_t form = static_cast<size_t>(DW_FORM::DW_FORM_data8);
    *(meta_data->debug_abbrev) << "\t.uleb128 " << form << "\n";
    meta_data->debug_abbrev_size += sizeof_uleb128(form);

    // .debug_info
    *(meta_data->debug_info) << "\t.quad " << data_ << "\n";
    meta_data->debug_info_size += sizeof(uint64_t);
  }

  auto GenerateJson() const -> std::string override {
    // .debug_abbrev
    std::ostringstream oss;
    oss << "\t\"form\": DW_FORM_data8, \n";
    oss << "\t\"data\": " << data_ << ",\n";

    return oss.str();
  }

 private:
  std::string data_;
};

class FormSecOffset: public Value {
 public:
  FormSecOffset(uint32_t offset) : offset_(std::to_string(offset)) {}
  FormSecOffset(const std::string &offset) : offset_(offset) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_sec_offset;
  }

  auto ToString() const -> std::string override {
    return offset_;
  }

  void Generate(MetaData *meta_data) const override {
    // .debug_abbrev
    size_t form = static_cast<size_t>(DW_FORM::DW_FORM_sec_offset);
    *(meta_data->debug_abbrev) << "\t.uleb128 " << form << "\n";
    meta_data->debug_abbrev_size += sizeof_uleb128(form);

    // .debug_info
    *(meta_data->debug_info) << "\t.long " << offset_ << "\n";
    meta_data->debug_info_size += sizeof(uint32_t);
  }

 private:
  std::string offset_;
};

class FormAddr: public Value {
 public:
  FormAddr(uint64_t addr, bool m64) : data_(std::to_string(addr)), m64_(m64) {}
  FormAddr(const std::string &label, bool m64): data_(label), m64_(m64) {}

  auto GetForm() const -> DW_FORM override {
    return DW_FORM::DW_FORM_addr;
  }

  auto ToString() const -> std::string override {
    return data_;
  }

  void Generate(MetaData *metadata) const override {
    // .debug_abbrev
    size_t form = static_cast<size_t>(this->GetForm());
    *(metadata->debug_abbrev) << "\t.uleb128 " << form << "\n";
    metadata->debug_abbrev_size += sizeof_uleb128(form);
    // .debug_info
    if (m64_) {
      (*metadata->debug_info) << "\t.quad " << data_ << "\n";
      metadata->debug_info_size += 8;
    } else {
      (*metadata->debug_info) << "\t.long " << data_ << "\n";
      metadata->debug_info_size += 4;
    }
  }

 private:
  std::string data_;
  // if true, 64-bit address
  bool m64_;
};

struct Attribute {
 public:
  DW_AT attr_name;
  std::shared_ptr<Value> attr_value;

  Attribute(DW_AT attr_name, std::shared_ptr<Value> attr_value)
   : attr_name(attr_name), attr_value(attr_value) {}

  auto GetForm() -> DW_FORM {
    return attr_value->GetForm();
  }
};

struct DebugInfoEntry {
 public:
  uint128_t abbrev_code_;
  DW_TAG tag_;
  bool children_;
  std::vector<Attribute> attributes_;

  DebugInfoEntry() {
    this->label_  = DebugInfoEntry::instances_++;
  }

  auto GetIndex() const -> size_t { return this->label_; }
  auto GetLabel() const -> std::string {
    std::ostringstream os;
    os << ".Ldebug_entry" << this->label_;
    return os.str();
  }

  auto SetTag(DW_TAG tag) -> DebugInfoEntry & {
    tag_ = tag;
    return *this;
  }

  auto SetChildren(bool children) -> DebugInfoEntry & {
    children_ = children;
    return *this;
  }

  auto SetAbbrevCode(size_t abbrev_code) -> DebugInfoEntry & {
    this->abbrev_code_[0] = abbrev_code;
    this->abbrev_code_[1] = 0;
    return *this;
  }

  auto AddAttribute(Attribute attr) -> DebugInfoEntry & {
    attributes_.push_back(attr);
    return *this;
  }
 private:
  size_t label_;
  static size_t instances_;
};

class DebugInfo {
 public:
  DebugInfo(bool m64, bool little_endian) : m64_(m64), little_endian_(little_endian) {}

  auto GetEntries() -> std::vector<DebugInfoEntry *> & {
    return entries_;
  }

  auto GetPointerSize() const -> size_t {
    return m64_ ? 8 : 4;
  }

  auto AddEntry(DebugInfoEntry *entry) -> DebugInfo & {
    assert(entry != nullptr);
    entries_.push_back(entry);
    return *this;
  }

  auto Generate(std::ostream &os) const -> std::ostream &;

 private:
  bool m64_; 
  bool little_endian_;
  std::vector<DebugInfoEntry *> entries_;
};

// Use a tree to manage debug info entry.

struct DIETreeNode {
 public:
  // FIXME: consider using a shared_ptr
  DebugInfoEntry *entry_{nullptr};
  std::vector<std::shared_ptr<DIETreeNode>> children_;

  auto HasChildren() const -> bool {
    return !children_.empty();
  }

  auto AddChild(std::shared_ptr<DIETreeNode> child) -> DIETreeNode & {
    children_.push_back(child);
    return *this;
  }

  // 'flatten' the tree structure so that DebugInfo can generate
  void PrepareForGeneration(DebugInfo *debug_info) const {
    assert(this->entry_ != nullptr);
    assert(debug_info != nullptr);

    if (this->HasChildren()) {
      this->entry_->SetChildren(true);
      debug_info->AddEntry(this->entry_);
      
      DIETreeNode *last = nullptr;
      for (const auto &child : children_) {
        child->PrepareForGeneration(debug_info);
        last = child.get();
      }
      last->entry_->AddAttribute({
        DW_AT::DW_AT_reserved,
        std::make_shared<FormReserved>()
      });
    } else {
      debug_info->AddEntry(this->entry_);
    }
  }
};

class DIETree {
 public:
  DIETree(bool m64, bool little_endian) : debug_info_(m64, little_endian) {}

  auto SetRoot(std::shared_ptr<DIETreeNode> root) -> DIETree & {
    assert(root != nullptr);
    root_ = root;
    root_->PrepareForGeneration(&debug_info_);
    return *this;
  }

  // disallow copy and move 
  DIETree(const DIETree &) = delete;
  DIETree(DIETree &&) = delete;

  auto Generate(std::ostream &os) const -> std::ostream & {
    debug_info_.Generate(os);
    return os;
  }

 private:
  std::shared_ptr<DIETreeNode> root_;
  // used during generation
  DebugInfo debug_info_;
};

} // namespace Dwarf

#endif // __DWARF_H__
