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
	bl	privet
	mov	w0, wzr
	ldr	x30, [sp], #16                  // 8-byte Folded Reload
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.globl	privet                          // -- Begin function privet
	.p2align	2
	.type	privet,@function
privet:                                 // @privet
	.cfi_startproc
// %bb.0:                               // %entry
	str	x30, [sp, #-16]!                // 8-byte Folded Spill
	.cfi_def_cfa_offset 16
	.cfi_offset w30, -16
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	bl	puts
	mov	w0, wzr
	ldr	x30, [sp], #16                  // 8-byte Folded Reload
	ret
.Lfunc_end1:
	.size	privet, .Lfunc_end1-privet
	.cfi_endproc
                                        // -- End function
	.type	.L__unnamed_1,@object           // @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"\320\277\321\200\320\270\320\262\320\265\321\202 \320\270\320\267 \321\204\321\203\320\275\320\272\321\206\320\270\320\270"
	.size	.L__unnamed_1, 33

	.section	".note.GNU-stack","",@progbits
