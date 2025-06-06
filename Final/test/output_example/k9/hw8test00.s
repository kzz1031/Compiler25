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
         mov r0, #20
         bl malloc
         mov r4, r0
         mov r0, #4
         str r0, [r4]
         mov r0, r4
         bl getarray
         mov r5, r0
         ldr r6, [r4]
         add r0, r6, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         str r6, [r0]
         mov r1, #4
         add r2, r6, #1
         mov r3, #4
         mul r3, r2, r3
main$L100: 
         cmp r1, r3
         blt main$L101
main$L102: 
         mov r4, r0
         mov r1, r4
         mov r0, r5
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
         add r6, r0, r1
         add r2, r4, r1
         ldr r2, [r2]
         mov r7, #0
         sub r2, r7, r2
         str r2, [r6]
         add r1, r1, #4
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
