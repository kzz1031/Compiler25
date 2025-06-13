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
         mov r0, #16
         bl malloc
         mov r4, r0
         mov r0, #4
         bl malloc
         mov r1, #0
         str r1, [r0]
         str r0, [r4, #0]
         mov r1, #1
         str r1, [r4, #4]
         mov r0, #16
         bl malloc
         mov r1, #3
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
         str r0, [r4, #12]
         mov r0, #4
         bl malloc
         mov r1, #0
         str r1, [r0]
         mov r0, #1
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
