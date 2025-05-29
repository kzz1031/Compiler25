.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #16
main$L113: 
         mov r10, #0
         str r10, [fp, #-36]
         mov r0, #4
         bl malloc
         mov r4, r0
         add r0, r4, #0
         ldr r1, =fib$f
         str r1, [r0]
         mov r0, #69
         bl putch
         mov r0, #110
         bl putch
         mov r0, #116
         bl putch
         mov r0, #101
         bl putch
         mov r0, #114
         bl putch
         mov r0, #32
         bl putch
         mov r0, #116
         bl putch
         mov r0, #104
         bl putch
         mov r0, #101
         bl putch
         mov r0, #32
         bl putch
         mov r0, #110
         bl putch
         mov r0, #117
         bl putch
         mov r0, #109
         bl putch
         mov r0, #98
         bl putch
         mov r0, #101
         bl putch
         mov r0, #114
         bl putch
         mov r0, #32
         bl putch
         mov r0, #111
         bl putch
         mov r0, #102
         bl putch
         mov r0, #32
         bl putch
         mov r0, #116
         bl putch
         mov r0, #101
         bl putch
         mov r0, #114
         bl putch
         mov r0, #109
         bl putch
         mov r0, #58
         bl putch
         bl getint
         mov r10, r0
         str r10, [fp, #-48]
         mov r0, #0
         ldr r9, [fp, #-48]
         cmp r9, r0
         blt main$L105
main$L104: 
         mov r0, #47
         ldr r9, [fp, #-48]
         cmp r9, r0
         bgt main$L105
main$L106: 
main$L107: 
         ldr r9, [fp, #-36]
         mov r10, r9
         str r10, [fp, #-40]
main$L110: 
         ldr r9, [fp, #-40]
         ldr r10, [fp, #-48]
         cmp r9, r10
         blt main$L111
main$L112: 
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L105: 
         mov r0, #0
         sub r0, r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L111: 
         ldr r2, [r4, #0]
         ldr r9, [fp, #-40]
         mov r1, r9
         mov r0, r4
         blx r2
         bl putint
         ldr r9, [fp, #-40]
         add r10, r9, #1
         str r10, [fp, #-44]
         mov r0, #32
         bl putch
         ldr r9, [fp, #-44]
         mov r10, r9
         str r10, [fp, #-40]
         b main$L110

@ Here's function: fib^f

.balign 4
.global fib$f
.section .text

fib$f:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #8
fib$f$L108: 
         mov r10, r1
         str r10, [fp, #-40]
         mov r10, r0
         str r10, [fp, #-36]
         mov r0, #0
         ldr r9, [fp, #-40]
         cmp r9, r0
         beq fib$f$L105
fib$f$L104: 
         mov r0, #1
         ldr r9, [fp, #-40]
         cmp r9, r0
         beq fib$f$L105
fib$f$L106: 
         ldr r9, [fp, #-36]
         ldr r2, [r9, #0]
         ldr r9, [fp, #-36]
         mov r0, r9
         ldr r9, [fp, #-40]
         sub r1, r9, #1
         blx r2
         mov r4, r0
         ldr r9, [fp, #-36]
         ldr r2, [r9, #0]
         ldr r9, [fp, #-36]
         mov r0, r9
         ldr r9, [fp, #-40]
         sub r1, r9, #2
         blx r2
         add r0, r4, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
fib$f$L105: 
         ldr r9, [fp, #-40]
         mov r0, r9
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
