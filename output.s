	.file	"sviroxk"
	.section	.rodata.cst8,"aM",@progbits,8
	.p2align	3, 0x0                          // -- Begin function main
.LCPI0_0:
	.xword	0x40091eb851eb851f              // double 3.1400000000000001
	.text
	.globl	main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	sub	sp, sp, #32
	.cfi_def_cfa_offset 32
	.cfi_offset w30, -16
	mov	x8, #34079                      // =0x851f
	mov	x9, #4611686018427387904        // =0x4000000000000000
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	movk	x8, #20971, lsl #16
	movk	x8, #7864, lsl #32
	movk	x8, #16393, lsl #48
	stp	x30, x8, [sp, #16]              // 8-byte Folded Spill
	mov	x8, #34079                      // =0x851f
	movk	x8, #20971, lsl #16
	movk	x8, #7864, lsl #32
	movk	x8, #16409, lsl #48
	stp	x8, x9, [sp]
	adrp	x8, .LCPI0_0
	ldr	d0, [x8, :lo12:.LCPI0_0]
	bl	printf
	ldr	d0, [sp, #8]
	adrp	x0, .L__unnamed_2
	add	x0, x0, :lo12:.L__unnamed_2
	bl	printf
	ldr	d0, [sp]
	adrp	x0, .L__unnamed_3
	add	x0, x0, :lo12:.L__unnamed_3
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
	.asciz	"%f\n"
	.size	.L__unnamed_2, 4

	.type	.L__unnamed_3,@object           // @2
.L__unnamed_3:
	.asciz	"%f\n"
	.size	.L__unnamed_3, 4

	.section	".note.GNU-stack","",@progbits
