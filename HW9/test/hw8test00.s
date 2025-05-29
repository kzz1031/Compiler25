.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #12
main$L105: 
         mov r0, #20
         bl malloc
         mov r10, r0
         str r10, [fp, #-36]
         mov r0, #4
         ldr r10, [fp, #-36]
         str r0, [r10]
         ldr r9, [fp, #-36]
         mov r0, r9
         bl getarray
         mov r10, r0
         str r10, [fp, #-40]
         ldr r9, [fp, #-36]
         ldr r4, [r9]
         add r0, r4, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         mov r10, r0
         str r10, [fp, #-44]
         ldr r10, [fp, #-44]
         str r4, [r10]
         mov r0, #4
         add r1, r4, #1
         mov r2, #4
         mul r2, r1, r2
         mov r3, r0
main$L100: 
         cmp r3, r2
         blt main$L101
main$L102: 
         ldr r9, [fp, #-44]
         mov r4, r9
         mov r1, r4
         ldr r9, [fp, #-40]
         mov r0, r9
         bl putarray
         ldr r0, [r4]
         mov r1, #0
         cmp r1, r0
         bge main$L103
main$L104: 
         mov r0, #0
         add r0, r0, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r4, r0
         ldr r0, [r0]
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         ldr r9, [fp, #-44]
         add r1, r9, r3
         ldr r9, [fp, #-36]
         add r0, r9, r3
         ldr r0, [r0]
         mov r4, #0
         sub r0, r4, r0
         str r0, [r1]
         add r3, r3, #4
         b main$L100
main$L103: 
         mov r0, #-1
         bl exit

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
