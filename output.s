	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	str	x30, [sp, #-32]!                // 8-byte Folded Spill
	stp	x20, x19, [sp, #16]             // 16-byte Folded Spill
	.cfi_def_cfa_offset 32
	.cfi_offset w19, -8
	.cfi_offset w20, -16
	.cfi_offset w30, -32
	mov	w20, wzr
	adrp	x19, .L__unnamed_1
	add	x19, x19, :lo12:.L__unnamed_1
.LBB0_1:                                // %loop
                                        // =>This Inner Loop Header: Depth=1
	cmp	w20, #3
	str	w20, [sp, #12]
	b.ge	.LBB0_3
// %bb.2:                               // %body
                                        //   in Loop: Header=BB0_1 Depth=1
	mov	x0, x19
	bl	puts
	add	w20, w20, #1
	b	.LBB0_1
.LBB0_3:                                // %after
	ldp	x20, x19, [sp, #16]             // 16-byte Folded Reload
	mov	w0, wzr
	ldr	x30, [sp], #32                  // 8-byte Folded Reload
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.type	.L__unnamed_1,@object           // @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"\320\277\321\200\320\270\320\262\320\265\321\202 \320\276\321\202 SVIROXK"
	.size	.L__unnamed_1, 26

	.section	".note.GNU-stack","",@progbits
