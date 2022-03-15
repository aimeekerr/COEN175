readarray:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$readarray.size, %esp
	movl	$0, %eax
	movl	%eax, 0+-4(%ebp)
.L0:
	movl	-4(%ebp), %eax
	cmpl	n, %eax
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L1
	movl	-4(%ebp), %eax
	imull	$4, %eax
	movl	a, %ecx
	addl	%eax, %ecx
	pushl	%ecx
	leal	.L2, %eax
	pushl	%eax
	call	scanf
	addl	$8, %esp
	movl	-4(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 0+-4(%ebp)
	jmp	.L0
.L1:

readarray.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	readarray.size, 4
	.globl	readarray

writearray:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$writearray.size, %esp
	movl	$0, %eax
	movl	%eax, 0+-4(%ebp)
.L3:
	movl	-4(%ebp), %eax
	cmpl	n, %eax
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L4
	movl	-4(%ebp), %eax
	imull	$4, %eax
	movl	a, %ecx
	addl	%eax, %ecx
	movl	(%ecx), %ecx
	pushl	%ecx
	leal	.L5, %eax
	pushl	%eax
	call	printf
	addl	$8, %esp
	movl	-4(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 0+-4(%ebp)
	jmp	.L3
.L4:
	leal	.L6, %eax
	pushl	%eax
	call	printf
	addl	$4, %esp

writearray.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	writearray.size, 4
	.globl	writearray

exchange:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$exchange.size, %esp
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movl	%eax, 0+-4(%ebp)
	movl	12(%ebp), %eax
	movl	(%eax), %eax
	movl	8(%ebp), %ecx
	movl	%eax,(%ecx)
	movl	12(%ebp), %eax
	movl	-4(%ebp),(%eax)

exchange.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	exchange.size, 4
	.globl	exchange

partition:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$partition.size, %esp
	movl	12(%ebp), %eax
	imull	$4, %eax
	movl	8(%ebp), %ecx
	addl	%eax, %ecx
	movl	(%ecx), %ecx
	movl	%ecx, 0+-12(%ebp)
	movl	12(%ebp), %eax
	subl	$1, %eax
	movl	%eax, 0+-4(%ebp)
	movl	16(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 0+-8(%ebp)
.L7:
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L8
	movl	-8(%ebp), %eax
	subl	$1, %eax
	movl	%eax, 0+-8(%ebp)
.L9:
	movl	-8(%ebp), %eax
	imull	$4, %eax
	movl	8(%ebp), %ecx
	addl	%eax, %ecx
	movl	(%ecx), %ecx
	cmpl	-12(%ebp), %ecx
	setg	%cl
	movzbl	%cl, %ecx
	cmpl	$0, %ecx
	je	.L10
	movl	-8(%ebp), %eax
	subl	$1, %eax
	movl	%eax, 0+-8(%ebp)
	jmp	.L9
.L10:
	movl	-4(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 0+-4(%ebp)
.L11:
	movl	-4(%ebp), %eax
	imull	$4, %eax
	movl	8(%ebp), %ecx
	addl	%eax, %ecx
	movl	(%ecx), %ecx
	cmpl	-12(%ebp), %ecx
	setl	%cl
	movzbl	%cl, %ecx
	cmpl	$0, %ecx
	je	.L12
	movl	-4(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 0+-4(%ebp)
	jmp	.L11
.L12:
	movl	-4(%ebp), %eax
	cmpl	-8(%ebp), %eax
	setl	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L13
	movl	-8(%ebp), %eax
	imull	$4, %eax
	movl	8(%ebp), %ecx
	addl	%eax, %ecx
	pushl	%ecx
	movl	-4(%ebp), %eax
	imull	$4, %eax
	movl	8(%ebp), %ecx
	addl	%eax, %ecx
	pushl	%ecx
	call	exchange
	addl	$8, %esp
.L13:
	jmp	.L7
.L8:
	movl	-8(%ebp), %eax
	jmp	partition.exit

partition.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	partition.size, 12
	.globl	partition

quicksort:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$quicksort.size, %esp
	movl	16(%ebp), %eax
	cmpl	12(%ebp), %eax
	setg	%al
	movzbl	%al, %eax
	cmpl	$0, %eax
	je	.L15
	pushl	16(%ebp)
	pushl	12(%ebp)
	pushl	8(%ebp)
	call	partition
	addl	$12, %esp
	movl	%eax, 0+-4(%ebp)
	pushl	-4(%ebp)
	pushl	12(%ebp)
	pushl	8(%ebp)
	call	quicksort
	addl	$12, %esp
	pushl	16(%ebp)
	movl	-4(%ebp), %eax
	addl	$1, %eax
	pushl	%eax
	pushl	8(%ebp)
	call	quicksort
	addl	$12, %esp
.L15:

quicksort.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	quicksort.size, 4
	.globl	quicksort

main:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$main.size, %esp
	movl	$8, %eax
	movl	%eax, 0+n
	movl	n, %eax
	imull	$4, %eax
	pushl	%eax
	call	malloc
	addl	$4, %esp
	movl	%eax, 0+a
	call	readarray
	movl	n, %eax
	subl	$1, %eax
	pushl	%eax
	pushl	$0
	pushl	a
	call	quicksort
	addl	$12, %esp
	call	writearray

main.exit:
	movl	%ebp, %esp
	popl	%ebp
	ret

	.set	main.size, 0
	.globl	main

	.comm	n, 4
	.comm	a, 4
	.data	
.L6:	.asciz	"\012"
.L2:	.asciz	"%d"
.L5:	.asciz	"%d "
