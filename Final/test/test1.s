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
         mov r0, #8
         bl malloc
         add r1, r0, #4
         ldr r2, =D$m
         str r2, [r1]
         ldr r1, [r0, #4]
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: D^m

.balign 4
.global D$m
.section .text

D$m:
         push {r4-r10, fp, lr}
         add fp, sp, #32
D$m$L100: 
         mov r0, #0
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
