.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L109: 
         mov r0, #20
         bl malloc
         mov r1, #4
         str r1, [r0]
         add r1, r0, #4
         mov r2, #1
         str r2, [r1]
         add r1, r0, #8
         mov r2, #2
         str r2, [r1]
         add r1, r0, #12
         mov r2, #3
         str r2, [r1]
         add r1, r0, #16
         mov r2, #4
         str r2, [r1]
         mov r4, #0
         mov r1, #0
main$L100: 
         mov r2, #4
         cmp r1, r2
         blt main$L101
main$L102: 
         mov r0, r4
         bl putint
         mov r0, r4
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         ldr r2, [r0]
         cmp r1, r2
         bge main$L107
main$L108: 
         add r2, r1, #1
         mov r3, #4
         mul r2, r2, r3
         add r2, r0, r2
         ldr r2, [r2]
         add r4, r4, r2
         add r1, r1, #1
         b main$L100
main$L107: 
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
