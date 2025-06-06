.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #4
main$L100: 
         mov r0, #8
         bl malloc
         mov r10, r0
         str r10, [fp, #-36]
         ldr r9, [fp, #-36]
         add r0, r9, #0
         ldr r1, =C$f
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #4
         ldr r1, =C$m
         str r1, [r0]
         ldr r9, [fp, #-36]
         ldr r2, [r9, #0]
         mov r1, #0
         ldr r9, [fp, #-36]
         mov r0, r9
         blx r2
         ldr r4, [r0, #4]
         ldr r9, [fp, #-36]
         ldr r2, [r9, #0]
         mov r1, #0
         ldr r9, [fp, #-36]
         mov r0, r9
         blx r2
         blx r4
         ldr r9, [fp, #-36]
         ldr r2, [r9, #0]
         mov r1, #1
         ldr r9, [fp, #-36]
         mov r0, r9
         blx r2
         ldr r4, [r0, #4]
         ldr r9, [fp, #-36]
         ldr r2, [r9, #0]
         mov r1, #1
         ldr r9, [fp, #-36]
         mov r0, r9
         blx r2
         blx r4
         mov r0, #0
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
         mov r0, #0
         bl putint
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: C^f

.balign 4
.global C$f
.section .text

C$f:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #4
C$f$L105: 
         mov r10, r1
         str r10, [fp, #-36]
         mov r0, #8
         bl malloc
         mov r4, r0
         add r0, r4, #0
         ldr r1, =C$f
         str r1, [r0]
         add r0, r4, #4
         ldr r1, =C$m
         str r1, [r0]
         mov r0, #8
         bl malloc
         add r1, r0, #4
         ldr r2, =C1$m
         str r2, [r1]
         add r1, r0, #0
         ldr r2, =C$f
         str r2, [r1]
         mov r1, #0
         ldr r9, [fp, #-36]
         cmp r9, r1
         bgt C$f$L102
C$f$L103: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
C$f$L102: 
         mov r0, r4
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: C1^m

.balign 4
.global C1$m
.section .text

C1$m:
         push {r4-r10, fp, lr}
         add fp, sp, #32
C1$m$L100: 
         mov r0, #1
         bl putint
         mov r0, #10
         bl putch
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
