#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

uint32_t expr(char *, bool *);

enum {
	NOTYPE = 256, DECIMAL_NUM, HEX_NUM, REGS, EQ, NEQ, AND, XOR, NOT, DEREF 

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"0x[0-9a-f]+", HEX_NUM},		// hex number
	//{"0|[1-9]\\d*", DECIMAL_NUM},	// decimal number
	{"0|[1-9][0-9]*", DECIMAL_NUM},	// decimal number
	{"\\$[a-z]+", REGS},			// registers
	{"\\(", '('},					// left parenthese
	{"\\)", ')'},					// right parenthese
	{"\\+", '+'},					// plus
	{"-", '-'},						// minus or derefference
	{"\\*", '*'},					// multiple
	{"/", '/'},						// divide
	{"==", EQ},						// equal
	{"!=", NEQ},					// not equal
	{"&&", AND},					// logical and
	{"\\|\\|", XOR},				// logical xor
	{"!", NOT},						// logical not
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

#define REGEX_TEST(exp) \
{	\
	bool success = false;	\
	uint32_t result = expr(#exp, &success); \
	if(success) \
		assert(result == exp); \
	else \
		printf("invalid expression\n"); \
}

static regex_t re[NR_REGEX];
static uint8_t token_priority[300];


/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
	token_priority['!'] = token_priority[DEREF] = 200;
	token_priority['*'] = token_priority['/'] = 199;
	token_priority['+'] = token_priority['-'] = 198;
	token_priority[EQ] = token_priority[NEQ] = 197;
	token_priority[AND] = 196;
	token_priority[XOR] = 195;

	//REGEX_TEST((21 - 1));
	//REGEX_TEST((4 + 3 * (2 - 1)));
	//REGEX_TEST(4 + 3 * (2 - 1));
	//REGEX_TEST((4 + 3) * (2 - 1));
	////REGEX_TEST((4 + 3)) * ((2 - 1));
	//bool success;
	//uint32_t result = expr("(4 + 3)) * ((2 - 1)", &success);
	//if(success) 
		//result += 1;
		////assert(result == (4 + 3)) * ((2 - 1));
	//else 
		//printf("invalid expression\n"); 
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array ``tokens''. For certain 
				 * types of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case DECIMAL_NUM:
					case HEX_NUM:
					case REGS:
						if(substr_len >= 32)
							assert(0);
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						tokens[nr_token].str[substr_len] = '\0';
						tokens[nr_token++].type = rules[i].token_type;
						break;
					case NOTYPE:
						break;
					default: 
						tokens[nr_token++].type = rules[i].token_type;
						break;
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(Token *tokens, int p, int q, bool *success) {
	int left_parenthese_nr = 0, right_parenthese_nr = 0, i;
	for(i = p; i <= q; ++i){
		if(tokens[i].type == '(')
			left_parenthese_nr += 1;
		if(tokens[i].type == ')') {
			right_parenthese_nr += 1;
			if(right_parenthese_nr > left_parenthese_nr){
				*success = false;
				return false;
			}
		}
	}
	if(right_parenthese_nr != left_parenthese_nr){
		*success = false;
		return false;
	}
	*success = true;
	if(tokens[p].type == '(' && tokens[q].type == ')') {
		left_parenthese_nr = right_parenthese_nr = 0;
		for(i = p + 1; i <= q - 1; ++i){
			if(tokens[i].type == '(')
				left_parenthese_nr += 1;
			if(tokens[i].type == ')') {
				right_parenthese_nr += 1;
				if(right_parenthese_nr > left_parenthese_nr){
					return false;
				}
			}
		}
		return true;
	}
	return false;
}

int get_dominant_op(Token *tokens, int p, int q, bool *success) {
	int parens_nr = 0, i, op = -1, priority = 0;
	*success = false;
	for(i = p; i <= q; ++i){
		int type = tokens[i].type;
		if(type == '(')
			parens_nr += 1;
		if(type == ')')
			parens_nr -= 1;
		if(parens_nr == 0){
			switch(type){
				case DECIMAL_NUM:
				case HEX_NUM:
				case REGS:
				case '(':
				case ')':	continue;
				default:	break;
			}
			if(op == -1) {
				*success = true;
				op = i;
				priority = token_priority[type];
			}
			else {
				if(token_priority[type] <= priority){
					op = i;
					priority = token_priority[type];
				}
			}
		}
	}
	return op;
}

char registers[][5] = {
	"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "eip"
};

uint32_t eval(Token *tokens, int p, int q, bool *success){
	if(p > q) {
		*success = false;
		return 0;
	}
	else if(p == q) {
		if(tokens[p].type == DECIMAL_NUM){
			*success = true;
			return strtoul(tokens[p].str, NULL, 10);
		}
		else if(tokens[p].type == HEX_NUM){
			*success = true;
			return strtoul(tokens[p].str, NULL, 16);
		}
		else if(tokens[p].type == REGS){
			int i;
			for(i = 0; i < 9; ++i) {
				if(strcmp(tokens[p].str + 1, registers[i]) == 0)
					break;
			}
			switch(i) {
				case 0:	*success = true;	return cpu.eax;
				case 1:	*success = true;	return cpu.ecx;
				case 2:	*success = true;	return cpu.edx;
				case 3:	*success = true;	return cpu.ebx;
				case 4:	*success = true;	return cpu.esp;
				case 5:	*success = true;	return cpu.ebp;
				case 6:	*success = true;	return cpu.esi;
				case 7:	*success = true;	return cpu.edi;
				case 8:	*success = true;	return cpu.eip;
				default: *success = false;	return 0;
			}
		}
		else {
			*success = false;
			return 0;
		}
	}
	else{
		if(check_parentheses(tokens, p, q, success) == true) {
			/*puts("1");*/
			return eval(tokens, p + 1, q - 1, success);
		}
		if(*success == false) {
			return 0;
		}
		int op = get_dominant_op(tokens, p, q, success);
		if(*success == false) {
			return 0;
		}
		/*printf("%d\n", op);*/
			/*puts("2");*/
		uint32_t val1 = 0;
		if(tokens[op].type != NOT && tokens[op].type != DEREF) {
			val1 = eval(tokens, p, op - 1, success);
			if(*success == false) {
				return 0;
			}
		}
			/*puts("3");*/
		uint32_t val2 = eval(tokens, op + 1, q, success);
		if(*success == false) {
			return 0;
		}
		*success = true;
		switch(tokens[op].type) {
			case '+': return val1 + val2;break;
			case '-': return val1 - val2;break;
			case '*': return val1 * val2;break;
			case '/': return val1 / val2;break;
			case EQ : return val1 == val2;break;
			case NEQ: return val1 != val2;break;
			case AND: return val1 && val2;break;
			case XOR: return val1 || val2;break;
			case NOT: return !val2;break;
			case DEREF: return swaddr_read(val2, 4);break;
			default: assert(0);break;
		}
	}
	return 0;
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	int i;
	for(i = 0; i < nr_token; ++i) {
		if(tokens[i].type == '*') {
			if(i == 0){
				tokens[i].type = DEREF;
				continue;
			}
			switch(tokens[i - 1].type){
				case DEREF:
				case '(':
				case '+':
				case '-':
				case '*':
				case '/':
				case EQ:
				case NEQ:
				case AND:
				case XOR:
				case NOT:	tokens[i].type = DEREF;break;

				case DECIMAL_NUM:
				case HEX_NUM:
				case ')':
				case REGS:	break;

				default: assert(0);
			}
		}
	}

	printf("token num: %d\t\n", nr_token);
	for(i = 0; i < nr_token; ++i){
		if(tokens[i].type == DECIMAL_NUM || tokens[i].type == HEX_NUM || tokens[i].type == REGS)
		printf("%s\n", tokens[i].str);
	}

	/* TODO: Insert codes to evaluate the expression. */
	return eval(tokens, 0, nr_token - 1, success);
}

