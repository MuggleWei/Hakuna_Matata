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

str_hello:
	.string "hello world\n"

# --------------------------------
# Section: text
# --------------------------------
	.section .text
	.globl _start

_start:

exit:
	# system call: write(stdout, str_hello, 12)
	movl $4, %eax  # system call: write
	movl $1, %ebx  # output file descripte
	movl $str_hello, %ecx  # get str address
	movl $12, %edx
	int $0x80

	# system call: exit(0)
	movl $1, %eax
	movl $0, %ebx
	int $0x80
