// .debug_info
// # COMPILE_UNIT
// .Lentry0:
// DW_AT_language DW_LANG_C11
// DW_AT_name __FILE__
// DW_AT_comp_dir __DIR__
// DW_AT_low_pc 0x0
// DW_AT_high_pc .L0

// # LOCAL_SYMBOLS
// .Lentry1:
// DW_TAG_SUBPROGRAM
//  DW_AT_external 1
//  DW_AT_NAME _start
//  DW_AT_decl_file __FILE__
//  DW_AT_decl_line __LINE__
//  DW_AT_decl_column 0
//  DW_AT_low_pc 0x0
//  DW_AT_high_pc .L1
//  (DW_AT_frame_base)
//  DW_AT_sibling .Lentry3
// .Lentry2:
//  DW_TAG_VARIABLE
//   DW_AT_name pt
//   DW_AT_decl_file __FILE__
//   DW_AT_decl_line __LINE__
//   DW_AT_decl_column 0
//   DW_AT_type .Lentry3
//   DW_AT_location 
// .Lentry3:
//   DW_TAG_pointer_type
//   DW_AT_type <int>
//   DW_AT_byte_size <sizeof(void *)> 
// .Lentry4
//   DW_TAG_base_type
//   DW_AT_name int
//   DW_AT_encoding DW_ATE_signed
//   DW_AT_byte_size <sizeof(int)>

void _start() {
  int *pt;
  pt = 0x0;

  // should crash
  *pt = 0x0;

  // .L1 
}
  // .L0  
