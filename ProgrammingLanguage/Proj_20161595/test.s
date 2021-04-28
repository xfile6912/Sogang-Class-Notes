	.file	"test.c"
	.globl	globali
	.data
	.align 4
	.type	globali, @object
	.size	globali, 4
globali:
	.long	1
	.globl	globalf
	.align 4
	.type	globalf, @object
	.size	globalf, 4
globalf:
	.long	1077936128
	.globl	globalarr
	.align 8
	.type	globalarr, @object
	.size	globalarr, 12
globalarr:
	.long	1
	.long	2
	.long	3
	.comm	globalp,8,8
	.comm	globalfp,8,8
	.text
	.globl	fun1
	.type	fun1, @function
fun1:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	fun1, .-fun1
	.globl	fun2
	.type	fun2, @function
fun2:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, %rax
	movq	%xmm0, %rcx
	movq	%rcx, %rdx
	movq	%rax, -16(%rbp)
	movq	%rdx, -8(%rbp)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	fun2, .-fun2
	.globl	fun3
	.type	fun3, @function
fun3:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	%esi, -8(%rbp)
	movl	%edx, -12(%rbp)
	movl	%ecx, %eax
	movl	%r8d, -20(%rbp)
	movl	%r9d, -24(%rbp)
	movb	%dil, -4(%rbp)
	movw	%ax, -16(%rbp)
	nop
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	fun3, .-fun3
	.globl	fun4
	.type	fun4, @function
fun4:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movss	%xmm0, -4(%rbp)
	movss	%xmm1, -8(%rbp)
	movss	%xmm2, -12(%rbp)
	movsd	%xmm3, -24(%rbp)
	movsd	%xmm4, -32(%rbp)
	movsd	%xmm5, -40(%rbp)
	movss	%xmm6, -16(%rbp)
	movss	-8(%rbp), %xmm0
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	fun4, .-fun4
	.globl	fun5
	.type	fun5, @function
