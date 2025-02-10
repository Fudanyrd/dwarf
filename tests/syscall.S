  .text
  .globl write
  .type write, @function
write:
  movq $1, %rax
  syscall
  ret

  .globl read
  .type read, @function
read:
  movq $0, %rax
  syscall
  ret

  .globl exit
  .type exit, @function
exit:
  movq $60, %rax
  syscall
  ret
