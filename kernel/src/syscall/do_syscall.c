#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);
void serial_printc(char);
int fs_open(const char *pathname, int flags);	/* 在我们的实现中可以忽略flags */
int fs_read(int fd, void *buf, int len);
int fs_write(int fd, void *buf, int len);
int fs_lseek(int fd, int offset, int whence);
int fs_close(int fd);

static void sys_brk(TrapFrame *tf) {
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;

		/* TODO: Add more system calls. */
		case SYS_open:
				tf->eax = fs_open((char *)(tf->ebx), tf->ecx);	  
				break;
		case SYS_read:
				tf->eax = fs_read(tf->ebx, (void *)(tf->ecx), tf->edx);
				break;
		case SYS_write:
				tf->eax = fs_write(tf->ebx, (void *)(tf->ecx), tf->edx);
				break;
		case SYS_lseek:
				tf->eax = fs_lseek(tf->ebx, tf->ecx, tf->edx);
				break;
		case SYS_close:
				tf->eax = fs_close(tf->ebx);
				break;
		default: panic("Unhandled system call: id = %d", tf->eax);
	}
}

