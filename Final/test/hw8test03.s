.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L118: 
         mov r0, #28
         bl malloc
         mov r4, r0
         mov r0, #6
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
         add r0, r4, #20
         mov r1, #5
         str r1, [r0]
         add r0, r4, #24
         mov r1, #6
         str r1, [r0]
         mov r0, #28
         bl malloc
         mov r6, r0
         mov r0, #6
         str r0, [r6]
         add r0, r6, #4
         mov r1, #1
         str r1, [r0]
         add r0, r6, #8
         mov r1, #2
         str r1, [r0]
         add r0, r6, #12
         mov r1, #3
         str r1, [r0]
         add r0, r6, #16
         mov r1, #4
         str r1, [r0]
         add r0, r6, #20
         mov r1, #5
         str r1, [r0]
         add r0, r6, #24
         mov r1, #6
         str r1, [r0]
         mov r5, #0
         ldr r7, [r4]
         ldr r0, [r6]
         cmp r7, r0
         bne main$L100
main$L101: 
         add r0, r7, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         str r7, [r0]
         mov r1, #4
         add r2, r7, #1
         mov r3, #4
         mul r3, r2, r3
main$L102: 
         cmp r1, r3
         blt main$L103
main$L104: 
         mov r4, r0
main$L105: 
         ldr r0, [r4]
         cmp r5, r0
         blt main$L106
main$L107: 
         mov r0, #10
         bl putch
         mov r0, r5
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L100: 
         mov r0, #-1
         bl exit
main$L103: 
         add r7, r0, r1
         add r2, r4, r1
         ldr r8, [r2]
         add r2, r6, r1
         ldr r2, [r2]
         add r2, r8, r2
         str r2, [r7]
         add r1, r1, #4
         b main$L102
main$L106: 
         ldr r0, [r4]
         cmp r5, r0
         bge main$L114
main$L115: 
         add r0, r5, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r4, r0
         ldr r1, [r0]
         ldr r0, [r6]
         cmp r5, r0
         bge main$L116
main$L117: 
         add r0, r5, #1
         mov r2, #4
         mul r0, r0, r2
         add r0, r6, r0
         ldr r0, [r0]
         add r0, r1, r0
         bl putint
         mov r0, #32
         bl putch
         add r5, r5, #1
         b main$L105
main$L114: 
         mov r0, #-1
         bl exit
main$L116: 
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
