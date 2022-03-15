towers:
#prologue:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$0, %esp
#block
#simple
#Call:
	push	16(%ebp)
	push	20(%ebp)
	push	12(%ebp)
	push	8(%ebp)
	call	call_towers
#simple
#Call:
	push	16(%ebp)
	push	12(%ebp)
	call	print_move
#simple
#Call:
	push	12(%ebp)
	push	16(%ebp)
	push	20(%ebp)
	push	8(%ebp)
	call	call_towers
#epilogue:
	movl	%ebp, %esp
	popl	%ebp
	ret
	.globl	towers
main:
#prologue:
	pushl	%ebp
	movl	%esp, %ebp
	subl	$4, %esp
#block
#assignment
	movl		$3, %eax
	movl	%eax, -4(%ebp)
#simple
#Call:
	push	-4(%ebp)
	call	print
#simple
#Call:
	push		$3
	push		$2
	push		$1
	push	-4(%ebp)
	call	towers
#epilogue:
	movl	%ebp, %esp
	popl	%ebp
	ret
	.globl	main
#Global generator
