#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

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

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *);

static int cmd_si(char *);

static int cmd_info(char *);

static void print_regs();

static void print_watchpoints();

static int cmd_x(char *);

static int cmd_p(char *);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Step one instruction exactly.\nUsage: stepi [N]\nArgument N means step N times (or till program stops for another reason).", cmd_si},
	{ "info", "List of integer registers and their contents", cmd_info},
	{ "x", "Print ram", cmd_x},
	{ "p", "Print value", cmd_p},

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

#define NR_RAM_UNIT_EACH_LINE 4

static int cmd_p(char *args){
	static uint32_t count = 0;
	bool success;
	uint32_t res = expr(args, &success);
	if(!success) {
		printf("Invalid expression.\n");
	}
	else {
		printf("$%d = %u\n", ++count, res);
	}
	return 0;
}

static int cmd_x(char *args) {
	uint32_t num, addr, i, j;
	sscanf(args, "%u%x", &num, &addr);
	for(i = 0; i < num; i += NR_RAM_UNIT_EACH_LINE) {
		printf("0x%08x:\t", addr + i * 4);
		for(j = 0; i + j< num && j < NR_RAM_UNIT_EACH_LINE; ++j)
			printf("0x%08x\t", swaddr_read(addr + (i + j) * 4, 4));
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
}

static void print_watchpoints() {

}

static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	if(strcmp(arg, "r") == 0)
		print_regs();
	else if(strcmp(arg, "w") == 0)
		print_watchpoints();
	else
		printf("Unknown command '%s'\n", arg);
	return 0;
}

static int cmd_si(char *args) {
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
