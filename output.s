	.text	
	.align		2
	.globl		main
main:
	li		$t0, 9
	sw		$t0, i

	jal		fib
	lw		$t0, i
	li		$v0, 1
	move		$a0, $t0
	syscall	
finishpart:
	li		$v0, 10
	syscall	

fib:
	addi		$sp, $sp, -4
	sw		$ra, 0($sp)
	lw		$t0, i
	li		$t1, 1
	sub		$t2, $t0, $t1
	sw		$t2, i
	lw		$t0, i
	li		$t1, 0
	blt		$t0, $t1, L1

	jal		fib
L1:
fibExit:
	lw		$ra, 0($sp)
	addi		$sp, $sp, 4
	addi		$sp, $sp, 0
	jr		$ra

	.data	
	.align		4
_nl:	.asciiz		"\n"
_space:	.asciiz		" "
True:	.asciiz		"true"
False:	.asciiz		"false"
IS:	.asciiz		"Enter an integer\n"
FS:	.asciiz		"Enter an float\n"
BS:	.asciiz		"Enter an boolean, 0 for false, 1 for true\n"
i:	.word		0
