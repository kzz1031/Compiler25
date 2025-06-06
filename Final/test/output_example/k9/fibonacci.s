.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L113: 
         mov r4, #0
         mov r0, #4
         bl malloc
         mov r5, r0
         add r0, r5, #0
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
         mov r6, r0
         mov r0, #0
         cmp r6, r0
         blt main$L105
main$L104: 
         mov r0, #47
         cmp r6, r0
         bgt main$L105
main$L106: 
main$L107: 
main$L110: 
         cmp r4, r6
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
         ldr r2, [r5, #0]
         mov r1, r4
         mov r0, r5
         blx r2
         bl putint
         add r4, r4, #1
         mov r0, #32
         bl putch
         b main$L110

@ Here's function: fib^f

.balign 4
.global fib$f
.section .text

fib$f:
         push {r4-r10, fp, lr}
         add fp, sp, #32
fib$f$L108: 
         mov r4, r1
         mov r5, r0
         mov r0, #0
         cmp r4, r0
         beq fib$f$L105
fib$f$L104: 
         mov r0, #1
         cmp r4, r0
         beq fib$f$L105
fib$f$L106: 
         ldr r2, [r5, #0]
         mov r0, r5
         sub r1, r4, #1
         blx r2
         mov r6, r0
         ldr r2, [r5, #0]
         mov r0, r5
         sub r1, r4, #2
         blx r2
         add r0, r6, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
fib$f$L105: 
         mov r0, r4
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
