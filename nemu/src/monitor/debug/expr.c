#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NUMBER

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
	{"\\+", '+'},					// plus
	{"==", EQ},						// equal
	{"-", '-'},
	{"\\*", '*'},
	{"/", '/'},
	{"\\(", '('},
	{"\\)", ')'},
	{"[1-9][0-9]*", NUMBER},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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
					case NUMBER:
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
	int parens_nr = 0, i, op, flag = 0;
	*success = false;
	for(i = q; i >= p; --i){
		if(tokens[i].type == ')')
			parens_nr += 1;
		if(tokens[i].type == '(')
			parens_nr -= 1;
		if(parens_nr == 0){
			if(tokens[i].type == '*' || tokens[i].type == '/'){
				if(!flag) {
					*success = true;
					op = tokens[i].type;
					flag = 1;
				}
			}
			if(tokens[i].type == '+' || tokens[i].type == '-') {
				*success = true;
				op = tokens[i].type;
				break;
			}
		}
	}
	return op;
}

uint32_t eval(Token *tokens, int p, int q, bool *success){
	if(p > q) {
		*success = false;
		return 0;
	}
	else if(p == q) {
		if(tokens[p].type == NUMBER){
			*success = true;
			return strtoul(tokens[p].str, NULL, 10);
		}
		else {
			*success = false;
			return 0;
		}
	}
	else{
		if(check_parentheses(tokens, p, q, success) == true) {
			return eval(tokens, p + 1, q - 1, success);
		}
		if(*success == false) {
			return 0;
		}
		int op = get_dominant_op(tokens, p, q, success);
		if(*success == false) {
			return 0;
		}
		uint32_t val1 = eval(tokens, p, op - 1, success);
		if(*success == false) {
			return 0;
		}
		uint32_t val2 = eval(tokens, op + 1, q, success);
		if(*success == false) {
			return 0;
		}
		*success = true;
		switch(op_type) {
			case '+': return val1 + val2;break;
			case '-': return val1 - val2;break;
			case '*': return val1 * val2;break;
			case '/': return val1 / val2;break;
			default: assert(0);break;
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	/* TODO: Insert codes to evaluate the expression. */
	return eval(tokens, 0, nr_token - 1, success);
}

