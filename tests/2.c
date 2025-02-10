// .debug_info
// # COMPILE_UNIT
// .Lentry0:
// DW_TAG_compile_unit
// DW_AT_producer "$(shell gcc -v)"
// DW_AT_language DW_LANG_C11
// DW_AT_name __FILE__
// DW_AT_comp_dir __DIR__
// DW_AT_low_pc 0x0
// DW_AT_high_pc .Letext

// # local symbols
// .Lentry1:
//  DW_TAG_subprogram
//    DW_AT_external 1
//    DW_AT_name "silly"
//    DW_AT_decl_file __FILE__
//    DW_AT_decl_line __LINE__
//    DW_AT_decl_column 0
//    DW_AT_type 0x4c
//    DW_AT_low_pc silly
//    DW_AT_high_pc .L2
//    (DW_AT_frame_base)
// .Lentry2:
//  DW_TAG_base_type
//    DW_AT_byte_size sizeof(int)
//    DW_AT_encoding DW_ATE_signed
//    DW_AT_name "int"

// .Ltext:
// .globl silly
// .type silly, @function
// silly:
int silly() {
  // movq $0, %rax
  // ret
  return 0;
  // .L1:
}
// .Letext:
