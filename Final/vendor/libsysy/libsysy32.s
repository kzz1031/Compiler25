	.cpu cortex-a72
	.arch armv8-a
	.fpu neon-fp-armv8
	.arch_extension crc
	.eabi_attribute 28, 1
	.eabi_attribute 20, 1
	.eabi_attribute 21, 1
	.eabi_attribute 23, 3
	.eabi_attribute 24, 1
	.eabi_attribute 25, 1
	.eabi_attribute 26, 2
	.eabi_attribute 30, 6
	.eabi_attribute 34, 1
	.eabi_attribute 18, 4
	.file	"libsysy32.c"
	.text
	.global	_sysy_start
	.bss
	.align	2
	.type	_sysy_start, %object
	.size	_sysy_start, 8
_sysy_start:
	.space	8
	.global	_sysy_end
	.align	2
	.type	_sysy_end, %object
	.size	_sysy_end, 8
_sysy_end:
	.space	8
	.global	_sysy_l1
	.align	2
	.type	_sysy_l1, %object
	.size	_sysy_l1, 4096
_sysy_l1:
	.space	4096
	.global	_sysy_l2
	.align	2
	.type	_sysy_l2, %object
	.size	_sysy_l2, 4096
_sysy_l2:
	.space	4096
	.global	_sysy_h
	.align	2
	.type	_sysy_h, %object
	.size	_sysy_h, 4096
_sysy_h:
	.space	4096
	.global	_sysy_m
	.align	2
	.type	_sysy_m, %object
	.size	_sysy_m, 4096
_sysy_m:
	.space	4096
	.global	_sysy_s
	.align	2
	.type	_sysy_s, %object
	.size	_sysy_s, 4096
_sysy_s:
	.space	4096
	.global	_sysy_us
	.align	2
	.type	_sysy_us, %object
	.size	_sysy_us, 4096
_sysy_us:
	.space	4096
	.global	_sysy_idx
	.align	2
	.type	_sysy_idx, %object
	.size	_sysy_idx, 4
_sysy_idx:
	.space	4
	.section	.rodata
	.align	2
.LC0:
	.ascii	"%d\000"
	.text
	.align	1
	.global	getint
	.syntax unified
	.thumb
	.thumb_func
	.type	getint, %function
getint:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #8
	add	r7, sp, #0
	ldr	r2, .L4
.LPIC1:
	add	r2, pc
	ldr	r3, .L4+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #4]
	mov	r3, #0
	mov	r3, r7
	mov	r1, r3
	ldr	r3, .L4+8
.LPIC0:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldr	r3, [r7]
	ldr	r1, .L4+12
.LPIC2:
	add	r1, pc
	ldr	r2, .L4+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #4]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L3
	bl	__stack_chk_fail(PLT)
.L3:
	mov	r0, r3
	add	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L5:
	.align	2
.L4:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC1+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC0+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC2+4)
	.size	getint, .-getint
	.section	.rodata
	.align	2
.LC1:
	.ascii	"%c\000"
	.text
	.align	1
	.global	getch
	.syntax unified
	.thumb
	.thumb_func
	.type	getch, %function
getch:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #8
	add	r7, sp, #0
	ldr	r2, .L9
.LPIC4:
	add	r2, pc
	ldr	r3, .L9+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #4]
	mov	r3, #0
	add	r3, r7, #3
	mov	r1, r3
	ldr	r3, .L9+8
