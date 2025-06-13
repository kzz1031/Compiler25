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
         mov r4, #19
         mov r0, #0
         mov r1, #1
         cmp r4, r1
         bgt main$L102
main$L103: 
         bl putch
         mov r0, r4
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L102: 
         mov r0, #1
         b main$L103

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
