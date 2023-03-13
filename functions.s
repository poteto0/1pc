	.arch armv8-a
	.file	"functions.c"
	.text
	.align	2
	.global	ret3
	.type	ret3, %function
ret3:
	mov	w0, 3
	ret
	.size	ret3, .-ret3
	.align	2
	.global	ret5
	.type	ret5, %function
ret5:
	mov	w0, 5
	ret
	.size	ret5, .-ret5
	.ident	"GCC: (Ubuntu/Linaro 7.5.0-3ubuntu1~18.04) 7.5.0"
	.section	.note.GNU-stack,"",@progbits
