#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <elf.h>

void cpu_exec(uint32_t);
extern uint32_t expr(char *, bool *);

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

extern uint64_t time_consuming;
static int cmd_c(char *args) {
	cpu_exec(-1);
	printf("%llu\n", time_consuming);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *);

static int cmd_si(char *);

static int cmd_info(char *);

static void print_regs();

static int cmd_x(char *);

static int cmd_p(char *);

static int cmd_w(char *);

static int cmd_d(char *);

static int cmd_bt(char *);

static int cmd_page(char *);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Step N instruction exactly", cmd_si},
	{ "info", "List of integer registers and watchpoints", cmd_info},
	{ "x", "Print ram", cmd_x},
	{ "p", "Print value", cmd_p},
	{ "w", "Set watchpoint", cmd_w},
	{ "d", "Delete watchpoint", cmd_d},
	{ "bt", "Print stack frame list", cmd_bt},
	{ "page", "Show the physical address according to the virtual address", cmd_page},

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))
#define NR_RAM_UNIT_EACH_LINE 4
#define MAX_FUNC_NUM 1000

typedef struct {
    swaddr_t prev_ebp;
	swaddr_t ret_addr;
	uint32_t args[4];
} PartOfStackFrame;

extern char *strtab;
extern Elf32_Sym *symtab;
extern int nr_symtab_entry;
static struct {
	char str[32];
	swaddr_t begin;
	swaddr_t end;
} func_info[MAX_FUNC_NUM];
static int func_num = 0;

void get_func_info(void) {
	int i;
	for(i = 0; i < nr_symtab_entry; ++i) {
		Elf32_Sym *syb = symtab + i;
		if(ELF32_ST_TYPE(syb->st_info) == STT_FUNC) {
			if(func_num >= MAX_FUNC_NUM)
				assert(0);
			func_info[func_num].begin = syb->st_value;
			func_info[func_num].end = syb->st_value + syb->st_size;
			strcpy(func_info[func_num++].str, strtab + syb->st_name);
		}
	}
	/*for(i = 0; i < func_num; ++i) {*/
		/*printf("%d\t%d\t%s\n", func_info[i].begin, func_info[i].end, func_info[i].str);*/
	/*}*/
}

extern hwaddr_t page_translate(lnaddr_t);

static int cmd_page(char *args) {
	if(args == NULL) {
		printf("Please input right address!\n");
		return 0;
	}
	uint32_t addr = -1;
	if(sscanf(args, "%x", &addr) < 1){
		printf("Invalid watchpoint number.\n");
		return 0;
	}

	printf("virtual address: 0x%x\n", addr);
	printf("physical address: 0x%x\n", page_translate(addr));
	return 0;
}

static int cmd_bt(char *args) {
	swaddr_t ebp = cpu.ebp, eip = cpu.eip;
	int i;
	for(; ebp != 0; ) {
		printf("0x%08x\t", eip);
		for(i = 0; i < func_num; ++i) {
			if(eip >= func_info[i].begin && eip < func_info[i].end) {
				printf("%s\t", func_info[i].str);
				break;
			}
		}
		for(i = 0; i < 16; i += 4) {
			printf("0x%08x\t", swaddr_read(ebp + 8 + i, 4, SS));
		}
		eip = swaddr_read(ebp + 4, 4, SS);
		ebp = swaddr_read(ebp, 4, SS);
		printf("\n");
	}
	return 0;
}

static int cmd_d(char *args) {
	if(!args) {
		printf("Please input right args!\n");
		return 0;
	}
	int no = -1;
	if(sscanf(args, "%d", &no) < 1){
		printf("Invalid watchpoint number.\n");
		return 0;
	}
	if(!free_wp(no)){
		printf("No required watchpoint.\n");
		return 0;
	}
	return 0;
}

static int cmd_w(char *args) {
	if(!args) {
		printf("Please input right args!\n");
		return 0;
	}
	while(*args == ' ')	++args;
	WP *wp = new_wp();
	if(!wp) {
		printf("Cannot set more watchpoint.\n");
		return 0;
	}
	if(strlen(args) + 1 >= EXPR_LEN) {
		if(!free_wp(wp->NO)){
			printf("No required watchpoint.\n");
			return 0;
		}
		printf("Expression too long.\n");
		return 0;
	}
	bool success = false;
	wp->old_val = expr(args, &success);
	if(!success) {
		if(!free_wp(wp->NO)){
			printf("No required watchpoint.\n");
			return 0;
		}
		printf("Invalid expression.\n");
		return 0;
	}
	strcpy(wp->str, args);
	return 0;
}

