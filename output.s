	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	stp	x29, x30, [sp, #-48]!           // 16-byte Folded Spill
	stp	x22, x21, [sp, #16]             // 16-byte Folded Spill
	mov	x29, sp
	stp	x20, x19, [sp, #32]             // 16-byte Folded Spill
	sub	sp, sp, #16
	.cfi_def_cfa w29, 48
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w21, -24
	.cfi_offset w22, -32
	.cfi_offset w30, -40
	.cfi_offset w29, -48
	fmov	d0, #16.00000000
	fsqrt	d0, d0
	fcmp	d0, d0
	b.vs	.LBB0_2
.LBB0_1:                                // %entry.split
	fcvtzs	w8, d0
	mov	x19, sp
	stur	w8, [x29, #-4]
	sub	x8, x19, #16
	mov	sp, x8
	mov	w8, #5                          // =0x5
	mov	x20, sp
	stur	w8, [x19, #-16]
	sub	x8, x20, #16
	mov	sp, x8
	fmov	d0, #2.00000000
	fmov	d1, #8.00000000
	bl	pow
	fcvtzs	w8, d0
	mov	x21, sp
	stur	w8, [x20, #-16]
	sub	x8, x21, #16
	mov	sp, x8
	mov	w8, #7                          // =0x7
	mov	x22, sp
	stur	w8, [x21, #-16]
	sub	x8, x22, #16
	mov	sp, x8
	ldur	w1, [x29, #-4]
	mov	w8, #3                          // =0x3
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	stur	w8, [x22, #-16]
	bl	printf
	ldur	w1, [x19, #-16]
	adrp	x0, .L__unnamed_2
	add	x0, x0, :lo12:.L__unnamed_2
	bl	printf
	ldur	w1, [x20, #-16]
	adrp	x0, .L__unnamed_3
	add	x0, x0, :lo12:.L__unnamed_3
	bl	printf
	ldur	w1, [x21, #-16]
	adrp	x0, .L__unnamed_4
	add	x0, x0, :lo12:.L__unnamed_4
	bl	printf
	ldur	w1, [x22, #-16]
	adrp	x0, .L__unnamed_5
	add	x0, x0, :lo12:.L__unnamed_5
	bl	printf
	mov	w0, wzr
	mov	sp, x29
	ldp	x20, x19, [sp, #32]             // 16-byte Folded Reload
	ldp	x22, x21, [sp, #16]             // 16-byte Folded Reload
	ldp	x29, x30, [sp], #48             // 16-byte Folded Reload
	ret
.LBB0_2:                                // %call.sqrt
	fmov	d0, #16.00000000
	bl	sqrt
	b	.LBB0_1
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.type	.L__unnamed_1,@object           // @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%d\n"
	.size	.L__unnamed_1, 4

	.type	.L__unnamed_2,@object           // @1
.L__unnamed_2:
	.asciz	"%d\n"
	.size	.L__unnamed_2, 4

	.type	.L__unnamed_3,@object           // @2
.L__unnamed_3:
	.asciz	"%d\n"
	.size	.L__unnamed_3, 4

	.type	.L__unnamed_4,@object           // @3
.L__unnamed_4:
	.asciz	"%d\n"
	.size	.L__unnamed_4, 4

	.type	.L__unnamed_5,@object           // @4
.L__unnamed_5:
	.asciz	"%d\n"
	.size	.L__unnamed_5, 4

	.section	".note.GNU-stack","",@progbits
