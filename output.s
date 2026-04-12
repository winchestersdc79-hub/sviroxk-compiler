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
	mov	w8, #5                          // =0x5
	add	x9, sp, #28
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	mov	w1, #5                          // =0x5
	str	w8, [sp, #28]
	stp	x9, x30, [sp, #8]               // 8-byte Folded Spill
	bl	printf
	ldr	x8, [sp, #8]
	mov	w9, #10                         // =0xa
	adrp	x0, .L__unnamed_2
	add	x0, x0, :lo12:.L__unnamed_2
	mov	w1, #10                         // =0xa
	str	w9, [x8]
	bl	printf
	ldr	w1, [sp, #28]
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

	.section	".note.GNU-stack","",@progbits
