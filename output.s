	.file	"sviroxk"
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0                          // -- Begin function main
.LCPI0_0:
	.xword	0x400d99999999999a              // double 3.7000000000000002
.LCPI0_1:
	.xword	0x400999999999999a              // double 3.2000000000000002
	.text
	.globl	main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	sub	sp, sp, #48
	str	x30, [sp, #32]                  // 8-byte Folded Spill
	.cfi_def_cfa_offset 48
	.cfi_offset w30, -16
	adrp	x8, .LCPI0_0
	ldr	d0, [x8, :lo12:.LCPI0_0]
	bl	floor
	adrp	x8, .LCPI0_1
	str	d0, [sp, #40]
	ldr	d0, [x8, :lo12:.LCPI0_1]
	bl	ceil
	str	d0, [sp, #24]
	fmov	d0, #3.50000000
	bl	round
	str	d0, [sp, #16]
	bl	rand
	ldr	d0, [sp, #40]
	str	w0, [sp, #12]
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	bl	printf
	ldr	d0, [sp, #24]
	adrp	x0, .L__unnamed_2
	add	x0, x0, :lo12:.L__unnamed_2
	bl	printf
	ldr	d0, [sp, #16]
	adrp	x0, .L__unnamed_3
	add	x0, x0, :lo12:.L__unnamed_3
	bl	printf
	ldr	w1, [sp, #12]
	adrp	x0, .L__unnamed_4
	add	x0, x0, :lo12:.L__unnamed_4
	bl	printf
	ldr	x30, [sp, #32]                  // 8-byte Folded Reload
	mov	w0, wzr
	add	sp, sp, #48
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
	.asciz	"%f\n"
	.size	.L__unnamed_2, 4

	.type	.L__unnamed_3,@object           // @2
.L__unnamed_3:
	.asciz	"%f\n"
	.size	.L__unnamed_3, 4

	.type	.L__unnamed_4,@object           // @3
.L__unnamed_4:
	.asciz	"%d\n"
	.size	.L__unnamed_4, 4

	.section	".note.GNU-stack","",@progbits
