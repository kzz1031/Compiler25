.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L105: 
         mov r0, #0
         mov r0, #32
         bl malloc
         mov r5, r0
         mov r0, #7
         str r0, [r5]
         add r0, r5, #4
         mov r1, #6
         str r1, [r0]
         add r0, r5, #8
         mov r1, #3
         str r1, [r0]
         add r0, r5, #12
         mov r1, #0
         str r1, [r0]
         add r0, r5, #16
         mov r1, #5
         str r1, [r0]
         add r0, r5, #20
         mov r1, #9
         str r1, [r0]
         add r0, r5, #24
         mov r1, #1
         str r1, [r0]
         add r0, r5, #28
         mov r1, #2
         str r1, [r0]
         mov r0, #32
         bl malloc
         mov r4, r0
         mov r0, #7
         str r0, [r4]
         add r0, r4, #4
         mov r1, #0
         str r1, [r0]
         add r0, r4, #8
         mov r1, #1
         str r1, [r0]
         add r0, r4, #12
         mov r1, #2
         str r1, [r0]
         add r0, r4, #16
         mov r1, #3
         str r1, [r0]
         add r0, r4, #20
         mov r1, #4
         str r1, [r0]
         add r0, r4, #24
         mov r1, #5
         str r1, [r0]
         add r0, r4, #28
         mov r1, #6
         str r1, [r0]
         mov r1, #7
         mov r0, r5
         bl putarray
         ldr r7, [r5]
         ldr r0, [r4]
         cmp r7, r0
         bne main$L100
main$L101: 
         add r0, r7, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         mov r6, r0
         str r7, [r6]
         mov r0, #4
         add r1, r7, #1
         mov r2, #4
         mul r2, r1, r2
main$L102: 
         cmp r0, r2
         blt main$L103
main$L104: 
         mov r1, #7
         mov r0, r6
         bl putarray
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L100: 
         mov r0, #-1
         bl exit
main$L103: 
         add r3, r6, r0
         add r1, r5, r0
         ldr r7, [r1]
         add r1, r4, r0
         ldr r1, [r1]
         add r1, r7, r1
         str r1, [r3]
         add r0, r0, #4
         b main$L102

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
