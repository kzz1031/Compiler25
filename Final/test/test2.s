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
         mov r1, #1
         mov r0, #2
         add r0, r1, r0
         mov r1, #1
         mov r0, #2
         mul r0, r1, r0
         mov r0, #1
         mov r0, #0
         mov r0, #0
         add r0, r0, #1
         mov r1, #0
         mov r0, #1
         cmp r1, r0
         bne main$L102
main$L103: 
main$L104: 
main$L102: 
         b main$L104

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
