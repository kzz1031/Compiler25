.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #24
main$L100: 
         mov r4, #0
         mov r3, #1
         mov r2, #2
         mov r8, #3
         mov r10, #4
         str r10, [fp, #-36]
         mov r10, #5
         str r10, [fp, #-40]
         mov r10, #6
         str r10, [fp, #-44]
         mov r10, #7
         str r10, [fp, #-48]
         mov r7, #8
         mov r10, #9
         str r10, [fp, #-52]
         mov r6, #10
         mov r5, #11
         add r0, r4, r3
         add r0, r0, r2
         add r0, r0, r8
         ldr r10, [fp, #-36]
         add r0, r0, r10
         ldr r10, [fp, #-40]
         add r0, r0, r10
         ldr r10, [fp, #-44]
         add r0, r0, r10
         ldr r10, [fp, #-48]
         add r0, r0, r10
         add r0, r0, r7
         ldr r10, [fp, #-52]
         add r0, r0, r10
         add r0, r0, r6
         add r10, r0, r5
         str r10, [fp, #-56]
         ldr r9, [fp, #-56]
         mul r1, r9, r4
         ldr r9, [fp, #-56]
         mul r0, r9, r3
         add r1, r1, r0
         ldr r9, [fp, #-56]
         mul r0, r9, r2
         add r0, r1, r0
         ldr r9, [fp, #-56]
         mul r1, r9, r8
         add r1, r0, r1
         ldr r9, [fp, #-56]
         ldr r10, [fp, #-36]
         mul r0, r9, r10
         add r1, r1, r0
         ldr r9, [fp, #-56]
         ldr r10, [fp, #-40]
         mul r0, r9, r10
         add r1, r1, r0
         ldr r9, [fp, #-56]
         ldr r10, [fp, #-44]
         mul r0, r9, r10
         add r1, r1, r0
         ldr r9, [fp, #-56]
         ldr r10, [fp, #-48]
         mul r0, r9, r10
         add r0, r1, r0
         ldr r9, [fp, #-56]
         mul r1, r9, r7
         add r1, r0, r1
         ldr r9, [fp, #-56]
         ldr r10, [fp, #-52]
         mul r0, r9, r10
         add r1, r1, r0
         ldr r9, [fp, #-56]
         mul r0, r9, r6
         add r1, r1, r0
         ldr r9, [fp, #-56]
         mul r0, r9, r5
         add r7, r1, r0
         add r0, r4, r3
         ldr r10, [fp, #-40]
         sdiv r1, r2, r10
         add r0, r0, r1
         mul r1, r5, r6
         add r0, r0, r1
         bl putint
         mov r0, #10
         bl putch
         ldr r9, [fp, #-56]
         mov r0, r9
         bl putint
         mov r0, #10
         bl putch
         mov r0, r7
         bl putint
         ldr r9, [fp, #-48]
         ldr r10, [fp, #-44]
         mul r0, r9, r10
         ldr r9, [fp, #-36]
         add r0, r9, r0
         ldr r10, [fp, #-52]
         sub r0, r0, r10
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
