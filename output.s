	.file	"sviroxk"
	.text
	.globl	main                            // -- Begin function main
	.p2align	2
	.type	main,@function
main:                                   // @main
	.cfi_startproc
// %bb.0:                               // %entry
	sub	sp, sp, #128
	stp	x30, x19, [sp, #112]            // 16-byte Folded Spill
	.cfi_def_cfa_offset 128
	.cfi_offset w19, -8
	.cfi_offset w30, -16
	adrp	x0, .L__unnamed_1
	add	x0, x0, :lo12:.L__unnamed_1
	adrp	x1, .L__unnamed_2
	add	x1, x1, :lo12:.L__unnamed_2
	adrp	x8, .L__unnamed_3
	add	x8, x8, :lo12:.L__unnamed_3
	stp	x1, x0, [sp, #96]
	str	x8, [sp, #88]
	bl	strcmp
	ldr	x8, [sp, #104]
	cmp	w0, #0
	ldr	x1, [sp, #88]
	cset	w9, eq
	mov	x0, x8
	strb	w9, [sp, #84]
	bl	strcmp
	ldr	w1, [sp, #84]
	cmp	w0, #0
	adrp	x0, .L__unnamed_4
	add	x0, x0, :lo12:.L__unnamed_4
	cset	w8, eq
	strb	w8, [sp, #80]
	bl	printf
	ldr	w1, [sp, #80]
	adrp	x0, .L__unnamed_5
	add	x0, x0, :lo12:.L__unnamed_5
	bl	printf
	mov	w8, #42                         // =0x2a
	adrp	x1, .L__unnamed_6
	add	x1, x1, :lo12:.L__unnamed_6
	mov	x0, sp
	mov	w2, #42                         // =0x2a
	str	w8, [sp, #76]
	mov	x19, sp
	bl	sprintf
	mov	x0, sp
	str	x19, [sp, #64]
	bl	puts
	ldp	x30, x19, [sp, #112]            // 16-byte Folded Reload
	mov	w0, wzr
	add	sp, sp, #128
	ret
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
	.cfi_endproc
                                        // -- End function
	.type	.L__unnamed_1,@object           // @0
	.section	.rodata.str1.1,"aMS",@progbits,1
.L__unnamed_1:
	.asciz	"\320\277\321\200\320\270\320\262\320\265\321\202"
	.size	.L__unnamed_1, 13

	.type	.L__unnamed_2,@object           // @1
.L__unnamed_2:
	.asciz	"\320\277\321\200\320\270\320\262\320\265\321\202"
	.size	.L__unnamed_2, 13

	.type	.L__unnamed_3,@object           // @2
.L__unnamed_3:
	.asciz	"\320\274\320\270\321\200"
	.size	.L__unnamed_3, 7

	.type	.L__unnamed_4,@object           // @3
.L__unnamed_4:
	.asciz	"%d\n"
	.size	.L__unnamed_4, 4

	.type	.L__unnamed_5,@object           // @4
.L__unnamed_5:
	.asciz	"%d\n"
	.size	.L__unnamed_5, 4

	.type	.L__unnamed_6,@object           // @5
.L__unnamed_6:
	.asciz	"%d"
	.size	.L__unnamed_6, 3

	.section	".note.GNU-stack","",@progbits
