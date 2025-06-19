.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L100: 
         mov r0, #20
         bl malloc
         mov r2, r0
         mov r0, #4
         str r0, [r2]
         add r0, r2, #4
         mov r1, #1
         str r1, [r0]
         add r0, r2, #8
         mov r1, #2
         str r1, [r0]
         add r0, r2, #12
         mov r1, #3
         str r1, [r0]
         add r0, r2, #16
         mov r1, #4
         str r1, [r0]
         mov r1, #0
         mov r0, #0
main$L102: 
         mov r3, #4
         cmp r1, r3
         blt main$L101
main$L103: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         ldr r3, [r2]
         cmp r1, r3
         bge main$L104
main$L105: 
         add r3, r1, #1
         mov r4, #4
         mul r3, r3, r4
         add r3, r2, r3
         ldr r3, [r3]
         add r0, r0, r3
         add r1, r1, #1
         b main$L102
main$L104: 
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
