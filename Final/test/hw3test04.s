.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L116: 
         mov r0, #44
         bl malloc
         mov r5, r0
         mov r0, #10
         str r0, [r5]
         add r0, r5, #4
         mov r1, #1
         str r1, [r0]
         add r0, r5, #8
         mov r1, #2
         str r1, [r0]
         add r0, r5, #12
         mov r1, #3
         str r1, [r0]
         add r0, r5, #16
         mov r1, #4
         str r1, [r0]
         add r0, r5, #20
         mov r1, #5
         str r1, [r0]
         add r0, r5, #24
         mov r1, #6
         str r1, [r0]
         add r0, r5, #28
         mov r1, #7
         str r1, [r0]
         add r0, r5, #32
         mov r1, #8
         str r1, [r0]
         add r0, r5, #36
         mov r1, #9
         str r1, [r0]
         add r0, r5, #40
         mov r1, #10
         str r1, [r0]
         mov r0, #16
         bl malloc
         mov r4, r0
         mov r0, #3
         str r0, [r4]
         add r0, r4, #4
         mov r1, #3
         str r1, [r0]
         add r0, r4, #8
         mov r1, #4
         str r1, [r0]
         add r0, r4, #12
         mov r1, #5
         str r1, [r0]
         ldr r0, [r4]
         mov r1, #0
         cmp r1, r0
         bge main$L100
main$L101: 
         mov r0, #0
         add r0, r0, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r4, r0
         ldr r1, [r0]
         ldr r0, [r5]
         cmp r1, r0
         bge main$L102
main$L103: 
         add r0, r1, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r5, r0
         ldr r0, [r0]
         mov r1, #1
         cmp r0, r1
         blt main$L106
main$L107: 
         mov r5, r4
main$L108: 
         mov r0, #0
         mov r2, #9
         mov r1, #10
         cmp r2, r1
         bgt main$L111
main$L112: 
         ldr r1, [r5]
         cmp r0, r1
         bge main$L114
main$L115: 
         add r0, r0, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r5, r0
         ldr r0, [r0]
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L100: 
         mov r0, #-1
         bl exit
main$L102: 
         mov r0, #-1
         bl exit
main$L106: 
         b main$L108
main$L111: 
         mov r0, #1
         b main$L112
main$L114: 
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
