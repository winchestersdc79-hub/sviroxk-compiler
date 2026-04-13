	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	str	x30, [sp, #-16]!                // 8-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w30, -16
	mov	w8, #1                          // =0x1
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	stp	wzr, w8, [sp, #8]
	bl	puts
	ldr	w8, [sp, #8]
	adrp	x9, .L__unnamed_2
	add	x9, x9, :lo12:.L__unnamed_2
	cmp	w8, #0
	adrp	x8, .L__unnamed_3
	add	x8, x8, :lo12:.L__unnamed_3
	csel	x0, x8, x9, ne
	bl	puts
	mov	w0, wzr
	ldr	x30, [sp], #16                  // 8-byte Folded Reload
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.type	.L__unnamed_1,@object           // @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"x \321\215\321\202\320\276 true"
	.size	.L__unnamed_1, 14

	.type	.L__unnamed_4,@object           // @1
.L__unnamed_4:
	.asciz	"x \321\215\321\202\320\276 false"
	.size	.L__unnamed_4, 15

	.type	.L__unnamed_3,@object           // @2
.L__unnamed_3:
	.asciz	"y \321\215\321\202\320\276 true"
	.size	.L__unnamed_3, 14

	.type	.L__unnamed_2,@object           // @3
.L__unnamed_2:
	.asciz	"y \321\215\321\202\320\276 false"
	.size	.L__unnamed_2, 15

	.section	".note.GNU-stack","",@progbits
