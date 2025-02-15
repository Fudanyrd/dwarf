// Example usage of dwarf package.
// Will generate assembly code of this debug info table:

// DW_TAG_compile_unit
// DW_CHILDREN_no
// DW_AT_name DW_FORM_string "dw-example.cc"
// DW_AT_producer DW_FORM_string "g++ (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3"
// DW_AT_language DW_FORM_data1 DW_LANG_C_plus_plus
// DW_AT_low_pc DW_FORM_addr .Ltext0:
// DW_AT_high_pc DW_FORM_addr .Letext0:

// This correspond to the following c source code:
// It should be put at /home/liuyu/hello.c
/*
#include <syscall.h>
void _start() {
	register long rax asm("rax") = SYS_write;
	register long rdi asm("rdi") = 1;
	register long rsi asm("rsi") = "Hello world!\n";
	register long rdx asm("rdx") = 14;
	
	asm volatile("syscall" : "+r"(rax)
	   : "r"(rax), "r"(rdi), "r"(rsi), "r"(rdx)
	   : "memory");	

	rax = SYS_exit;	
	rdi = 0;
	asm volatile("syscall" : "+r"(rax)
	   : "r"(rax), "r"(rdi)
	   : "memory");	
}
*/

// And its assembly code without debug info:
// The line number are kept for reference.
/*
	.section .text
.Ltext0:
	.globl _start
_start:
  .file 1 "hello.c"
  .loc 1 3 4
	movq $1, %rax
  .loc 1 4 4
	movq $1, %rdi
  .loc 1 5 4
	leaq .LC0, %rsi
  .loc 1 6 4
	movq $14, %rdx
  .loc 1 8 4
	syscall
  .loc 1 12 4
	movq $60, %rax
  .loc 1 13 4
	movq $0, %rdi
  .loc 1 14 4
  syscall
  .loc 1 17 4
.Letext0:
  .section .data
.LC0:
  .string "Hello world!\n"
*/

#include <src/dwarf.h>
#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>

using Dwarf::DebugInfo;
using Dwarf::DebugInfoEntry;
using Dwarf::DwarfOperation;
using Dwarf::Attribute;
using Dwarf::DW_AT;
using Dwarf::DW_ATE;
using Dwarf::DW_TAG;
using Dwarf::DW_FORM;
using Dwarf::DW_OP;
using Dwarf::FormAddr;
using Dwarf::FormData1;
using Dwarf::FormData2;
using Dwarf::FormData4;
using Dwarf::FormData8;
using Dwarf::FormExprLoc;
using Dwarf::FormRefAddr;
using Dwarf::FormReserved;
using Dwarf::FormSecOffset;
using Dwarf::FormString;
using Dwarf::FormStrp;
using Dwarf::Value;

