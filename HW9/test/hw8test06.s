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
main$L102: 
         ldr r0, [r4, #0]
         mov r1, #0
         cmp r0, r1
         bge main$L103
main$L104: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L103: 
         ldr r0, [r4, #0]
         bl putint
         mov r0, #10
         bl putch
         ldr r4, [r4, #4]
         b main$L102

@ Here's function: C^next

.balign 4
.global C$next
.section .text

C$next:
         push {r4-r10, fp, lr}
         add fp, sp, #32
C$next$L105: 
         mov r5, r1
         mov r0, #12
         bl malloc
         mov r4, r0
         add r0, r4, #8
         ldr r1, =C$next
         str r1, [r0]
         str r5, [r4, #0]
         mov r0, #100
         cmp r5, r0
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
         mov r0, r4
         add r1, r5, #1
         add r5, r4, #4
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
