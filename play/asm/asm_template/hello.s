#
# Brief: system call exit(0)
#
# Registers:
#
# Memory:
#

# --------------------------------
# Section: data
# --------------------------------
	.section .data

# --------------------------------
# Section: text
# --------------------------------
	.section .text
	.globl _start

_start:

exit:
	# system call: exit(0)
	movl $1, %eax
	movl $0, %ebx
	int $0x80