fun5:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$336, %rsp
	movsd	%xmm0, -312(%rbp)
	movl	%edi, %eax
	movq	%rsi, -328(%rbp)
	movw	%ax, -316(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movsd	.LC0(%rip), %xmm0
	movsd	%xmm0, -312(%rbp)
	movw	$2, -316(%rbp)
	movq	-328(%rbp), %rax
	movss	.LC1(%rip), %xmm0
	movss	%xmm0, (%rax)
	movq	16(%rbp), %rax
	movq	%rax, -272(%rbp)
	movq	24(%rbp), %rax
	movq	%rax, -264(%rbp)
	movq	32(%rbp), %rax
	movq	%rax, -256(%rbp)
	movq	40(%rbp), %rax
	movq	%rax, -248(%rbp)
	movq	48(%rbp), %rax
	movq	%rax, -240(%rbp)
	movl	$3, -304(%rbp)
	movb	$4, -32(%rbp)
	movq	-328(%rbp), %rax
	movq	-8(%rbp), %rdx
	xorq	%fs:40, %rdx
	je	.L8
	call	__stack_chk_fail
.L8:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	fun5, .-fun5
	.globl	fun6
	.type	fun6, @function
fun6:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$224, %rsp
	movl	%esi, -184(%rbp)
	movss	%xmm0, -188(%rbp)
	movq	%rdx, -200(%rbp)
	movl	%ecx, %eax
	movl	%r8d, -204(%rbp)
	movl	%r9d, -208(%rbp)
	movb	%dil, -180(%rbp)
	movw	%ax, -192(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movb	$1, -180(%rbp)
	movl	$2, -184(%rbp)
	movss	.LC2(%rip), %xmm0
	movss	%xmm0, -188(%rbp)
	leaq	-180(%rbp), %rax
	movq	%rax, -200(%rbp)
	movl	$4, 16(%rbp)
	movw	$7, -192(%rbp)
	movl	$8, -204(%rbp)
	movl	$9, -208(%rbp)
	movl	$10, 40(%rbp)
	movb	$11, -161(%rbp)
	movss	.LC3(%rip), %xmm0
	movss	%xmm0, -112(%rbp)
	movl	$12, -160(%rbp)
	movl	globali(%rip), %eax
	movl	%eax, -184(%rbp)
	movss	globalf(%rip), %xmm0
	movss	%xmm0, -188(%rbp)
	movq	globalp(%rip), %rax
	movq	%rax, -200(%rbp)
	movl	globalarr+8(%rip), %eax
	movl	%eax, -184(%rbp)
	movl	-208(%rbp), %eax
	movl	%eax, -160(%rbp)
	movss	-188(%rbp), %xmm0
	movss	%xmm0, -112(%rbp)
	movq	-200(%rbp), %rax
	movq	%rax, globalp(%rip)
	leaq	-112(%rbp), %rdx
	movabsq	$4614613358185178726, %rax
	subq	$8, %rsp
	pushq	-128(%rbp)
	pushq	-136(%rbp)
	pushq	-144(%rbp)
	pushq	-152(%rbp)
	pushq	-160(%rbp)
	movq	%rdx, %rsi
	movl	$1, %edi
	movq	%rax, -216(%rbp)
	movsd	-216(%rbp), %xmm0
	call	fun5
	addq	$48, %rsp
	movq	%rax, globalfp(%rip)
	movl	16(%rbp), %edx
	movl	-184(%rbp), %eax
	addl	%edx, %eax
	addl	$5, %eax
	movq	-8(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L11
	call	__stack_chk_fail
.L11:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	fun6, .-fun6
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$112, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movb	$1, -91(%rbp)
	movl	$2, -88(%rbp)
	movss	.LC2(%rip), %xmm0
	movss	%xmm0, -84(%rbp)
	leaq	-91(%rbp), %rax
	movq	%rax, -80(%rbp)
	movl	$4, -48(%rbp)
	movb	$5, -44(%rbp)
	movsd	.LC5(%rip), %xmm0
	movsd	%xmm0, -40(%rbp)
	movl	$6, -64(%rbp)
	movsd	.LC6(%rip), %xmm0
	movsd	%xmm0, -72(%rbp)
	movw	$7, -90(%rbp)
	movb	$9, -16(%rbp)
	movl	-88(%rbp), %eax
	movl	%eax, globali(%rip)
	movss	-84(%rbp), %xmm0
	movss	%xmm0, globalf(%rip)
	movzbl	-14(%rbp), %eax
	movb	%al, -91(%rbp)
	movzbl	-91(%rbp), %eax
	movb	%al, -15(%rbp)
	movq	-80(%rbp), %rax
	movq	%rax, globalp(%rip)
	pushq	-32(%rbp)
	pushq	-40(%rbp)
	pushq	-48(%rbp)
	call	fun1
	addq	$24, %rsp
	movq	-64(%rbp), %rdx
	movq	-56(%rbp), %rax
	movq	%rdx, %rdi
	movq	%rax, -104(%rbp)
	movsd	-104(%rbp), %xmm0
	call	fun2
	subq	$8, %rsp
	pushq	$7
	movl	$6, %r9d
	movl	$5, %r8d
	movl	$4, %ecx
	movl	$3, %edx
	movl	$2, %esi
	movl	$1, %edi
	call	fun3
	addq	$16, %rsp
	movsd	.LC5(%rip), %xmm2
	movsd	.LC7(%rip), %xmm1
	movsd	.LC8(%rip), %xmm0
	movss	.LC9(%rip), %xmm6
	movapd	%xmm2, %xmm5
	movapd	%xmm1, %xmm4
	movapd	%xmm0, %xmm3
	movss	.LC2(%rip), %xmm2
	movss	.LC10(%rip), %xmm1
	movss	.LC11(%rip), %xmm0
	call	fun4
	movd	%xmm0, %eax
	movl	%eax, -84(%rbp)
	movswl	-90(%rbp), %ecx
	movzbl	-91(%rbp), %eax
	movsbl	%al, %eax
	leaq	-91(%rbp), %rdx
	movl	-84(%rbp), %edi
	movl	-88(%rbp), %esi
	pushq	$3
	pushq	-32(%rbp)
	pushq	-40(%rbp)
	pushq	-48(%rbp)
	movl	$2, %r9d
	movl	$1, %r8d
	movl	%edi, -104(%rbp)
	movss	-104(%rbp), %xmm0
	movl	%eax, %edi
	call	fun6
	addq	$32, %rsp
	movl	%eax, -88(%rbp)
	nop
	movq	-8(%rbp), %rax
	xorq	%fs:40, %rax
	je	.L13
	call	__stack_chk_fail
.L13:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.section	.rodata
	.align 8
.LC0:
	.long	0
	.long	1072693248
	.align 4
.LC1:
	.long	1077516698
	.align 4
.LC2:
	.long	1077936128
	.align 4
.LC3:
	.long	1082130432
	.align 8
.LC5:
	.long	0
	.long	1075314688
	.align 8
.LC6:
	.long	0
	.long	1075838976
	.align 8
.LC7:
	.long	0
	.long	1075052544
	.align 8
.LC8:
	.long	0
	.long	1074790400
	.align 4
.LC9:
	.long	1088421888
	.align 4
.LC10:
	.long	1073741824
	.align 4
.LC11:
	.long	1065353216
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.12) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
