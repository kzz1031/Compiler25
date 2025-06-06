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
         mov r4, #-2
         mov r0, #0
         cmp r4, r0
         bgt main$L102
main$L103: 
         mov r0, #0
         sub r0, r0, r4
         bl putint
main$L104: 
         mov r0, #10
         bl putch
         mov r0, r4
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L102: 
         mov r0, r4
         bl putint
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
