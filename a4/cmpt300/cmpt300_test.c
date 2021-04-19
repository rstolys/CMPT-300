#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(cmpt300_test, int, arg)
{
	long result = 0;

	printk("Hello World!\n");
	printk("--scall argument %d\n", arg);

	result = arg + 1;
	printk("--returning %d + 1 = %ld\n", arg, result);
	return result;
}
