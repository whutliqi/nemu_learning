#include <isa.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_NUM,
  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"==", TK_EQ},        // equal
  {"\\-",'-'},
  {"\\*",'*'},
  {"\\/",'/'},
  {"\\(",'('},
  {"\\)",')'},
  {"[0123456789]+",TK_NUM},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
	Token t0 = {0, {}};
	for(int i=0; i<32; i++)
		tokens[i] = t0;
	nr_token = 0;
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
			case '+':
			case '-':
			case '*':
			case '/':
			case '(':
			case ')':	
			{
			tokens[nr_token].type = rules[i].token_type;
			nr_token++;	
			break;	
			}
			case TK_NUM:
			{
			tokens[nr_token].type = rules[i].token_type;
			for(int index=0; index<substr_len; index++){
				tokens[nr_token].str[index] = *(substr_start + index);
			}
			nr_token++;
			break;		
			}
			default: {}
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

int eval(int p, int q);
bool check_parentheses(int p, int q);

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  for(int i = 0 ; i<nr_token; i++){
	printf("type is %d\n", tokens[i].type);
    if(tokens[i].type == 258){
	printf("value is %d\n",atoi(tokens[i].str));
	}	
  }
   int ans = eval(0, nr_token-1);
	printf("the ans is %d\n", ans);	
  return ans;
}

bool check_parentheses(int p, int q){
	if((tokens[p].type == '(') && (tokens[q].type == ')')){
		return true;
	}
	else{
		return false;
	}
}
int eval(int p, int q){
	if(p > q){
	
	}
	else if(p == q){
		if(tokens[q].type == TK_NUM){
			return atoi(tokens[p].str);	
		}	
	}
	else if(check_parentheses(p, q) == true){
		return eval(p+1, q-1);	
	}
	else{
		int priority = 0;
		int op_type = 0;
		int op_index = 0;
		int parentheses_flag = 0;
		for(int i=p; i<q; i++){
			if(tokens[i].type == '('){
				parentheses_flag ++;
			}
			if(tokens[i].type == ')'){
				parentheses_flag --;
			}
			if(parentheses_flag == 0)
			{
				switch(tokens[i].type){
				case TK_NUM: break;
				case '*':
				case '/':
					 {
						if(priority <= 1){
							op_type = tokens[i].type;
							priority = 1;
							op_index = i;
						}			
						break;
				 }
				case '+':
				case '-':
					 {
						if(priority <= 2){
							op_type = tokens[i].type;
							priority = 2;
							op_index = i;
						}			
						break;
				 }
		   	  }
			}
		  }
		
		int val1 = eval(p, op_index-1);
		int val2 = eval(op_index+1, q);
		switch(op_type){
			case '+': return val1 + val2;		
			case '-': return val1 - val2;		
			case '*': return val1 * val2;		
			case '/': return val1 / val2;
			default:assert(0);		  
		}
		}	
    return 0;
}

