#pragma warning(disable:4996)

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define BEGIN 1
#define END 2
#define VAR 3
#define INT 4
#define WHILE 5
#define IF 6
#define THEN 7
#define ELSE 8
#define DO 9
#define EQUAL 10//赋值
#define PLUS 11
#define STAR 12
#define LP 13
#define RP 14//右括号
#define LT 15
#define LE 16
#define GT 17
#define GE 18
#define NE 19
#define EQ 20//等号
#define POINT 21//间隔符
#define COLON 22//冒号
#define SEMICOLON 23//分号
#define COMMA 24//逗号
#define ID 25//标识符
#define CONSTANT 26//常数
#define PROGRAM 27
#define MINUS 28//减号
#define DIV 29//除号
#define N 30//四元式数组大小


char prog[800], token[80];
char data[800][80];
char data1[800][80];
int number[800];//存储syn码
char number1[800][80];
char var[100][20];
char ch;
int syn, p, err;
int m, sum;
int i = 0;
int j = 0;
int k = 1, kk = 0;
int count = 0;//记录变量
int flag = 0; //判断是否已有语义错误产生 flag=1;
int  lable = 0;//标记注释
int V = -1;//变量定义结束  V=1;
int l;//存储变量数组
int NXQ = 0;


//关键字存储
char *key[18] = {
	"BEGIN","END","VAR","INTEGER","WHILE","IF", "THEN","ELSE","DO",
	"begin","end","var","integer","while","if", "then","else","do"
};


 struct {
	char result[12];
	char ag1[12];
	char op[12];
	char ag2[12];
} four[N];

void read();
void scanner();
void program();
void procedure();
void variable();
void table();
void lrparser();
void condition();
void whiled();
void compound();
int  guanxi();
char* factor();
char* expression();
char* term();
void statement();
void yuju();
char* newtemp();
void  emit(char* op, char* ag1, char* ag2, char* result);



int main() {
	read();
	if (flag == -1) {
		printf("\n未定义标识符“%s”\n\n", token);
	}
	if (flag == 0) {
		p = 0;
		j = 0;
		scanner();
		program();

		for (i = 0; i < NXQ; i++) {
			printf("\n%d :(%s ,%s ,%s ,%s )\n", i, four[i].op, four[i].ag1, four[i].ag2, four[i].result);
		}

	}
	return 0;
}

