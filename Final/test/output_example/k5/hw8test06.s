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
         sub sp, sp, #4
C$next$L105: 
         mov r4, r1
         mov r0, #12
         bl malloc
         mov r10, r0
         str r10, [fp, #-36]
         ldr r9, [fp, #-36]
         add r0, r9, #8
         ldr r1, =C$next
         str r1, [r0]
         ldr r10, [fp, #-36]
         str r4, [r10, #0]
         mov r0, #100
         cmp r4, r0
         blt C$next$L102
C$next$L103: 
         ldr r9, [fp, #-36]
         add r1, r9, #0
         mov r0, #0
         sub r0, r0, #1
         str r0, [r1]
C$next$L104: 
         ldr r9, [fp, #-36]
         mov r0, r9
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
C$next$L102: 
         ldr r9, [fp, #-36]
         ldr r2, [r9, #8]
         ldr r9, [fp, #-36]
         mov r0, r9
         add r1, r4, #1
         ldr r9, [fp, #-36]
         add r4, r9, #4
         blx r2
         str r0, [r4]
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
