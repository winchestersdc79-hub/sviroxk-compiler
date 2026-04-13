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
	mov	w8, #65                         // =0x41
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	mov	w1, #65                         // =0x41
	stur	w8, [sp, #15]
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
	.asciz	"%c\n"
	.size	.L__unnamed_1, 4

	.section	".note.GNU-stack","",@progbits
