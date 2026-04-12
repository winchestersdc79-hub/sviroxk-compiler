	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	sub	sp, sp, #32
	stp	x30, x19, [sp, #16]             // 16-byte Folded Spill
	.cfi_def_cfa_offset 32
	.cfi_offset w19, -8
	.cfi_offset w30, -16
	mov	w8, wzr
	adrp	x19, .L__unnamed_1
	add	x19, x19, :lo12:.L__unnamed_1
.LBB0_1:                                // %cond
                                        // =>This Inner Loop Header: Depth=1
	cmp	w8, #2
	str	w8, [sp, #12]
	b.gt	.LBB0_3
// %bb.2:                               // %body
                                        //   in Loop: Header=BB0_1 Depth=1
	ldr	w1, [sp, #12]
	mov	x0, x19
	bl	printf
	ldr	w8, [sp, #12]
	add	w8, w8, #1
	b	.LBB0_1
.LBB0_3:                                // %after
	ldp	x30, x19, [sp, #16]             // 16-byte Folded Reload
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
