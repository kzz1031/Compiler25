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
         mov r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: c3^m1

.balign 4
.global c3$m1
.section .text

c3$m1:
         push {r4-r10, fp, lr}
         add fp, sp, #32
c3$m1$L100: 
         mov r0, #4
         bl malloc
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
