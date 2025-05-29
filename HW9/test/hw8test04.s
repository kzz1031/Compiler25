.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #20
main$L109: 
         mov r0, #28
         bl malloc
         mov r10, r0
         str r10, [fp, #-36]
         mov r0, #6
         ldr r10, [fp, #-36]
         str r0, [r10]
         ldr r9, [fp, #-36]
         add r0, r9, #4
         mov r1, #1
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #8
         mov r1, #2
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #12
         mov r1, #3
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #16
         mov r1, #4
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #20
         mov r1, #5
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #24
         mov r1, #6
         str r1, [r0]
         mov r0, #28
         bl malloc
         mov r10, r0
         str r10, [fp, #-40]
         mov r0, #6
         ldr r10, [fp, #-40]
         str r0, [r10]
         ldr r9, [fp, #-40]
         add r0, r9, #4
         mov r1, #1
         str r1, [r0]
         ldr r9, [fp, #-40]
         add r0, r9, #8
         mov r1, #2
         str r1, [r0]
         ldr r9, [fp, #-40]
         add r0, r9, #12
         mov r1, #3
         str r1, [r0]
         ldr r9, [fp, #-40]
         add r0, r9, #16
         mov r1, #4
         str r1, [r0]
         ldr r9, [fp, #-40]
         add r0, r9, #20
         mov r1, #5
         str r1, [r0]
         ldr r9, [fp, #-40]
         add r0, r9, #24
         mov r1, #6
         str r1, [r0]
         ldr r9, [fp, #-36]
         ldr r4, [r9]
         ldr r9, [fp, #-40]
         ldr r0, [r9]
         cmp r4, r0
         bne main$L100
main$L101: 
         add r0, r4, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         str r4, [r0]
         mov r1, #4
         add r2, r4, #1
         mov r3, #4
         mul r4, r2, r3
         mov r10, r1
         str r10, [fp, #-44]
main$L102: 
         ldr r9, [fp, #-44]
         cmp r9, r4
         blt main$L103
main$L104: 
         ldr r1, [r0]
         mov r2, #1
         cmp r2, r1
         bge main$L105
main$L106: 
         mov r1, #1
         add r1, r1, #1
         mov r2, #4
         mul r1, r1, r2
         add r0, r0, r1
         ldr r1, [r0]
         ldr r9, [fp, #-40]
         mov r10, r9
         str r10, [fp, #-52]
         ldr r9, [fp, #-40]
         ldr r0, [r9]
         mov r2, #0
         cmp r2, r0
         bge main$L107
main$L108: 
         mov r0, #0
         add r0, r0, #1
         mov r2, #4
         mul r0, r0, r2
         ldr r9, [fp, #-52]
         add r0, r9, r0
         ldr r0, [r0]
         add r0, r1, r0
         bl putint
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L100: 
         mov r0, #-1
         bl exit
main$L103: 
         ldr r10, [fp, #-44]
         add r3, r0, r10
         ldr r9, [fp, #-36]
         ldr r10, [fp, #-44]
         add r1, r9, r10
         ldr r2, [r1]
         ldr r9, [fp, #-40]
         ldr r10, [fp, #-44]
         add r1, r9, r10
         ldr r1, [r1]
         add r1, r2, r1
         str r1, [r3]
         ldr r9, [fp, #-44]
         add r10, r9, #4
         str r10, [fp, #-48]
         ldr r9, [fp, #-48]
         mov r10, r9
         str r10, [fp, #-44]
         b main$L102
main$L105: 
         mov r0, #-1
         bl exit
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
