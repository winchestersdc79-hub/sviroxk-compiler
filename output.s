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
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	bl	puts
	adrp	x0, .L__unnamed_2
	add	x0, x0, :lo12:.L__unnamed_2
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
	.asciz	"\320\277\321\200\320\270\320\262\320\265\321\202 \320\274\320\270\321\200"
	.size	.L__unnamed_1, 20

	.type	.L__unnamed_2,@object           // @1
.L__unnamed_2:
	.asciz	"\321\217\320\267\321\213\320\272 SVIROXK \321\200\320\260\320\261\320\276\321\202\320\260\320\265\321\202"
	.size	.L__unnamed_2, 34

	.section	".note.GNU-stack","",@progbits
