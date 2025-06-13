.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L110: 
         mov r0, #19
main$L102: 
         mov r1, #1
         mov r0, #0
         cmp r1, r0
         bne main$L103
main$L104: 
         mov r0, #3
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L103: 
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
