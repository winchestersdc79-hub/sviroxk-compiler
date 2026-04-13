	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	.cfi_offset w30, -16
	fmov	d0, #5.00000000
	mov	w8, #5                          // =0x5
	mov	x9, #4617315517961601024        // =0x4014000000000000
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	str	w8, [sp, #28]
	stp	x9, x30, [sp, #8]               // 8-byte Folded Spill
	bl	printf
	mov	x8, #34079                      // =0x851f
	mov	w9, #3                          // =0x3
	adrp	x0, .L__unnamed_2
	add	x0, x0, :lo12:.L__unnamed_2
	movk	x8, #20971, lsl #16
	mov	w1, #3                          // =0x3
	movk	x8, #7864, lsl #32
	str	w9, [sp, #24]
	movk	x8, #16393, lsl #48
	str	x8, [sp]
	bl	printf
	ldr	x30, [sp, #16]                  // 8-byte Folded Reload
	mov	w0, wzr
	add	sp, sp, #32
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.type	.L__unnamed_1,@object           // @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"%f\n"
	.size	.L__unnamed_1, 4

	.type	.L__unnamed_2,@object           // @1
.L__unnamed_2:
	.asciz	"%d\n"
	.size	.L__unnamed_2, 4

	.section	".note.GNU-stack","",@progbits