template<typename T>
auto as_ref(std::shared_ptr<T> ptr) -> T & {
  return *ptr;
}

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
    .SetChildren(true)
    .AddAttribute({
      DW_AT::DW_AT_name, 
      std::make_shared<FormStrp>("hello.c")
    })
    .AddAttribute({
      DW_AT::DW_AT_comp_dir,
      std::make_shared<FormStrp>("/home/liuyu")
    })
    .AddAttribute({
      DW_AT::DW_AT_producer, 
      std::make_shared<FormStrp>("g++ (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3")
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
    })
    .AddAttribute({
      DW_AT::DW_AT_stmt_list,
      std::make_shared<FormSecOffset>("0")
    });
  
  DebugInfoEntry func_start;
  func_start
    .SetChildren(true)
    .SetTag(DW_TAG::DW_TAG_subprogram)
    .AddAttribute({
      DW_AT::DW_AT_name,
      std::make_shared<FormStrp>("_start")
    })
    .AddAttribute({
      DW_AT::DW_AT_low_pc,
      std::make_shared<FormAddr>("_start", m64)
    })
    .AddAttribute({
      DW_AT::DW_AT_high_pc,
      std::make_shared<FormAddr>(".Letext0", m64)
    })
    .AddAttribute({
      DW_AT::DW_AT_decl_file,
      std::make_shared<FormStrp>("hello.c")
    })
    .AddAttribute({
      DW_AT::DW_AT_decl_line,
      std::make_shared<FormData2>("2")
    })
    .AddAttribute({
      DW_AT::DW_AT_external,
      std::make_shared<FormData1>("1")
    })
    .AddAttribute({
      DW_AT::DW_AT_decl_column,
      std::make_shared<FormData1>("0")
    });
    
  // Add a signed integer type named "long".
  DebugInfoEntry type_long;
  type_long
    .SetTag(DW_TAG::DW_TAG_base_type)
    .SetChildren(false)
    .AddAttribute({
      DW_AT::DW_AT_name,
      std::make_shared<FormStrp>("long")
    })
    .AddAttribute({
      DW_AT::DW_AT_encoding,
      std::make_shared<FormData1>(static_cast<uint8_t>(DW_ATE::DW_ATE_signed))
    })
    .AddAttribute({
      DW_AT::DW_AT_byte_size,
      std::make_shared<FormData1>("8")
    });
  
  // Add an unsigned integer type size_t
  DebugInfoEntry type_size_t;
  type_size_t
    .SetTag(DW_TAG::DW_TAG_base_type)
    .SetChildren(false)
    .AddAttribute({
      DW_AT::DW_AT_name,
      std::make_shared<FormStrp>("size_t")
    })
    .AddAttribute({
      DW_AT::DW_AT_encoding,
      std::make_shared<FormData1>(static_cast<uint8_t>(DW_ATE::DW_ATE_unsigned))
    })
    .AddAttribute({
      DW_AT::DW_AT_byte_size,
      std::make_shared<FormData1>("16")
    });
  
  // add entry for variables.
  DebugInfoEntry var_rax;
  std::vector<DwarfOperation> expr = {
    DwarfOperation(DW_OP::DW_OP_reg0), // in register rax.
  };
  var_rax 
    .SetChildren(false)
    .SetTag(DW_TAG::DW_TAG_variable)
    .AddAttribute({
      DW_AT::DW_AT_name, 
      std::make_shared<FormStrp>("rax")
    })
    .AddAttribute({
      DW_AT::DW_AT_decl_file,
      std::make_shared<FormStrp>("hello.c")
    })
    .AddAttribute({
      DW_AT::DW_AT_decl_line,
      std::make_shared<FormData1>("3")
    })
    .AddAttribute({
      DW_AT::DW_AT_type,
      std::make_shared<FormRefAddr>(type_long.GetLabel())
    })
    .AddAttribute({
      DW_AT::DW_AT_location,
      std::make_shared<FormExprLoc>(expr)
    });
  
  DebugInfoEntry var_rdi;
  expr = {
    DwarfOperation(DW_OP::DW_OP_reg5), // in register rdi.
  };
  var_rdi
    .SetChildren(false)
    .SetTag(DW_TAG::DW_TAG_variable)
    .AddAttribute({
      DW_AT::DW_AT_name, 
      std::make_shared<FormStrp>("rdi")
    })
    .AddAttribute({
      DW_AT::DW_AT_decl_file,
      std::make_shared<FormStrp>("hello.c")
    })
    .AddAttribute({
      DW_AT::DW_AT_decl_line,
      std::make_shared<FormData1>("5")
    })
    .AddAttribute({
      DW_AT::DW_AT_type,
      std::make_shared<FormRefAddr>(type_long.GetLabel())
    })
    .AddAttribute({
      DW_AT::DW_AT_location,
      std::make_shared<FormExprLoc>(expr)
    });


  // FIXME:
  // Adding other variables: rdi, rsi, rdx
  // will be left for you as an exercise.
  // Hint: $rdi -> DW_OP_reg5,
  // $rsi -> DW_OP_reg4, 
  // $rdx -> DW_OP_reg1



  // The debug info entries are organized in a tree structure.
  func_start
    .AddAttribute({
      DW_AT::DW_AT_sibling,
      std::make_shared<FormRefAddr>(type_long.GetLabel())
    });
  type_long
    .AddAttribute({
      DW_AT::DW_AT_sibling,
      std::make_shared<FormRefAddr>(type_size_t.GetLabel())
    });
  
  // the parent of var_rax is func_start.
  // and there is no more children of func_start.
  var_rax
    .AddAttribute({
      DW_AT::DW_AT_reserved,
      std::make_shared<FormReserved>()
    });

  DebugInfo info(m64, little);
  info.AddEntry(&comp_unit);
  info.AddEntry(&func_start);
  info.AddEntry(&var_rdi);
  info.AddEntry(&var_rax);
  info.AddEntry(&type_long);
  info.AddEntry(&type_size_t);
  info.Generate(std::cout);
  return 0;
}
