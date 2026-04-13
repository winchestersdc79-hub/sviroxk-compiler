	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	.cfi_def_cfa_offset 16
	.cfi_offset w30, -16
	mov	x1, #58367                      // =0xe3ff
	movk	x1, #21515, lsl #16
	movk	x1, #2, lsl #32
	stp	x30, x1, [sp, #-16]!            // 8-byte Folded Spill
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	bl	printf
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
	.asciz	"%lld\n"
	.size	.L__unnamed_1, 6

	.section	".note.GNU-stack","",@progbits
