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
         mov r4, r0
         mov r0, #4
         str r0, [r4]
         add r0, r4, #4
         mov r1, #1
         str r1, [r0]
         add r0, r4, #8
         mov r1, #2
         str r1, [r0]
         add r0, r4, #12
         mov r1, #3
         str r1, [r0]
         add r0, r4, #16
         mov r1, #4
         str r1, [r0]
         mov r0, #0
         mov r2, #0
main$L100: 
         mov r3, r5
         mov r3, r2
         mov r1, #4
         cmp r3, r1
         blt main$L101
main$L102: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         ldr r1, [r4]
         cmp r3, r1
         bge main$L107
main$L108: 
         add r1, r3, #1
         mov r5, #4
         mul r1, r1, r5
         add r1, r4, r1
         ldr r1, [r1]
         add r0, r0, r1
         add r5, r3, #1
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