.LPIC3:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldrb	r3, [r7, #3]	@ zero_extendqisi2
	ldr	r1, .L9+12
.LPIC5:
	add	r1, pc
	ldr	r2, .L9+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #4]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L8
	bl	__stack_chk_fail(PLT)
.L8:
	mov	r0, r3
	add	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L10:
	.align	2
.L9:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC4+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC1-(.LPIC3+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC5+4)
	.size	getch, .-getch
	.align	1
	.global	getarray
	.syntax unified
	.thumb
	.thumb_func
	.type	getarray, %function
getarray:
	@ args = 0, pretend = 0, frame = 24
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #24
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r2, .L16
.LPIC8:
	add	r2, pc
	ldr	r3, .L16+4
	ldr	r3, [r2, r3]
	ldr	r3, [r3]
	str	r3, [r7, #20]
	mov	r3, #0
	add	r3, r7, #12
	mov	r1, r3
	ldr	r3, .L16+8
.LPIC6:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #4]
	str	r2, [r3]
	mov	r3, #0
	str	r3, [r7, #16]
	b	.L12
.L13:
	ldr	r3, [r7, #16]
	add	r3, r3, #1
	lsl	r3, r3, #2
	ldr	r2, [r7, #4]
	add	r3, r3, r2
	mov	r1, r3
	ldr	r3, .L16+12
.LPIC7:
	add	r3, pc
	mov	r0, r3
	bl	__isoc99_scanf(PLT)
	ldr	r3, [r7, #16]
	add	r3, r3, #1
	str	r3, [r7, #16]
.L12:
	ldr	r3, [r7, #12]
	ldr	r2, [r7, #16]
	cmp	r2, r3
	blt	.L13
	ldr	r3, [r7, #12]
	ldr	r1, .L16+16
.LPIC9:
	add	r1, pc
	ldr	r2, .L16+4
	ldr	r2, [r1, r2]
	ldr	r1, [r2]
	ldr	r2, [r7, #20]
	eors	r1, r2, r1
	mov	r2, #0
	beq	.L15
	bl	__stack_chk_fail(PLT)
.L15:
	mov	r0, r3
	add	r7, r7, #24
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L17:
	.align	2
.L16:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC8+4)
	.word	__stack_chk_guard(GOT)
	.word	.LC0-(.LPIC6+4)
	.word	.LC0-(.LPIC7+4)
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC9+4)
	.size	getarray, .-getarray
	.align	1
	.global	putint
	.syntax unified
	.thumb
	.thumb_func
	.type	putint, %function
putint:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r1, [r7, #4]
	ldr	r3, .L19
.LPIC10:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	nop
	add	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L20:
	.align	2
.L19:
	.word	.LC0-(.LPIC10+4)
	.size	putint, .-putint
	.align	1
	.global	putch
	.syntax unified
	.thumb
	.thumb_func
	.type	putch, %function
putch:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r0, [r7, #4]
	bl	putchar(PLT)
	nop
	add	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
	.size	putch, .-putch
	.section	.rodata
	.align	2
.LC2:
	.ascii	"%d:\000"
	.align	2
.LC3:
	.ascii	" %d\000"
	.text
	.align	1
	.global	putarray
	.syntax unified
	.thumb
	.thumb_func
	.type	putarray, %function
putarray:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #16
	add	r7, sp, #0
	str	r0, [r7, #4]
	str	r1, [r7]
	ldr	r1, [r7, #4]
	ldr	r3, .L25
.LPIC11:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	mov	r3, #0
	str	r3, [r7, #12]
	b	.L23
.L24:
	ldr	r3, [r7, #12]
	add	r3, r3, #1
	lsl	r3, r3, #2
	ldr	r2, [r7]
	add	r3, r3, r2
	ldr	r3, [r3]
	mov	r1, r3
	ldr	r3, .L25+4
.LPIC12:
	add	r3, pc
	mov	r0, r3
	bl	printf(PLT)
	ldr	r3, [r7, #12]
	add	r3, r3, #1
	str	r3, [r7, #12]
.L23:
	ldr	r2, [r7, #12]
	ldr	r3, [r7, #4]
	cmp	r2, r3
	blt	.L24
	mov	r0, #10
	bl	putchar(PLT)
	nop
	add	r7, r7, #16
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L26:
	.align	2
.L25:
	.word	.LC2-(.LPIC11+4)
	.word	.LC3-(.LPIC12+4)
	.size	putarray, .-putarray
	.align	1
	.global	before_main
	.syntax unified
	.thumb
	.thumb_func
	.type	before_main, %function
before_main:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	@ link register save eliminated.
	str	r7, [sp, #-4]!
	sub	sp, sp, #12
	add	r7, sp, #0
	mov	r3, #0
	str	r3, [r7, #4]
	b	.L28
.L29:
	ldr	r3, .L30
.LPIC13:
	add	r3, pc
	ldr	r2, [r7, #4]
	mov	r1, #0
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L30+4
.LPIC14:
	add	r3, pc
	ldr	r2, [r7, #4]
	ldr	r1, [r3, r2, lsl #2]
	ldr	r3, .L30+8
.LPIC15:
	add	r3, pc
	ldr	r2, [r7, #4]
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L30+12
.LPIC16:
	add	r3, pc
	ldr	r2, [r7, #4]
	ldr	r1, [r3, r2, lsl #2]
	ldr	r3, .L30+16
.LPIC17:
	add	r3, pc
	ldr	r2, [r7, #4]
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L30+20
.LPIC18:
	add	r3, pc
	ldr	r2, [r7, #4]
	ldr	r1, [r3, r2, lsl #2]
	ldr	r3, .L30+24
.LPIC19:
	add	r3, pc
	ldr	r2, [r7, #4]
	str	r1, [r3, r2, lsl #2]
	ldr	r3, [r7, #4]
	add	r3, r3, #1
	str	r3, [r7, #4]
.L28:
	ldr	r3, [r7, #4]
	cmp	r3, #1024
	blt	.L29
	ldr	r3, .L30+28
.LPIC20:
	add	r3, pc
	mov	r2, #1
	str	r2, [r3]
	nop
	add	r7, r7, #12
	mov	sp, r7
	@ sp needed
	ldr	r7, [sp], #4
	bx	lr
.L31:
	.align	2
.L30:
	.word	_sysy_us-(.LPIC13+4)
	.word	_sysy_us-(.LPIC14+4)
	.word	_sysy_s-(.LPIC15+4)
	.word	_sysy_s-(.LPIC16+4)
	.word	_sysy_m-(.LPIC17+4)
	.word	_sysy_m-(.LPIC18+4)
	.word	_sysy_h-(.LPIC19+4)
	.word	_sysy_idx-(.LPIC20+4)
	.size	before_main, .-before_main
	.section	.init_array,"aw"
	.align	2
	.word	before_main
	.section	.rodata
	.align	2
.LC4:
	.ascii	"Timer@%04d-%04d: %dH-%dM-%dS-%dus\012\000"
	.text
	.align	1
	.global	after_main
	.syntax unified
	.thumb
	.thumb_func
	.type	after_main, %function
after_main:
	@ args = 0, pretend = 0, frame = 16
	@ frame_needed = 1, uses_anonymous_args = 0
	str	r4, [sp, #-20]!
	strd	r5, r6, [sp, #4]
	strd	r7, lr, [sp, #12]
	sub	sp, sp, #36
	add	r7, sp, #16
	ldr	r4, .L35
.LPIC21:
	add	r4, pc
	mov	r3, #1
	str	r3, [r7, #12]
	b	.L33
.L34:
	ldr	r3, .L35+4
	ldr	r3, [r4, r3]
	ldr	r6, [r3]
	ldr	r3, .L35+8
.LPIC22:
	add	r3, pc
	ldr	r2, [r7, #12]
	ldr	r3, [r3, r2, lsl #2]
	str	r3, [r7, #4]
	ldr	r3, .L35+12
.LPIC23:
	add	r3, pc
	ldr	r2, [r7, #12]
	ldr	r1, [r3, r2, lsl #2]
	str	r1, [r7]
	ldr	r3, .L35+16
.LPIC24:
	add	r3, pc
	ldr	r2, [r7, #12]
	ldr	r3, [r3, r2, lsl #2]
	ldr	r2, .L35+20
.LPIC25:
	add	r2, pc
	ldr	r1, [r7, #12]
	ldr	r2, [r2, r1, lsl #2]
	ldr	r1, .L35+24
.LPIC26:
	add	r1, pc
	ldr	r0, [r7, #12]
	ldr	r1, [r1, r0, lsl #2]
	ldr	r0, .L35+28
.LPIC27:
	add	r0, pc
	ldr	r5, [r7, #12]
	ldr	r0, [r0, r5, lsl #2]
	str	r0, [sp, #12]
	str	r1, [sp, #8]
	str	r2, [sp, #4]
	str	r3, [sp]
	ldr	r3, [r7]
	ldr	r2, [r7, #4]
	ldr	r1, .L35+32
.LPIC28:
	add	r1, pc
	mov	r0, r6
	bl	fprintf(PLT)
	ldr	r3, .L35+36
.LPIC29:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L35+40
.LPIC30:
	add	r3, pc
	ldr	r1, [r7, #12]
	ldr	r3, [r3, r1, lsl #2]
	add	r2, r2, r3
	ldr	r3, .L35+44
.LPIC31:
	add	r3, pc
	str	r2, [r3]
	ldr	r3, .L35+48
.LPIC32:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L35+52
.LPIC33:
	add	r3, pc
	ldr	r1, [r7, #12]
	ldr	r3, [r3, r1, lsl #2]
	add	r2, r2, r3
	ldr	r3, .L35+56
.LPIC34:
	add	r3, pc
	str	r2, [r3]
	ldr	r3, .L35+60
.LPIC35:
	add	r3, pc
	ldr	r2, [r3]
	movw	r3, #56963
	movt	r3, 17179
	smull	r1, r3, r3, r2
	asr	r1, r3, #18
	asr	r3, r2, #31
	sub	r3, r1, r3
	movw	r1, #16960
	movt	r1, 15
	mul	r3, r1, r3
	sub	r3, r2, r3
	ldr	r2, .L35+64
.LPIC36:
	add	r2, pc
	str	r3, [r2]
	ldr	r3, .L35+68
.LPIC37:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L35+72
.LPIC38:
	add	r3, pc
	ldr	r1, [r7, #12]
	ldr	r3, [r3, r1, lsl #2]
	add	r2, r2, r3
	ldr	r3, .L35+76
.LPIC39:
	add	r3, pc
	str	r2, [r3]
	ldr	r3, .L35+80
.LPIC40:
	add	r3, pc
	ldr	r2, [r3]
	movw	r3, #34953
	movt	r3, 34952
	smull	r1, r3, r3, r2
	add	r3, r3, r2
	asr	r1, r3, #5
	asr	r3, r2, #31
	sub	r1, r1, r3
	mov	r3, r1
	lsl	r3, r3, #4
	sub	r3, r3, r1
	lsl	r3, r3, #2
	sub	r1, r2, r3
	ldr	r3, .L35+84
.LPIC41:
	add	r3, pc
	str	r1, [r3]
	ldr	r3, .L35+88
.LPIC42:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L35+92
.LPIC43:
	add	r3, pc
	ldr	r1, [r7, #12]
	ldr	r3, [r3, r1, lsl #2]
	add	r2, r2, r3
	ldr	r3, .L35+96
.LPIC44:
	add	r3, pc
	str	r2, [r3]
	ldr	r3, .L35+100
.LPIC45:
	add	r3, pc
	ldr	r2, [r3]
	movw	r3, #34953
	movt	r3, 34952
	smull	r1, r3, r3, r2
	add	r3, r3, r2
	asr	r1, r3, #5
	asr	r3, r2, #31
	sub	r1, r1, r3
	mov	r3, r1
	lsl	r3, r3, #4
	sub	r3, r3, r1
	lsl	r3, r3, #2
	sub	r1, r2, r3
	ldr	r3, .L35+104
.LPIC46:
	add	r3, pc
	str	r1, [r3]
	ldr	r3, [r7, #12]
	add	r3, r3, #1
	str	r3, [r7, #12]
.L33:
	ldr	r3, .L35+108
.LPIC47:
	add	r3, pc
	ldr	r3, [r3]
	ldr	r2, [r7, #12]
	cmp	r2, r3
	blt	.L34
	nop
	nop
	add	r7, r7, #20
	mov	sp, r7
	@ sp needed
	ldrd	r4, r5, [sp]
	ldrd	r6, r7, [sp, #8]
	add	sp, sp, #16
	ldr	pc, [sp], #4
.L36:
	.align	2
.L35:
	.word	_GLOBAL_OFFSET_TABLE_-(.LPIC21+4)
	.word	stderr(GOT)
	.word	_sysy_l1-(.LPIC22+4)
	.word	_sysy_l2-(.LPIC23+4)
	.word	_sysy_h-(.LPIC24+4)
	.word	_sysy_m-(.LPIC25+4)
	.word	_sysy_s-(.LPIC26+4)
	.word	_sysy_us-(.LPIC27+4)
	.word	.LC4-(.LPIC28+4)
	.word	_sysy_us-(.LPIC29+4)
	.word	_sysy_us-(.LPIC30+4)
	.word	_sysy_us-(.LPIC31+4)
	.word	_sysy_s-(.LPIC32+4)
	.word	_sysy_s-(.LPIC33+4)
	.word	_sysy_s-(.LPIC34+4)
	.word	_sysy_us-(.LPIC35+4)
	.word	_sysy_us-(.LPIC36+4)
	.word	_sysy_m-(.LPIC37+4)
	.word	_sysy_m-(.LPIC38+4)
	.word	_sysy_m-(.LPIC39+4)
	.word	_sysy_s-(.LPIC40+4)
	.word	_sysy_s-(.LPIC41+4)
	.word	_sysy_h-(.LPIC42+4)
	.word	_sysy_h-(.LPIC43+4)
	.word	_sysy_h-(.LPIC44+4)
	.word	_sysy_m-(.LPIC45+4)
	.word	_sysy_m-(.LPIC46+4)
	.word	_sysy_idx-(.LPIC47+4)
	.size	after_main, .-after_main
	.section	.fini_array,"aw"
	.align	2
	.word	after_main
	.text
	.align	1
	.global	_sysy_starttime
	.syntax unified
	.thumb
	.thumb_func
	.type	_sysy_starttime, %function
_sysy_starttime:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	ldr	r3, .L38
.LPIC48:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L38+4
.LPIC49:
	add	r3, pc
	ldr	r1, [r7, #4]
	str	r1, [r3, r2, lsl #2]
	mov	r1, #0
	ldr	r3, .L38+8
.LPIC50:
	add	r3, pc
	mov	r0, r3
	bl	gettimeofday(PLT)
	nop
	add	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L39:
	.align	2
.L38:
	.word	_sysy_idx-(.LPIC48+4)
	.word	_sysy_l1-(.LPIC49+4)
	.word	_sysy_start-(.LPIC50+4)
	.size	_sysy_starttime, .-_sysy_starttime
	.align	1
	.global	_sysy_stoptime
	.syntax unified
	.thumb
	.thumb_func
	.type	_sysy_stoptime, %function
_sysy_stoptime:
	@ args = 0, pretend = 0, frame = 8
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	sub	sp, sp, #8
	add	r7, sp, #0
	str	r0, [r7, #4]
	mov	r1, #0
	ldr	r3, .L41
.LPIC51:
	add	r3, pc
	mov	r0, r3
	bl	gettimeofday(PLT)
	ldr	r3, .L41+4
.LPIC52:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+8
.LPIC53:
	add	r3, pc
	ldr	r1, [r7, #4]
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+12
.LPIC54:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+16
.LPIC55:
	add	r3, pc
	ldr	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+20
.LPIC56:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+24
.LPIC57:
	add	r3, pc
	ldr	r3, [r3]
	sub	r2, r2, r3
	movw	r3, #16960
	movt	r3, 15
	mul	r2, r3, r2
	ldr	r3, .L41+28
.LPIC58:
	add	r3, pc
	ldr	r3, [r3, #4]
	add	r2, r2, r3
	ldr	r3, .L41+32
.LPIC59:
	add	r3, pc
	ldr	r3, [r3, #4]
	sub	r3, r2, r3
	ldr	r2, .L41+36
.LPIC60:
	add	r2, pc
	ldr	r2, [r2]
	add	r1, r1, r3
	ldr	r3, .L41+40
.LPIC61:
	add	r3, pc
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+44
.LPIC62:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+48
.LPIC63:
	add	r3, pc
	ldr	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+52
.LPIC64:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+56
.LPIC65:
	add	r3, pc
	ldr	r2, [r3, r2, lsl #2]
	movw	r3, #56963
	movt	r3, 17179
	smull	r0, r3, r3, r2
	asr	r0, r3, #18
	asr	r3, r2, #31
	sub	r3, r0, r3
	ldr	r2, .L41+60
.LPIC66:
	add	r2, pc
	ldr	r2, [r2]
	add	r1, r1, r3
	ldr	r3, .L41+64
.LPIC67:
	add	r3, pc
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+68
.LPIC68:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+72
.LPIC69:
	add	r3, pc
	ldr	r2, [r3, r2, lsl #2]
	ldr	r3, .L41+76
.LPIC70:
	add	r3, pc
	ldr	r0, [r3]
	movw	r3, #56963
	movt	r3, 17179
	smull	r1, r3, r3, r2
	asr	r1, r3, #18
	asr	r3, r2, #31
	sub	r3, r1, r3
	movw	r1, #16960
	movt	r1, 15
	mul	r3, r1, r3
	sub	r3, r2, r3
	ldr	r2, .L41+80
.LPIC71:
	add	r2, pc
	str	r3, [r2, r0, lsl #2]
	ldr	r3, .L41+84
.LPIC72:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+88
.LPIC73:
	add	r3, pc
	ldr	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+92
.LPIC74:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+96
.LPIC75:
	add	r3, pc
	ldr	r3, [r3, r2, lsl #2]
	movw	r2, #34953
	movt	r2, 34952
	smull	r0, r2, r2, r3
	add	r2, r2, r3
	asr	r2, r2, #5
	asr	r3, r3, #31
	sub	r3, r2, r3
	ldr	r2, .L41+100
.LPIC76:
	add	r2, pc
	ldr	r2, [r2]
	add	r1, r1, r3
	ldr	r3, .L41+104
.LPIC77:
	add	r3, pc
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+108
.LPIC78:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+112
.LPIC79:
	add	r3, pc
	ldr	r2, [r3, r2, lsl #2]
	ldr	r3, .L41+116
.LPIC80:
	add	r3, pc
	ldr	r0, [r3]
	movw	r3, #34953
	movt	r3, 34952
	smull	r1, r3, r3, r2
	add	r3, r3, r2
	asr	r1, r3, #5
	asr	r3, r2, #31
	sub	r1, r1, r3
	mov	r3, r1
	lsl	r3, r3, #4
	sub	r3, r3, r1
	lsl	r3, r3, #2
	sub	r1, r2, r3
	ldr	r3, .L41+120
.LPIC81:
	add	r3, pc
	str	r1, [r3, r0, lsl #2]
	ldr	r3, .L41+124
.LPIC82:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+128
.LPIC83:
	add	r3, pc
	ldr	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+132
.LPIC84:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+136
.LPIC85:
	add	r3, pc
	ldr	r3, [r3, r2, lsl #2]
	movw	r2, #34953
	movt	r2, 34952
	smull	r0, r2, r2, r3
	add	r2, r2, r3
	asr	r2, r2, #5
	asr	r3, r3, #31
	sub	r3, r2, r3
	ldr	r2, .L41+140
.LPIC86:
	add	r2, pc
	ldr	r2, [r2]
	add	r1, r1, r3
	ldr	r3, .L41+144
.LPIC87:
	add	r3, pc
	str	r1, [r3, r2, lsl #2]
	ldr	r3, .L41+148
.LPIC88:
	add	r3, pc
	ldr	r2, [r3]
	ldr	r3, .L41+152
.LPIC89:
	add	r3, pc
	ldr	r2, [r3, r2, lsl #2]
	ldr	r3, .L41+156
.LPIC90:
	add	r3, pc
	ldr	r0, [r3]
	movw	r3, #34953
	movt	r3, 34952
	smull	r1, r3, r3, r2
	add	r3, r3, r2
	asr	r1, r3, #5
	asr	r3, r2, #31
	sub	r1, r1, r3
	mov	r3, r1
	lsl	r3, r3, #4
	sub	r3, r3, r1
	lsl	r3, r3, #2
	sub	r1, r2, r3
	ldr	r3, .L41+160
.LPIC91:
	add	r3, pc
	str	r1, [r3, r0, lsl #2]
	ldr	r3, .L41+164
.LPIC92:
	add	r3, pc
	ldr	r3, [r3]
	add	r2, r3, #1
	ldr	r3, .L41+168
.LPIC93:
	add	r3, pc
	str	r2, [r3]
	nop
	add	r7, r7, #8
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
.L42:
	.align	2
.L41:
	.word	_sysy_end-(.LPIC51+4)
	.word	_sysy_idx-(.LPIC52+4)
	.word	_sysy_l2-(.LPIC53+4)
	.word	_sysy_idx-(.LPIC54+4)
	.word	_sysy_us-(.LPIC55+4)
	.word	_sysy_end-(.LPIC56+4)
	.word	_sysy_start-(.LPIC57+4)
	.word	_sysy_end-(.LPIC58+4)
	.word	_sysy_start-(.LPIC59+4)
	.word	_sysy_idx-(.LPIC60+4)
	.word	_sysy_us-(.LPIC61+4)
	.word	_sysy_idx-(.LPIC62+4)
	.word	_sysy_s-(.LPIC63+4)
	.word	_sysy_idx-(.LPIC64+4)
	.word	_sysy_us-(.LPIC65+4)
	.word	_sysy_idx-(.LPIC66+4)
	.word	_sysy_s-(.LPIC67+4)
	.word	_sysy_idx-(.LPIC68+4)
	.word	_sysy_us-(.LPIC69+4)
	.word	_sysy_idx-(.LPIC70+4)
	.word	_sysy_us-(.LPIC71+4)
	.word	_sysy_idx-(.LPIC72+4)
	.word	_sysy_m-(.LPIC73+4)
	.word	_sysy_idx-(.LPIC74+4)
	.word	_sysy_s-(.LPIC75+4)
	.word	_sysy_idx-(.LPIC76+4)
	.word	_sysy_m-(.LPIC77+4)
	.word	_sysy_idx-(.LPIC78+4)
	.word	_sysy_s-(.LPIC79+4)
	.word	_sysy_idx-(.LPIC80+4)
	.word	_sysy_s-(.LPIC81+4)
	.word	_sysy_idx-(.LPIC82+4)
	.word	_sysy_h-(.LPIC83+4)
	.word	_sysy_idx-(.LPIC84+4)
	.word	_sysy_m-(.LPIC85+4)
	.word	_sysy_idx-(.LPIC86+4)
	.word	_sysy_h-(.LPIC87+4)
	.word	_sysy_idx-(.LPIC88+4)
	.word	_sysy_m-(.LPIC89+4)
	.word	_sysy_idx-(.LPIC90+4)
	.word	_sysy_m-(.LPIC91+4)
	.word	_sysy_idx-(.LPIC92+4)
	.word	_sysy_idx-(.LPIC93+4)
	.size	_sysy_stoptime, .-_sysy_stoptime
	.align	1
	.global	starttime
	.syntax unified
	.thumb
	.thumb_func
	.type	starttime, %function
starttime:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	add	r7, sp, #0
	mov	r0, #78
	bl	_sysy_starttime(PLT)
	nop
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
	.size	starttime, .-starttime
	.align	1
	.global	stoptime
	.syntax unified
	.thumb
	.thumb_func
	.type	stoptime, %function
stoptime:
	@ args = 0, pretend = 0, frame = 0
	@ frame_needed = 1, uses_anonymous_args = 0
	strd	r7, lr, [sp, #-8]!
	add	r7, sp, #0
	mov	r0, #79
	bl	_sysy_stoptime(PLT)
	nop
	mov	sp, r7
	@ sp needed
	pop	{r7, pc}
	.size	stoptime, .-stoptime
	.ident	"GCC: (Ubuntu 13.2.0-4ubuntu3) 13.2.0"
	.section	.note.GNU-stack,"",%progbits
