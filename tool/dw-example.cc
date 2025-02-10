// Example usage of dwarf package.
// Will generate assembly code of this debug info table:

// DW_TAG_compile_unit
// DW_CHILDREN_no
// DW_AT_name DW_FORM_string "dw-example.cc"
// DW_AT_producer DW_FORM_string "g++ (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
// DW_AT_language DW_FORM_data1 DW_LANG_C_plus_plus
// DW_AT_low_pc DW_FORM_addr .Ltext0:
// DW_AT_high_pc DW_FORM_addr .Letext0:

#include <src/dwarf.h>
#include <iostream>

using Dwarf::DebugInfo;
using Dwarf::DebugInfoEntry;
using Dwarf::Attribute;
using Dwarf::DW_AT;
using Dwarf::DW_TAG;
using Dwarf::DW_FORM;
using Dwarf::FormAddr;
using Dwarf::FormData1;
using Dwarf::FormData2;
using Dwarf::FormData4;
using Dwarf::FormData8;
using Dwarf::FormString;

int main(int argc, char **argv) {
  // get system endianess and pointer size.
  int endian = 0x1;
  bool little = true;
  if (*(char *)&endian) {
    // little endian
  } else {
    little = false;
  } 
  bool m64 = (sizeof(void *) == 8);

  DebugInfoEntry comp_unit;
  comp_unit
    .SetTag(DW_TAG::DW_TAG_compile_unit)
    .SetChildren(false)
    .AddAttribute({
      DW_AT::DW_AT_name, 
      std::make_shared<FormString>(__FILE__)
    })
    .AddAttribute({
      DW_AT::DW_AT_producer, 
      std::make_shared<FormString>("g++ (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3")
    })
    .AddAttribute({
      DW_AT::DW_AT_language, 
      std::make_shared<FormData1>(static_cast<uint8_t>(Dwarf::DW_LANG::DW_LANG_C_plus_plus))
    })
    .AddAttribute({
      DW_AT::DW_AT_low_pc, 
      std::make_shared<FormAddr>(".Ltext0", m64)
    })
    .AddAttribute({
      DW_AT::DW_AT_high_pc, 
      std::make_shared<FormAddr>(".Letext0", m64)
    });
  

  DebugInfo info(m64, little);
  info.AddEntry(comp_unit);
  info.Generate(std::cout);
  return 0;
}
