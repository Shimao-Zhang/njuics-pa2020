#include "nemu.h"
#include "cpu/reg.h"
#include "memory/memory.h"

#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include<cpu/cpu.h>
#include<elf.h>
extern uint32_t look_up_symtab(char* sym,bool* success);

enum
{
	NOTYPE = 256,
	EQ,             //equal
	UEQ,            //unequal
	NUM,            //number
	BNUM,           //binary number
	ONUM,           //OCTAL system number
	HNUM,           //hexadecimal number
	REG,            //register
	PREG,           //pointer register
	SYMB,           //symbol
	DEREF,          //a pointer
	NEG,            //negative,not subtraction
	AND,            //&&
	OR,             //||
	FUN             //function

	/* TODO: Add more token types */

};

static struct rule
{
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +", NOTYPE}, // white space
	{"\\+", '+'},
	{"\\-", '-'},
	{"\\*", '*'},
	{"\\/", '/'},
	{"\\(", '('},
	{"\\)", ')'},
	{"==", EQ},
	{"!=", UEQ},
	//{"[0-9]+", NUM},
	{"0[Xx][0-9a-fA-F]+", HNUM},
	{"[0-9a-fA-F]+[Hh]", HNUM},
	{"0[Bb][0-1]+", BNUM},
	{"[0-9]+", NUM},
	{"%e[a-d]x", REG},
	{"%esp|%ebp|%eip", PREG},
	{"&&", AND},
	{"\\|\\|", OR},      //Pay attention to this!!!
	{"\\!", '!'},
	{"[a-zA-Z_][a-zA-Z_0-9]+", SYMB},
	{"[a-zA-Z_][a-zA-Z0-9_]+", FUN}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX];

/* Rules are used for more times.
 * Therefore we compile them only once before any usage.
 */
void init_regex()
{
	int i;
	char error_msg[128];
	int ret;

	for (i = 0; i < NR_REGEX; i++)
	{
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if (ret != 0)
		{
			regerror(ret, &re[i], error_msg, 128);
			assert(ret != 0);
		}
	}
}

