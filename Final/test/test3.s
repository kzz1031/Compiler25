.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L107: 
         mov r0, #20
         bl malloc
         mov r5, r0
         mov r0, #4
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
         mov r0, #20
         bl malloc
         mov r4, r0
         mov r0, #4
         str r0, [r4]
         add r0, r4, #4
         mov r1, #9
         str r1, [r0]
         add r0, r4, #8
         mov r1, #8
         str r1, [r0]
         add r0, r4, #12
         mov r1, #7
         str r1, [r0]
         add r0, r4, #16
         mov r1, #6
         str r1, [r0]
         mov r0, #4
         bl malloc
         mov r1, #0
         str r1, [r0]
         ldr r6, [r5]
         ldr r0, [r4]
         cmp r6, r0
         bne main$L100
main$L101: 
         add r0, r6, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         mov r1, r0
         str r6, [r1]
         mov r0, #4
         add r2, r6, #1
         mov r3, #4
         mul r3, r2, r3
main$L102: 
         cmp r0, r3
         blt main$L103
main$L104: 
         ldr r0, [r1]
         mov r2, #0
         cmp r2, r0
         bge main$L105
main$L106: 
         mov r0, #0
         add r0, r0, #1
         mov r2, #4
         mul r0, r0, r2
         add r0, r1, r0
         ldr r0, [r0]
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L100: 
         mov r0, #-1
         bl exit
main$L103: 
         add r6, r1, r0
         add r2, r5, r0
         ldr r7, [r2]
         add r2, r4, r0
         ldr r2, [r2]
         add r2, r7, r2
         str r2, [r6]
         add r0, r0, #4
         b main$L102
main$L105: 
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
