.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L102: 
         mov r0, #12
         bl malloc
         mov r4, r0
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
         str r0, [r4, #4]
         add r0, r4, #8
         ldr r1, =C$m1
         str r1, [r0]
         mov r0, #12
         bl malloc
         mov r4, r0
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
         str r0, [r4, #4]
         add r0, r4, #8
         ldr r1, =D$m1
         str r1, [r0]
         mov r0, r4
         ldr r1, [r0, #8]
         blx r1
         ldr r1, [r0]
         mov r2, #0
         cmp r2, r1
         bge main$L100
main$L101: 
         mov r1, #0
         add r1, r1, #1
         mov r2, #4
         mul r1, r1, r2
         add r0, r0, r1
         ldr r0, [r0]
         bl putint
         mov r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L100: 
         mov r0, #-1
         bl exit

@ Here's function: C^m1

.balign 4
.global C$m1
.section .text

C$m1:
         push {r4-r10, fp, lr}
         add fp, sp, #32
C$m1$L100: 
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
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: D^m1

.balign 4
.global D$m1
.section .text

D$m1:
         push {r4-r10, fp, lr}
         add fp, sp, #32
D$m1$L100: 
         ldr r0, [r0, #4]
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
