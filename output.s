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
	adrp	x1, .L__unnamed_2
	add	x1, x1, :lo12:.L__unnamed_2
	bl	fopen
	mov	x1, x0
	str	x0, [sp, #8]
	adrp	x0, .L__unnamed_3
	add	x0, x0, :lo12:.L__unnamed_3
	bl	fputs
	ldr	x0, [sp, #8]
	bl	fclose
	adrp	x0, .L__unnamed_4
	add	x0, x0, :lo12:.L__unnamed_4
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
	.asciz	"test.txt"
	.size	.L__unnamed_1, 9

	.type	.L__unnamed_2,@object           // @1
.L__unnamed_2:
	.asciz	"w"
	.size	.L__unnamed_2, 2

	.type	.L__unnamed_3,@object           // @2
.L__unnamed_3:
	.asciz	"\320\277\321\200\320\270\320\262\320\265\321\202 \320\270\320\267 SVIROXK"
	.size	.L__unnamed_3, 26

	.type	.L__unnamed_4,@object           // @3
.L__unnamed_4:
	.asciz	"\321\204\320\260\320\271\320\273 \320\267\320\260\320\277\320\270\321\201\320\260\320\275"
	.size	.L__unnamed_4, 24

	.section	".note.GNU-stack","",@progbits
