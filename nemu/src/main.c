#include<stdlib.h>
#include<stdio.h> 

void init_monitor(int, char *[]);
void reg_test();
void restart();
void ui_mainloop();

int main(int argc, char *argv[]) {

	/* Initialize the monitor. */
	init_monitor(argc, argv);

	/* Test the implementation of the ``CPU_state'' structure. */
	reg_test();

	/* Initialize the virtual computer system. */
	restart();
	/*printf("%d\t%d\n", sizeof(long), sizeof(long long));*/
	/*unsigned int b = -1;*/
	/*long long c = b;*/
	/*printf("%d\t%lld\n", b, c);*/

	/* Receive commands from user. */
	ui_mainloop();

	return 0;
}