void read() {
	int i;
	char* str = " ";
	p = 0;
	FILE *fp = fopen("data.txt", "r");
	if (fp == NULL)
	{
		printf("null file\n");
	}
	while (!feof(fp))
	{
		fscanf(fp, "%s", &data[p]);
		strcat(data[p], "  ");
		p++;
	}
	fclose(fp);

	for (j = 0; j < p; j++)
		strcat(prog, data[j]);
	FILE *fpWrite = fopen("table.txt", "w");
	FILE *Write = fopen("program.txt", "w");
	FILE *Var = fopen("variable.txt", "w");
	j = 0;
	p = 0;
	//将源码去掉注释存储在新的文件中
	do {
		scanner();
		switch (syn) {
		case 0:
			break;

		case -1:
			flag = 1;
			printf("(数值越界)");
			break;

		case -2:
			flag = 1;
			fprintf(Write, "%-5s", token);
			printf("%-8s  --> (%-5d标识符错误)\n", token, syn);
			break;

		case -3://注释
			break;

		case -4:
			printf("%-8s  --> (%-5d非法字符)\n", token, syn);
			break;

		case -5:
			printf("左侧大括号‘{’字符不匹配\n");
			break;

		case -6:
			printf("右侧大括号‘}’字符不匹配\n");
			break;

		case 25:
			if (V == 0) {
				fprintf(Var, "%-5s ", token);
				strcpy(var[l++], token);
			}
			if (V == 1) {
				flag = -1;
				for (i = 0; i < l; i++)
					if (strcmp(token, var[i]) == 0)
						flag = 0;
				if (flag == -1)
					return;
			}
			fprintf(Write, "%-5s ", token);
			number[j++] = syn;
			fprintf(fpWrite, "%-8s  --> (%-5d%-5d)\n", token, syn, k);
			printf("%-8s  --> (%-5d%-5d)\n", token, syn, k++);
			break;

		case 26:
			fprintf(Write, "%-5d ", sum);
			number[j++] = syn;
			printf("%-8d  --> (%-5d%-5d)\n", sum, syn, sum);
			fprintf(fpWrite, "%-8d  --> (%-5d%-5d)\n", sum, syn, sum);
			break;
		default:
			if (syn == 1)
				V = 1;
			if (syn == 3)
				V = 0;
			fprintf(Write, "%-5s ", token);
			number[j++] = syn;
			printf("%-8s  --> (%-5d%-5d)\n", token, syn, 0);
			fprintf(fpWrite, "%-8s  --> (%-5d%-5d)\n", token, syn, 0);
			break;
		}
	} while (syn != 0);
	fclose(fpWrite);
	fclose(Write);
	fclose(Var);

	p = 0;
	FILE *fp1 = fopen("program.txt", "r");
	if (fp1 == NULL)
	{
		printf("null file\n");
	}
	while (!feof(fp1))
	{
		fscanf(fp1, "%s ", &data1[p]);
		strcat(data1[p], " ");
		p++;
	}
	fclose(fp1);
	*prog = 0;
	for (j = 0; j < p; j++)
		strcat(prog, data1[j]);
}
/*浏览文本，读取单词*/
void scanner() {
	int n;
	int time = 0;
	syn = 0;
	for (n = 0; n < 8; n++)
		token[n] = '\0';
	//过滤空格
	ch = prog[p++];
	while (ch == ' ' || ch == '\n') {
		ch = prog[p];
		p++;
	}

	if ((ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z'))
	{ //可能是标示符或者变量名
		m = 0;             //整形变量int
		while ((ch >= '0'&&ch <= '9') || (ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) {
			token[m++] = ch;
			ch = prog[p++];
		}
		//识别出一个标示符
		token[m++] = '\0';
		//回退一位
		p--;
		syn = 25;//单纯的标示符
		for (n = 0; n < 18; n++) //将识别出来的字符和已定义的标示符作比较，
			if (strcmp(token, key[n]) == 0) {
				//字符串类别
				syn = n % 9 + 1;
				break;
			}
		if ((strcmp(token, "PROGRAM") == 0) || (strcmp(token, "program") == 0)) {
			//字符串类别
			syn = 27;
		}
	}
	else if ((ch >= '0'&&ch <= '9')) { //数字
		{
			char a = ch;
			sum = 0;
			while ((ch >= '0'&&ch <= '9')) {
				sum = sum * 10 + ch - '0';
				ch = prog[p++];	//读取下一个字符
			}
			m = 0;
			//			printf("%c",ch);
			token[m++] = a;
			if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) {
				token[m++] = ch;
				ch = prog[p++];
				while ((ch >= '0'&&ch <= '9') || (ch >= 'a'&&ch <= 'z') || (ch >= 'A'&&ch <= 'Z')) {
					token[m++] = ch;
					ch = prog[p++];
				}
				syn = -2;

			}
		}
		//回退一下
		p--;
		if (syn != -2)
			syn = 26;//26标示数值类型
					 //限制数组的大小范围
		if (sum > 999999999)
			syn = -1;
	}
	else switch (ch) { //其他字符
	case'<':
		m = 0;
		token[m++] = ch;
		//读取下一个字符
		ch = prog[p++];
		if (ch == '>') {
			syn = 19;
			token[m++] = ch;
		}
		else if (ch == '=') {
			syn = 16;
			token[m++] = ch;
		}
		else {
			syn = 15;
			p--;
		}
		break;
	case'>':
		m = 0;
		token[m++] = ch;
		//读取下一个字符
		ch = prog[p++];
		if (ch == '=') {
			syn = 18;
			token[m++] = ch;
		}
		else {
			syn = 17;
			p--;
		}
		break;
	case':':
		m = 0;
		token[m++] = ch;
		ch = prog[p++];
		if (ch == '=') {
			syn = 10;
			token[m++] = ch;
		}
		else {
			syn = 22;
			p--;
		}
		break;
	case'+':syn = 11; token[0] = ch; break;
	case'-':syn = 28; token[0] = ch; break;
	case'*':syn = 12; token[0] = ch; break;
	case'/':syn = 29; token[0] = ch; break;
	case'(':syn = 13; token[0] = ch; break;
	case')':syn = 14; token[0] = ch; break;
	case'=':syn = 20; token[0] = ch; break;
	case'.':syn = 21; token[0] = ch; break;
	case';':syn = 23; token[0] = ch; break;
	case',':syn = 24; token[0] = ch; break;
	case '\0':syn = 0; break;
	case '{':
		lable = 1;
		do {
			ch = prog[p++];
			time++;
			if (ch == '\0')
			{
				syn = -5;
				break;
			}
			else
				syn = -3;
		} while (ch != '}'&&ch != '\0');
		if (ch == '\0')
			p = p - time;
		break;

	case '}':lable = 0;
		syn = -6; break;
	default:syn = -4; token[0] = ch; break;
	}
}

/*程序分析*/
void program() {
	if (syn == PROGRAM)//program开头
	{
		//扫描下一个单词
		scanner();
		if (syn == ID)//第二字符串为标识符
		{
			scanner();

			if (syn == SEMICOLON)//分号
			{
				scanner();
				procedure();
			}
			else {//第三个字符不是；
				printf("error:缺少分号；\n违背产生式：<程序>->program<标识符>;<分程序>\n\n");
				flag = 1;
			}
		}
		else {//第二个单词不是标识符
			printf("error:第二个单词不是标识符\n\n违背产生式：<程序>->program<标识符>;<分程序>\n\n");
			flag = 1;
		}
	}
	else {//第一个单词不是program
		printf("error:lack program\n\n违背产生式：<程序>->program<标识符>;<分程序>\n\n");
		flag = 1;
	}
}

/*分程序分析*/
void procedure() {

	if (flag == 0) {
		if (syn != BEGIN && syn != VAR)
		{//不是begin 且不是VAR
			printf("error:违背产生式：<分程序>-><变量说明>BEGIN<语句表>END\n\n");
			flag = 1;
		}
		else if (syn == VAR) { //VAR存在
			scanner();
			variable();
		}
		else //syn=1表示的单词符号是begin
			lrparser();//语句分析
	}
}

/*变量说明表*/
void variable() {
	table();
	if (flag == 0) {
		if (syn == COLON) {//冒号
			scanner();
			if (syn == INT) {//整型
				scanner();
				if (syn == SEMICOLON) {
					scanner();
					if (syn == VAR) { //VAR存在
						scanner();
						variable();
					}
					else if (syn == BEGIN) {
						lrparser();
					}
					else {
						printf("1error:违背产生式：<变量说明表>-><变量表>：<类型>|<变量表>：<类型>;<变量说明表>\n\n");
						flag = 1;
					}
				}
				else {
					printf("2error:违背产生式：<变量说明表>-><变量表>：<类型>|<变量表>：<类型>;<变量说明表>\n\n");
					printf("2error:违背产生式：<变量说明>->VAR<变量说明表>;\n");
					flag = 1;
				}
			}
			else {
				printf("3error:违背产生式：<变量说明表>-><变量表>：<类型>|<变量表>：<类型>;<变量说明表>\n\n");
				flag = 1;
			}
		}
		else {
			printf("4error:违背产生式：<变量说明表>-><变量表>：<类型>|<变量表>：<类型>;<变量说明表>\n\n");
			flag = 1;
		}
	}
}

/*变量表分析*/
void table() {
	if (flag == 0) {
		if (syn != COLON && syn != ID) //var后不是接分号，也不是标识符
		{
			printf("error1:违背产生式：<变量表>-><变量>|<变量>,<变量表>\n\n");
			flag = 1;
		}
		if (syn == ID) {//标识符
			scanner();
			while (syn == COMMA) {//读取为逗号
				scanner();
				if (syn == ID)//标识符
					scanner();
				else {//逗号后不是标识符
					printf("error2:逗号后不是标识符 \n 违背产生式：<变量表>-><变量>|<变量>,<变量表>\n\n");
					flag = 1;
				}
			}
		}
	}
}

/*程序分析*/
void lrparser()    //<程序>：：=begin<语句串>end
{
	if (flag == 0) {
		if (syn == BEGIN) //syn=1表示的单词符号是begin
		{
			printf("\nBEGIN\n");           //如果是begin
			scanner();
			yuju();    //进行语句串分析
			if (flag == 0) {
				if (syn == END)//如果是end
				{
					printf("END\n");
					scanner();
					if (syn == POINT) {
						//scanner();
						scanner();
						if (syn == 0 && flag == 0)//如果是'\0',则成功
								printf("\nThis is a right program!\n\n");
						else {
							printf("error3: -->违背产生式：<分程序>-><变量说明>BEGIN<语句表>END.\n\n");
							flag = 1;
						}

					}
					else {
						printf("error2:缺少分隔符.-->违背产生式：<分程序>-><变量说明>BEGIN<语句表>END.\n\n");
						flag = 1;
					}
				}
				else {
					printf("error3:lack END -->违背产生式：<分程序>-><变量说明>BEGIN<语句表>END.\n\n");
					flag = 1;
				}
			}
		}
		else {//单词不是begin
			printf("error4:lack BEGIN--> 违背产生式：<分程序>-><变量说明>BEGIN<语句表>END.\n\n");
			flag = 1;
		}
	}
}

/*语句表分析*/
void yuju()   //<语句串>：：=<语句>{；<语句>}
{
	if (flag == 0) {
		statement();     //分析语句
		while (syn == SEMICOLON)  //如果单词是';',则扫描下一个单词
		{
			scanner();
			statement();
		}
		if (syn != END && flag == 0) {
			printf("error1:缺少分号；-->违背产生式：<语句表>-><语句>|<语句>;<语句表>\n\n");
			flag = 1;
		}
	}
}

/*语句分析*/
void statement()
{
	char *t, *place;//t保存赋值符的左操作数，place保存右操作数

	t = (char*)malloc(12);
	place = (char*)malloc(12);

	if (flag == 0) {
		if (syn == ID)  //赋值语句
		{
			strcpy(t, token);
			scanner();
			if (syn == EQUAL)  //如果标识符后面是赋值号，则扫描下一个单词
			{
				scanner();
				strcpy(place, expression());//算术表达式分析
				emit(":=", place, "_", t);
			}
			else    //如果标识符后面不是赋值号
			{
				printf("赋值语句错误！-->违背产生式：<赋值语句>-><变量>:=<算术表达式>\n\n");
				flag = 1;
			}
		}
		else if (syn == IF) {//条件语句
			printf("IF\n");
			condition();
		}
		else if (syn == WHILE) {//while语句
			printf("WHILE\n");
			whiled();
		}
		else if (syn == BEGIN) {//复合语句
			printf("into compound\n");
			compound();
			scanner();
		}
		else if (syn == END)
		{
			printf("error1:end前不可有分号；-->违背产生式：<语句>-><语句>|<语句>;<语句表>\n\n");
			flag = 1;
		}
		//else if (syn == SEMICOLON);//空语句
		else {   //如果不是标识符
			printf("语句错误！-->违背产生式：<语句>-><赋值语句>|<条件语句>|<WHILE语句>|<复合语句>\n\n");
			flag = 1;
		}
	}
}

/*复合语句分析*/
void compound() {
	if (flag == 0) {
		scanner();
		yuju();
		if (syn != END && flag == 0) {
			printf("复合语句错误！缺少END-->违背产生式：<复合语句>->BEGIN<语句表>END\n\n");
			flag = 1;
		}
	}
}

/*条件语句*/
void condition() {
	char *t;//t操作符，op1操作数1，op2操作数2，tp临时变量
	int FC, temp, temp1;
	t = (char*)malloc(12);
	if (flag == 0) {

		FC = guanxi();//假出口序号

		if (syn == THEN && flag == 0) {
			printf("THEN\n");
			scanner();
			statement();//进行语句分析

			emit("goto", "_", "_", "");	//跳出if语句，跳过else
			temp1 = NXQ - 1;

			if (flag == 0) {
				if (syn == ELSE) {
					printf("ELSE\n");
					scanner();
					temp = NXQ + 1;
					itoa(temp, t, 10);//
					strcpy(four[FC].result, t);

					statement();//进行语句分析

					temp = NXQ;
					itoa(temp, t, 10);
					strcpy(four[temp1].result, t);
				}
				else {
					printf("error:缺少关键字else --> 违背产生式：<条件语句>->IF<关系表达式>THEN<语句>ELSE<语句>\n\n");
					flag = 1;
				}
			}
		}
		else {
			printf("error:缺少关键字then --> 违背产生式：<条件语句>->IF<关系表达式>THEN<语句>ELSE<语句>\n\n");
			flag = 1;
		}
	}
}

/*while语句*/
void whiled() {
	char  *t;
	int FC, temp1, temp2;
	t = (char*)malloc(12);

	if (flag == 0) {

		temp2 = NXQ;//while开始位置

		FC = guanxi();//接收假出口

		if (syn == DO) {
			printf("DO\n");
			scanner();
			statement();//进行语句分析


			itoa(temp2, t, 10);//记录跳转地址
			emit("goto", "_", "_", t);	//假出口

			temp1 = NXQ;
			itoa(temp1, t, 10);//记录跳转位置
			strcpy(four[FC].result, t);

		}
		else {
			printf("缺少关键字DO --> 违背产生式：<WHILE语句>->WHILE<关系表达式>DO<语句>\n\n");
			flag = 1;
		}
	}
}

/*关系表达式*/
int guanxi() {
	char *t, *op1, *op2, *TC;//op1保存关系符的左操作数，op2保存右操作数
	int temp, FC;
	op1 = (char*)malloc(12);
	op2 = (char*)malloc(12);
	t = (char*)malloc(12);
	TC = (char*)malloc(12);
	strcpy(t, "");
	scanner();
	strcpy(op1, expression());//算术表达式
	if (flag == 0) {
		if (syn >= LT && syn <= EQ) {
			if (syn == LT)
				strcpy(t, "<");
			else if (syn == LE) {
				strcpy(t, "<");
				strcat(t, "=");
			}
			else if (syn == GT)
				strcpy(t, ">");
			else if (syn == GE)
			{
				strcpy(t, ">");
				strcat(t, "=");
			}
			else if (syn == NE)
			{
				strcpy(t, "<");
				strcat(t, ">");
			}
			else
				strcpy(t, "=");
			scanner();
			strcpy(op2, expression());//算术表达式

			temp = NXQ + 2;
			itoa(temp, TC, 10);//记录跳转地址
			emit(t, op1, op2, TC);//真出口

			FC = NXQ;
			emit("goto", "_", "_", "");	//假出口
		}
		else {
			printf("违背产生式：<关系表达式>-><算术表达式><关系符><算术表达式>\n\n");
			flag = 1;
		}
	}
	return FC;
}

/*算术表达式分析*/
char* expression()//<表达式>：：=<项>{+<项> | -<项>}
{
	char *tp, *op1, *op2, *t;//t操作符，op1操作数1，op2操作数2，tp临时变量
	t = (char*)malloc(12);
	tp = (char*)malloc(12);
	op1 = (char*)malloc(12);
	op2 = (char*)malloc(12);

	if (flag == 0) {
		//term();//项分析
		strcpy(op1, term());
		while (syn == PLUS || syn == MINUS)//如果是'+'或'-'，则扫描下一个单词
		{
			//scanner();
			if (syn == PLUS)
				*t = '+';
			else
				*t = '-';
			*(t + 1) = '\0';
			scanner();
			strcpy(op2, term());
			//term();//项分析
			strcpy(tp, newtemp());//调用newtemp()产生临时变量tp;
			emit(t, op1, op2, tp);
			strcpy(op1, tp);//将计算机结果作为下一次表达式计算机的第一项

		}
	}
	return op1;
}

/*项分析*/
char* term()//<项>：：=<因子>{*<因子> | /<因子>
{
	char *tp, *op1, *op2, *t;//t操作符，op1操作数1，op2操作数2，tp临时变量
	t = (char*)malloc(12);
	tp = (char*)malloc(12);
	op1 = (char*)malloc(12);
	op2 = (char*)malloc(12);

	if (flag == 0) {
		//factor();//因子分析
		strcpy(op1, factor());
		while (syn == STAR || syn == DIV)//如果是'*'或'/'，则扫描下一个单词
		{
			if (syn == STAR)
				*t = '*';
			else
				*t = '/';
			*(t + 1) = '\0';
			scanner();
			strcpy(op2, factor());//调用factor分析产生运算的第二项ep2;
			strcpy(tp, newtemp());

			emit(t, op1, op2, tp);
			strcpy(op1, tp);//将计算机结果作为下一次表达式计算机的第一项
							//	factor();//因子分析
		}
	}
	return op1;
}

/*因式分析*/
char* factor()//<因子>：：=ID | NUM | （<表达式>）
{
	char* fplace;
	fplace = (char*)malloc(12);
	strcpy(fplace, " ");

	if (syn == ID || syn == CONSTANT)//如果是标识符或数字，则扫描下一个单词
	{

		if (syn == ID)
			strcpy(fplace, token);
		else
			itoa(sum, fplace, 10);
		scanner();
	}
	else if (syn == LP)//如果是左括号
	{
		scanner();
		strcpy(fplace, expression());//调用expression分析返回表达式的值
		if (syn == RP)//如果是右括号
			scanner();
		else {
			printf("缺少“)”:违背产生式：<因式>-><变量>|常数|（<算术表达式>）\n\n");
			flag = 1;
		}
	}
	else {
		printf("违背产生式：<因式>-><变量>|常数|（<算术表达式>）\n\n");
		flag = 1;
	}
	return fplace;
}

void  emit(char* op, char* ag1, char* ag2, char* result) {
	//printf("\n%d:(%s ,%s ,%s ,%s )\n", NXQ++,op, ag1, ag2, result);

	strcpy(four[NXQ].ag1, ag1);
	strcpy(four[NXQ].ag2, ag2);
	strcpy(four[NXQ].op, op);
	strcpy(four[NXQ].result, result);
	NXQ++;
}

char* newtemp() {
	char* p;
	char m[8];
	p = (char*)malloc(8);
	kk++;
	itoa(kk, m, 10);
	strcpy(p + 1, m);
	p[0] = 't';
	return p;
}