static int cmd_p(char *args) {
	if(!args) {
		printf("Please input right args!\n");
		return 0;
	}
	static uint32_t count = 0;
	bool success;
	uint32_t res = expr(args, &success);
	if(!success) {
		printf("Invalid expression.\n");
	}
	else {
		printf("$%d = %d\t0x%08x\n", ++count, res, res);
	}
	return 0;
}

static int cmd_x(char *args) {
	if(!args) {
		printf("Please input right args!\n");
		return 0;
	}
	uint32_t num, addr, i, j;
	char *n = strtok(NULL, " ");
	if(sscanf(n, "%u", &num) < 1){
		printf("Invalid ram unit numbers.\n");
		return 0;
	}
	char *expression = n + strlen(n) + 1;
	bool success = false;
	addr = expr(expression, &success);
	if(!success){
		printf("Invalid expression.\n");
		return 0;
	}
	for(i = 0; i < num; i += NR_RAM_UNIT_EACH_LINE) {
		printf("0x%08x:\t", addr + i * 4);
		for(j = 0; i + j< num && j < NR_RAM_UNIT_EACH_LINE; ++j)
			printf("0x%08x\t", swaddr_read(addr + (i + j) * 4, 4, DS));
		printf("\n");
	}
	return 0;
}

static void print_regs() {
	printf("eax\t\t0x%08x\t%d\n", cpu.eax, cpu.eax);
	printf("ecx\t\t0x%08x\t%d\n", cpu.ecx, cpu.ecx);
	printf("edx\t\t0x%08x\t%d\n", cpu.edx, cpu.edx);
	printf("ebx\t\t0x%08x\t%d\n", cpu.ebx, cpu.ebx);
	printf("esp\t\t0x%08x\t%d\n", cpu.esp, cpu.esp);
	printf("ebp\t\t0x%08x\t%d\n", cpu.ebp, cpu.ebp);
	printf("esi\t\t0x%08x\t%d\n", cpu.esi, cpu.esi);
	printf("edi\t\t0x%08x\t%d\n", cpu.edi, cpu.edi);
	printf("eip\t\t0x%08x\t%d\n", cpu.eip, cpu.eip);
	printf("gdtr.limit\t0x%04x\n", cpu.gdtr.limit);
	printf("gdtr.base\t0x%08x\n", cpu.gdtr.base);
	printf("cr0\t\t0x%08x\n", cpu.cr0);
	printf("cr3\t\t0x%08x\n", cpu.cr3.val);
	printf("cs\t\t0x%04x\t0x%08x\t0x%08x\n", cpu.cs.selector, cpu.cs.base, cpu.cs.limit);
	printf("ds\t\t0x%04x\t0x%08x\t0x%08x\n", cpu.ds.selector, cpu.ds.base, cpu.ds.limit);
	printf("es\t\t0x%04x\t0x%08x\t0x%08x\n", cpu.es.selector, cpu.es.base, cpu.es.limit);
	printf("ss\t\t0x%04x\t0x%08x\t0x%08x\n", cpu.ss.selector, cpu.ss.base, cpu.ss.limit);
}

static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	if(!args) {
		printf("Please input right args!\n");
		return 0;
	}
	if(strcmp(arg, "r") == 0)
		print_regs();
	else if(strcmp(arg, "w") == 0)
		print_watchpoints();
	else
		printf("Unknown command '%s'\n", arg);
	return 0;
}

static int cmd_si(char *args) {
	if(!args) {
		printf("Please input right args!\n");
		return 0;
	}
	int steps;
	/*printf("%d\n", strlen(args));*/
	sscanf(args, "%d", &steps);
	/*printf("%d\n", steps);*/
	cpu_exec(steps);
	return 0;
}

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	get_func_info();
	while(1) {
		char *str = rl_gets();
		/*printf("%s\n", str);*/
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
