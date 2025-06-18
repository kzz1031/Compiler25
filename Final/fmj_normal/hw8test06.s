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
         mov r0, #12
         bl malloc
         add r1, r0, #8
         ldr r2, =C$next
         str r2, [r1]
         ldr r2, [r0, #8]
         mov r1, #0
         blx r2
         mov r4, r0
main$L100: 
         ldr r0, [r4, #0]
         mov r1, #0
         cmp r0, r1
         bge main$L101
main$L102: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         ldr r0, [r4, #0]
         bl putint
         mov r0, #10
         bl putch
         ldr r4, [r4, #4]
         b main$L100

@ Here's function: C^next

.balign 4
.global C$next
.section .text

C$next:
         push {r4-r10, fp, lr}
         add fp, sp, #32
C$next$L105: 
         str r1, [r4, #0]
         mov r0, #100
         cmp r1, r0
         blt C$next$L102
C$next$L103: 
         add r1, r4, #0
         mov r0, #0
         sub r0, r0, #1
         str r0, [r1]
C$next$L104: 
         mov r0, r4
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
C$next$L102: 
         ldr r2, [r4, #8]
         add r1, r1, #1
         add r5, r4, #4
         mov r0, r4
         blx r2
         str r0, [r5]
         b C$next$L104

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
