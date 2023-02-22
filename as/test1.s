.globl main
main:
 mov x0, 5
 str x0, [sp, -16]! 
 mov x0, 20
 str x0, [sp, -16]! 
 ldr x0, [sp], 16
 ldr x1, [sp], 16
 add x0, x0, x1
 str x0, [sp, -16]!
 mov x0, 4
 str x0, [sp, -16]! 
 ldr x0, [sp], 16
 ldr x1, [sp], 16
 sub x0, x1, x0
 str x0, [sp, -16]!
 ldr x0, [sp], 16
 ret
