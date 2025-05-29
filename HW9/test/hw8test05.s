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
         mov r2, #2
         str r2, [r0, #0]
         add r1, r0, #4
         ldr r2, =C$m
         str r2, [r1]
         ldr r1, [r0, #4]
         blx r1
         bl putint
         mov r0, #20
         bl putch
         mov r0, #9
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: C^m

.balign 4
.global C$m
.section .text

C$m:
         push {r4-r10, fp, lr}
         add fp, sp, #32
C$m$L100: 
         ldr r0, [r0, #0]
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