typedef struct token
{
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e)
{
	int position = 0;
	int i;
	regmatch_t pmatch;

	nr_token = 0;

	while (e[position] != '\0')
	{
		/* Try all rules one by one. */
		for (i = 0; i < NR_REGEX; i++)
		{
			if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0)
			{
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				printf("match regex[%d] at position %d with len %d: %.*s\n", i, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. 
				 * Add codes to perform some actions with this token.
				 */

				switch (rules[i].token_type)
				{
				case NOTYPE:
				    break;
				case EQ:
				    tokens[nr_token].type=rules[i].token_type;
				    nr_token++;
				    break;
				case UEQ:
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case NUM:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    nr_token++;
				    break;
				case BNUM:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    nr_token++;
				    break;
				case ONUM:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    ++nr_token;
				    break;
				case HNUM:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    ++nr_token;
				    break;
				case REG:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    ++nr_token;
				    break;
				case PREG:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    ++nr_token;
				    break;
				case '+':
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case '-':
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case '*':
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case '/':
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case '(':
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case ')':
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case AND:
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case OR:
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case '!':
				    tokens[nr_token++].type=rules[i].token_type;
				    break;
				case SYMB:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    ++nr_token;
				    break;
				case FUN:
				    tokens[nr_token].type=rules[i].token_type;
				    strncpy(tokens[nr_token].str,substr_start,substr_len);
				    tokens[nr_token].str[substr_len]='\0';
				    ++nr_token;
				    break;
				default:
					tokens[nr_token].type = rules[i].token_type;
					nr_token++;
				}

				break;
			}
		}

		if (i == NR_REGEX)
		{
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true;
}

bool check_parentheses(int p,int q)
{
    int count=0;
    for(int i=p;i<=q;++i)
    {
        if(tokens[i].type=='(')
            ++count;
        if(tokens[i].type==')')
            --count;
        if(count<0)
            return false;
    }
    if(count!=0)
        return false;
    count=0;
    if(tokens[p].type!='('||tokens[q].type!=')')
        return false;
    for(int i=p+1;i<q;++i)
    {
        if(tokens[i].type=='(')
            ++count;
        if(tokens[i].type==')')
            --count;
        if(count<0)
            return false;
    }
    if(count!=0)
        return false;
    return true;
}

bool judge_again(int p,int q)
{
    int count=0;
    for(int i=p;i<=q;++i)
    {
        if(tokens[i].type=='(')
            ++count;
        if(tokens[i].type==')')
            --count;
        if(count<0)
            return false;
    }
    if(count!=0)
        return false;
    return true;
}

uint32_t eval(int p,int q)
{
    if(p>q)
    {
        printf("This is a bad expression.\n");
        return 0;
    }
    else if(p==q)
    {
        if(tokens[p].type==NUM)
        {
            int res=0;
            int len=strlen(tokens[p].str);
            for(int i=0;i<len;++i)
            {
                int a=tokens[p].str[i]-'0';
                for(int k=0;k<len-i-1;++k)
                    a*=10;
                res+=a;
            }
            return res;
        }
        if(tokens[p].type==BNUM)
        {
            int res=0;
            int len=strlen(tokens[p].str);
            for(int i=2;i<len;++i)
            {
                int a=tokens[p].str[i]-'0';
                for(int k=0;k<len-i-1;++k)
                    a*=2;
                res+=a;
            }
            return res;
        }
        if(tokens[p].type==ONUM)
        {
            int res=0;
            int len=strlen(tokens[p].str);
            for(int i=2;i<len;++i)
            {
                int a=tokens[p].str[i]-'0';
                for(int k=0;k<len-i-1;++k)
                    a*=8;
                res+=a;
            }
            return res;
        }
        if(tokens[p].type==HNUM&&tokens[p].str[0]=='0'&&(tokens[p].str[1]=='x'||tokens[p].str[1]=='X'))
        {
            int res=0;
            int len=strlen(tokens[p].str);
            for(int i=2;i<len;++i)
            {
                int a;
                if(tokens[p].str[i]>='0'&&tokens[p].str[i]<='9')
                    a=tokens[p].str[i]-'0';
                else if(tokens[p].str[i]>='a'&&tokens[p].str[i]<='f')
                    a=tokens[p].str[i]-'a'+10;
                else
                    a=tokens[p].str[i]-'A'+10;
                for(int k=0;k<len-i-1;++k)
                    a*=16;
                res+=a;
            }
            return res;
        }
        else if(tokens[p].type==HNUM)
        {
            int res=0;
            int len=strlen(tokens[p].str);
            for(int i=0;i<len-1;++i)
            {
                int a;
                if(tokens[p].str[i]>='0'&&tokens[p].str[i]<='9')
                    a=tokens[p].str[i]-'0';
                else if(tokens[p].str[i]>='a'&&tokens[p].str[i]<='f')
                    a=tokens[p].str[i]-'a'+10;
                else
                    a=tokens[p].str[i]-'A'+10;
                for(int k=0;k<len-i-2;++k)
                    a*=16;
                res+=a;
            }
            return res;
        }
        if(tokens[p].type==REG)
        {
            int res=0;
            if(strcmp(tokens[p].str,"%eax")==0)
                res=cpu.eax;
            if(strcmp(tokens[p].str,"%ebx")==0)
                res=cpu.ebx;
            if(strcmp(tokens[p].str,"%ecx")==0)
                res=cpu.ecx;
            if(strcmp(tokens[p].str,"%edx")==0)
                res=cpu.edx;
            return res;
        }
        if(tokens[p].type==PREG)
        {
            int res=0;
            if(strcmp(tokens[p].str,"%ebp")==0)
                res=cpu.ebp;
            if(strcmp(tokens[p].str,"%esp")==0)
                res=cpu.esp;
            if(strcmp(tokens[p].str,"%eip")==0)
                res=cpu.eip;
            return res;
        }
        if(tokens[p].type==SYMB)
        {
            bool success;
            success=true;
            int a=look_up_symtab(tokens[p].str,&success);
            if(success)
                return a;
            else
            {
                printf("NO THIS SYMBOL\n");
                return -1;
            }
        }
        if(tokens[p].type==FUN)
        {
            bool success;
            success=true;
            int a=look_up_symtab(tokens[p].str,&success);
            if(success)
                return a;
            else
            {
                printf("NO THIS FUNCTION\n");
                return -1;
            }
        }
    }
    else if(check_parentheses(p,q)==true)
    {
        return eval(p+1,q-1);
    }
    else
    {
        if(judge_again(p,q))
        {
            int op=0;
            int level=-999;
            for(int i=p;i<=q;++i)
            {
                if(tokens[i].type=='(')
                {
                    int count=1;
                    do
                    {
                        ++i;
                        if(tokens[i].type=='(')
                            ++count;
                        if(tokens[i].type==')')
                            --count;
                    }
                    while(count!=0);
                }
                if(tokens[i].type==NEG||tokens[i].type==DEREF||tokens[i].type=='!')    
                //the smaller the level is, the higher the priority is
                {
                    if(level<=0)
                    {
                        op=i;
                        level=0;
                    }
                }
                if(tokens[i].type==EQ || tokens[i].type==UEQ)
                {
                    if(level<=3)
                    {
                        op=i;
                        level=3;
                    }
                }
                if(tokens[i].type==AND)
                {
                    if(level<=4)
                    {
                        op=i;
                        level=4;
                    }
                }
                if(tokens[i].type==OR)
                {
                    if(level<=5)
                    {
                        op=i;
                        level=5;
                    }
                }
                if(tokens[i].type=='+'||tokens[i].type=='-')
                {
                    if(level<=2)
                    {
                        op=i;
                        level=2;
                    }
                }
                if(tokens[i].type=='*'||tokens[i].type=='/')
                {
                    if(level<=1)
                    {
                        op=i;
                        level=1;
                    }
                }
            }
            //printf("%d\n",op);
            switch(tokens[op].type)
            {
                case EQ:
                    if(eval(p,op-1)==eval(op+1,q))
                        return 1;
                    else
                        return 0;
                    break;
                case UEQ:
                    if(eval(p,op-1)!=eval(op+1,q))
                        return 1;
                    else
                        return 0;
                    break;
                case DEREF:
                    return vaddr_read(eval(op+1,q),SREG_DS,4);
                    break;
                case NEG:
                    return -eval(op+1,q);
                    break;
                case '+':
                    return eval(p,op-1)+eval(op+1,q);
                    break;
                case '-':
                    return eval(p,op-1)-eval(op+1,q);
                    break;
                case '*':
                    return eval(p,op-1)*eval(op+1,q);
                    break;
                case '/':
                    return eval(p,op-1)/eval(op+1,q);
                    break;
                case '!':
                    if(eval(op+1,q)!=0)
                        return 0;
                    else
                        return 1;
                    break;
                case AND:
                    return eval(p,op-1)&&eval(op+1,q);
                    break;
                case OR:
                    return eval(p,op-1)||eval(op+1,q);
                    break;
            }
        }
        else
        {
            printf("This is a wrong expression.\n");
            return -1;
        }
    }
    return 0;
}

uint32_t expr(char *e, bool *success)
{
	if (!make_token(e))
	{
		*success = false;
		return 0;
	}
	else
	    *success=true;

	/*printf("\nPlease implement expr at expr.c\n");
	fflush(stdout);
	assert(0);*/
	for(int i=0;i<nr_token;++i)
	{
	    if(tokens[i].type=='*'&&(i==0||tokens[i-1].type=='+'||tokens[i-1].type=='-'||tokens[i-1].type=='*'||tokens[i-1].type=='/'||tokens[i-1].type=='('||tokens[i-1].type==EQ||tokens[i-1].type==UEQ))
	        tokens[i].type=DEREF;
	    if(tokens[i].type=='-'&&(i==0||tokens[i-1].type=='('||tokens[i-1].type==EQ||tokens[i-1].type==UEQ))
	        tokens[i].type=NEG;
	}

	return eval(0,nr_token-1);
}
