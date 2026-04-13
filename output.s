	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	sub	sp, sp, #32
	str	x30, [sp, #16]                  // 8-byte Folded Spill
	.cfi_def_cfa_offset 32
	.cfi_offset w30, -16
	mov	w10, #2                         // =0x2
	mov	w9, #3                          // =0x3
	mov	w8, #10                         // =0xa
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	mov	w1, #10                         // =0xa
	str	w9, [sp, #12]
	stp	w10, w8, [sp, #24]
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

	.section	".note.GNU-stack","",@progbits
