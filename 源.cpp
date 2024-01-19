#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//关键字
string reserved_words[] = { 
	"program","const","var","procedure","begin","end",
	"if","then","else","while","do","call","read","write","odd"
};

//一个词语
typedef struct SYM {
	string word;//词语
	string attribute;//属性
	int row;//所在行
	int column;//所在列
} SYM;

//词语属性
typedef enum {
	id, reservedwords, digits, op, aop, mop, lop, sop
}attribute;

//语法分析----------------------------------------------------------------------------------------------------------

//错误类型
typedef enum {
	id_error, op_error, invalid_char//id不符合要求，运算符不符合要求，非法字符
}lex_error_type;

//是否是字母
bool is_alpha(char a) {
	if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z')) return true;
	else return false;
}

//是否是数字
bool is_digit(char a) {
	if ((a >= '0' && a <= '9')) return true;
	else return false;
}

//是否是关键字
bool is_reserved(string s) {
	for (auto i = 0; i < sizeof(reserved_words); i++) {
		if (s == reserved_words[i]) return true;
	}
	return false;
}

//词法分析出错处理
void lex_error(lex_error_type lex_error_type, int row, int column) {
	if (lex_error_type == id_error) cout << "id_error in row" << row << " column" << column << endl;
	if (lex_error_type == op_error) cout << "op_error in row" << row << " column" << column << endl;
	if (lex_error_type == invalid_char) cout << "invalid_char in row" << row << " column" << column << endl;
	return;
}

//连接操作,字符串尾追加一个字符
void concat(string &s, char a) {
	s += a;
}

//词法分析
const int maxnum_per_row = 200;//每一行上限200个字符
bool lex() {
	int flag = 1;
	fstream lex_fp_in, lex_fp_out;
	lex_fp_in.open("origin_program.txt", ios::in);//读入源文件
	lex_fp_out.open("middle_program.txt", ios::out | ios::trunc);//输出每个词语和属性至中间文件
	if (lex_fp_in.fail() || lex_fp_out.fail()) return 0;
	char arr[maxnum_per_row];
	int row = 1, column = 0;
	while (lex_fp_in.getline(arr, sizeof(arr), '\n')) {
		string temp;
		while (column < sizeof(arr)) {
			if (arr[column] == '\0') {//该行读取完毕
				break;
			}
			if (arr[column] == ' '|| arr[column] == '\t') {//忽略空格和制表符
				column++;
				continue;
			}
			if (is_alpha(arr[column])) {//是字母
				while (is_alpha(arr[column]) || is_digit(arr[column])) {
					concat(temp, arr[column]);
					column++;
				}
				column--;
				if (is_reserved(temp)) lex_fp_out << temp << " reservedwords" << " " << row << " " << column + 1 << endl;//是关键字
				else lex_fp_out << temp << " id" << " " << row << " " << column + 1 << endl;//是id
				temp.erase(temp.begin(), temp.end());
			}
			else if (is_digit(arr[column])) {
				while (is_digit(arr[column])) {
					concat(temp, arr[column]);
					column++;
				}
				if (is_alpha(arr[column])) {
					lex_error(id_error, row, column);//id不符合要求
					flag = 0;
					while (arr[column] != ' ' && arr[column] != '\0') column++;
				}
				column--;
				lex_fp_out << temp << " digits" <<" "<< row <<" "<< column + 1 << endl;
				temp.erase(temp.begin(), temp.end());
			}
			else if (arr[column] == ':') {
				column++;
				if (arr[column] == '=') lex_fp_out << ":=" << " op" <<" "<< row <<" "<< column + 1 << endl;
				else {
					lex_error(op_error, row, column);//运算符不符合要求
					flag = 0;
					while (arr[column] != ' ' && arr[column] != '\0') column++;
				}
			}
			else if (arr[column] == '/') {
				lex_fp_out << '/' << " mop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == '*') {
				lex_fp_out << '*' << " mop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == '+') {
				lex_fp_out << '+' << " aop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == '-') {
				lex_fp_out << '-' << " aop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == '=') {
				lex_fp_out << '=' << " lop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == '<') {
				column++;
				if(arr[column] == '>')	lex_fp_out << "<>" << " lop" <<" "<< row <<" "<< column + 1 << endl;
				else if(arr[column] == '=') lex_fp_out << "<=" << " lop" <<" "<< row <<" "<< column + 1 << endl;
				else {
					column--;
					lex_fp_out << '<' << " lop" <<" "<< row <<" "<< column + 1 << endl;
				}			
			}
			else if (arr[column] == '>') {
				column++;
				if (arr[column] == '=') lex_fp_out << ">=" << " lop" <<" "<< row <<" "<< column + 1 << endl;
				else {
					column--;
					lex_fp_out << '>' << " lop" <<" "<< row <<" "<< column + 1 << endl;
				}
			}
			else if (arr[column] == '(') {
				lex_fp_out << '(' << " sop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == ')') {
				lex_fp_out << ')' << " sop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == ';') {
				lex_fp_out << ';' << " sop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == ',') {
				lex_fp_out << ',' << " sop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else if (arr[column] == '.') {
				lex_fp_out << '.' << " sop" <<" "<< row <<" "<< column + 1 << endl;
			}
			else {//非法字符
				lex_error(invalid_char, row, column);
				flag = 0;
				while (arr[column] != ' ' && arr[column] != '\0') column++;
			}
			column++;
		}
		column = 0;
		row++;
	}
	lex_fp_in.close();
	lex_fp_out.close();
	if (flag == 1) cout << "no lex error"<<endl;
	return flag;
}

//语法分析和语义分析-------------------------------------------------------------------------------------------------

fstream parse_fp_in;//语法分析文件指针
streampos pos;//用于在文件中回退一个单词
int current_row = 1, current_column = 1;//当前词语所在行，列
int pre_row = 1, pre_column = 1;//上一个词语所在行，列

//读取一个单词
SYM getword() {
	SYM sym;
	sym.word.clear();
	sym.attribute.clear();
	string temp_row = "";
	string temp_column = "";
	char temp[200];
	pos = parse_fp_in.tellg();
	if (parse_fp_in.getline(temp, sizeof(temp), '\n')) {
		int i = 0;
		while (temp[i] != ' ') {
			sym.word += temp[i];
			i++;
		}
		i++;
		while (temp[i] != ' ') {
			sym.attribute += temp[i];
			i++;
		}
		i++;
		while (temp[i] != ' ') {
			temp_row += temp[i];
			i++;
		}
		i++;
		while (temp[i] != '\0') {
			temp_column += temp[i];
			i++;
		}
		pre_row = current_row;
		pre_column = current_column;
		current_row = sym.row = stoi(temp_row);
		current_column = sym.column = stoi(temp_column);
		return sym;
	}
	return sym;
}

//回退一个单词
void backword() {
	parse_fp_in.seekg(pos);
	current_row = pre_row;
	current_column = pre_column;
}

//语法分析出错处理
void parse_error(int parse_error_type, SYM temp) {
	switch (parse_error_type) {
	case 0:cout << "excess of \"" << temp.word << "\" in row" << pre_row << " column" << pre_column << endl; break;
	case 1:cout << "lack of \"program\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 2:cout << "lack of \"id\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 3:cout << "lack of \";\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 4:cout << "lack of \"const\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 5:cout << "lack of \",\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 6:cout << "lack of \":=\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 7:cout << "lack of \"integer\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 8:cout << "lack of \"var\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 9:cout << "lack of \"procedure\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 10:cout << "lack of \"(\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 11:cout << "lack of \")\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 12:cout << "lack of \"begin\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 13:cout << "lack of \"end\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 14:cout << "lack of \"then\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 15:cout << "lack of \"else\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 16:cout << "lack of \"do\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 17:cout << "lack of \"lop\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 18:cout << "lack of \"aop\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	case 19:cout << "lack of \"mop\"" << " in row" << pre_row << " column" << pre_column << endl; break;
	}
}

//语法分析同步
bool synchronous(int flag, SYM temp) {
	switch (flag) {
	case 1: if (temp.word != "program" && temp.attribute != "id" && temp.word != ";") return true; break;
	case 2: if (temp.word != ")" && temp.attribute != "id" && temp.word != ";" && temp.word != "const" && temp.word != "var" && temp.word != "procedure" && temp.word != "begin") return true; break;
	case 3: if (temp.word != ";" && temp.word != "const" && temp.word != "var" && temp.word != "procedure" && temp.word != "begin") return true; break;
	case 4: if (temp.attribute != "id" && temp.word != ",") return true; break;
	case 5: if (temp.attribute != "id" && temp.word != ":=" && temp.attribute != "digits") return true; break;
	case 6: if (temp.word != ":=" && temp.attribute != "digits" && temp.word != "," && temp.word != "var" && temp.word != "procedure" && temp.word != "begin" && temp.word != ";") return true; break;
	case 7: if (temp.attribute != "digits" && temp.word != "," && temp.word != "var" && temp.word != "procedure" && temp.word != "begin" && temp.word != ";") return true; break;
	case 8: if (temp.attribute != "id" && temp.word != ",") return true; break;
	case 9: if (temp.attribute != "id" && temp.word != "," && temp.word != ";") return true; break;
	case 10: if (temp.attribute != "id" && temp.word != "(") return true; break;
	case 11: if (temp.attribute != "id" && temp.word != "," && temp.word != "(") return true; break;
	case 12: if (temp.attribute != "id" && temp.word != "(" && temp.word != ")") return true; break;
	case 13: if (temp.word != "do" && temp.word != "then" && temp.word != "end" && temp.attribute != "id" && temp.word != "if" && temp.word != "while" && temp.word != "call" && temp.word != "begin" && temp.word != "read" && temp.word != "write") return true; break;
	case 14: if (temp.attribute != "lop" && temp.word != ":=" && temp.attribute != "aop" && temp.word != "(" && temp.attribute != "id" && temp.attribute != "digits") return true; break;
	case 15: if (temp.word != ")" && temp.attribute != "aop" && temp.word != "(" && temp.attribute != "id" && temp.attribute != "digits") return true; break;
	case 16: if (temp.word != ")" && temp.attribute != "aop" && temp.word != "(" && temp.attribute != "id" && temp.attribute != "digits") return true; break;
	}
	return false;
}

//对象种类
typedef enum {
	constant,variable,procedure
}id_type;

//符号表中一条记录
typedef struct record {
	string name;//对象名
	id_type type;//对象种类：常变量，变量，过程名
	int level;//变量,常变量的所在层次，其他类型为-1
	int value;//常变量的值，过程名的初始指令位置，其他类型为-1
	int address;//变量和过程名的相对偏移量，其他类型为-1
	int size;//过程名的所需空间，其他类型为-1
}record;

//一条指令
typedef struct instruction {
	string f;
	int l;
	int a;
}instruction;

record table[1000];//符号表
int tx = 0;//符号表指针
instruction code[1000];//中间代码储存
int cx = 0;//中间代码储存器指针

//栈的结构：动态链，返回地址，静态链，形式参数，变量（增长方向为->）

//中间代码生成
void gen(string f, int l, int a) {
	code[cx].f = f;
	code[cx].l = l;
	code[cx].a = a;
	cx++;
}

//写入符号表
void enter(string name, id_type type, int level, int value, int address, int size) {
	table[tx].name = name;
	table[tx].type = type;
	table[tx].level = level;
	table[tx].value = value;
	table[tx].address = address;
	table[tx].size = size;
	tx++;
}

//在符号表中搜索对象，返回其指针
int find_name_index(string name) {
	for (int i = tx-1; i >=0; i--) {//从栈顶向栈底搜索，重定义时可以优先访问到最新定义
		if (table[i].name == name) return i;
	}
	return -1;
}

//函数声明
bool exp(int level);
bool body(int level);
bool block(int proc_name_index, int level);

//语义分析错误类型
typedef enum {
	use_before_definition,//未定义就使用
	redefinition,//重定义
	assignment_error,//试图对非变量赋值
	par_number_error,//参数个数错误
	proc_in_exp//过程名出现在表达式中
}meaning_error_type;

//语义分析出错处理
void meaning_error(meaning_error_type meaning_error_type, SYM temp) {
	if (meaning_error_type == use_before_definition) {
		cout <<"\"" << temp.word << "\" use before definition" << " in row " <<  pre_row << " column" << pre_column << endl;
		exit(-1);
	}
	else if (meaning_error_type == redefinition) {
		cout << "\"" << temp.word << "\" is redefinition" << " in row " << temp.row << " column" << temp.column << endl;
		exit(-1);
	}
	else if (meaning_error_type == assignment_error) {
		cout << "\"" << temp.word << "\" can't be assigned" << " in row " << temp.row << " column" << temp.column << endl;
		exit(-1);
	}
	else if (meaning_error_type == par_number_error) {
		cout << "\"" << temp.word << "\" has a mismatched number of parameters" << " in row " << temp.row << " column" << temp.column << endl;
		exit(-1);
	}
	else if (meaning_error_type == proc_in_exp) {
		cout << "\"" << temp.word << "\" can't be in exp" << " in row " << temp.row << " column" << temp.column << endl;
		exit(-1);
	}
}

//<factor> → <id>|<integer>|(<exp>)
bool factor(int level) {
	bool flag = 1;//标记当前语句语法分析是否出错
	SYM temp = getword();
	if (temp.attribute == "id") {
		int val_name_index = find_name_index(temp.word);//符号表搜索该变量，返回其在符号表中的位置
		if (val_name_index == -1) {//未定义就使用
			meaning_error(use_before_definition, temp); 
		}
		else {
			if (table[val_name_index].type == constant) {//常变量
				gen("lit", 0, table[val_name_index].value);//将常变量替换为立即数
			}
			else if (table[val_name_index].type == variable) {//变量
				int val_level = table[val_name_index].level;//该变量的层次
				int val_dx = table[val_name_index].address;//该变量的偏移量
				gen("lod", level - val_level, val_dx);//取变量值传至栈顶
			}
			else {//过程名
				meaning_error(proc_in_exp, temp);
			}
		}
	}
	else if (temp.attribute == "digits") {//立即数
		gen("lit", 0, stoi(temp.word));//取立即数至栈顶
	}
	else if (temp.word != "(") {
		while (synchronous(14, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != "(") {
			parse_error(10, temp);
			backword();
		}
		if (!exp(level)) flag = 0;
		temp = getword();
		if (temp.word != ")") {
			while (synchronous(12, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != ")") {
				parse_error(11, temp);
				backword();
			}
		}
	}
	return flag;
}

//<term> → <factor>{<mop><factor>}
bool terms(int level) {
	bool flag = 1;
	if (!factor(level)) flag = 0;
	SYM temp = getword();
	while (temp.attribute == "mop" || (temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
		while (temp.attribute == "mop") {
			int mop_type;//记录mop类型（* or /)
			if (temp.word == "*") mop_type = 4;
			else if (temp.word == "/") mop_type = 5;
			if (!factor(level)) flag = 0;
			gen("opr", 0, mop_type);
			temp = getword();
		}
		backword();
		temp = getword();
		if ((temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
			flag = 0;
			parse_error(19, temp);
			backword();
			if (!factor(level)) flag = 0;
			temp = getword();
		}		
	}
	backword();
	return flag;
}

//<exp> → [+|-]<term>{<aop><term>}
bool exp(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.attribute != "aop") backword();
	if (temp.word == "-") gen("opr", 0, 1);//取相反数
	if (!terms(level)) flag = 0;
	temp = getword();
	while (temp.attribute == "aop" || (temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
		while (temp.attribute == "aop") {
			int aop_type ;//记录aop类型（+ or -)
			if (temp.word == "+") aop_type = 2;
			else if (temp.word == "-") aop_type = 3;
			if (!terms(level)) flag = 0;
			gen("opr", 0, aop_type);
			temp = getword();
		}
		backword();
		temp = getword();
		if ((temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
			flag = 0;
			parse_error(18, temp);
			backword();
			if (!terms(level)) flag = 0;
			temp = getword();
		}	
	}
	backword();
	return flag;
}

//<lexp> → <exp> <lop> <exp>| odd <exp>
bool lexp(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.word == "odd") {
		if(!exp(level)) flag=0;
		gen("opr", 0, 6);//判断奇偶性
	}
	else {
		backword();
		if (!exp(level)) flag = 0;
		temp = getword();
		if (temp.attribute != "lop") {
			flag = 0;
			while (synchronous(14, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.attribute != "lop") {
				parse_error(17, temp);
				backword();
			}
		}
		int lop_type;//记录lop类型
		if (temp.word == "=") lop_type = 7;//栈顶与次栈顶是否相等
		else if (temp.word == "<>") lop_type = 8;//栈顶与次栈顶是否不等
		else if (temp.word == "<") lop_type = 9;//次栈顶是否小于栈顶
		else if (temp.word == ">=") lop_type = 10;//次栈顶是否大于等于栈顶
		else if (temp.word == ">") lop_type = 11;//次栈顶是否大于栈顶
		else if (temp.word == "<=") lop_type = 12;//次栈顶是否小于等于栈顶
		if (!exp(level)) flag = 0;
		gen("opr", 0, lop_type);
	}
	return flag;
}

//<statement> → <id> : = <exp>
//			| if <lexp> then <statement>[else <statement>]
//			| while <lexp> do <statement>
//			| call <id>（[<exp>{, <exp>}]）
//			| <body>
//			| read(<id>{，<id>})
//			| write(<exp>{, <exp>})
bool statement(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.attribute == "id") {
		int val_name_index = find_name_index(temp.word);//符号表搜索该变量，返回其在符号表中的位置
		int val_level;//该变量的层次
		int val_dx;//该变量的偏移量
		if (val_name_index == -1) {//未定义就使用
			meaning_error(use_before_definition, temp);
		}
		else if (table[val_name_index].type != variable) {
			meaning_error(assignment_error, temp);
		}
		else {
			val_level = table[val_name_index].level;
			val_dx = table[val_name_index].address;
		}
		temp = getword();
		if (temp.word != ":=") {
			flag = 0;
			while (synchronous(14, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != ":=") {
				parse_error(6, temp);
				backword();
			}
		}
		if (!exp(level)) flag = 0;
		gen("sto", level - val_level, val_dx);//取栈顶元素赋给该变量
	}
	else if (temp.word == "if") {
		if (!lexp(level)) flag = 0;
		int cx1 = cx;//为该jpc语句附上标签便于回填
		gen("jpc", 0, 0);//跳转至假出口，等待回填
		temp = getword();
		if (temp.word != "then") {
			flag = 0;
			while (synchronous(13, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != "then") {
				parse_error(14, temp);
				backword();
			}
		}
		if (!statement(level)) flag = 0;
		code[cx1].a = cx;//回填假出口
		temp = getword();
		if (temp.word == "else" || temp.attribute == "id" || temp.word == "if" || temp.word == "while" || temp.word == "call" || temp.word == "begin" || temp.word == "read" || temp.word == "write") {
			int cx2 = cx;//此时存在else，为该jmp语句附上标签便于回填
			gen("jmp", 0, 0);//跳转至真出口，等待回填
			code[cx1].a = cx;//假出口需要更新，即jmp指令的后一条指令
			if (temp.word != "else") {
				flag = 0;
				parse_error(15, temp);
				backword();
			}
			if (!statement(level)) flag = 0;
			code[cx2].a = cx;//回填真出口
		}
		else backword();
	}
	else if (temp.word == "while") {
		int cx1 = cx;//该while的初始位置，便于回填
		if (!lexp(level)) flag = 0;
		int cx2 = cx;//为该jpc语句附上标签便于回填
		gen("jpc", 0, 0);//跳转至假出口，等待回填
		temp = getword();
		if (temp.word != "do") {
			flag = 0;
			while (synchronous(13, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != "do") {
				parse_error(16, temp);
				backword();
			}
		}
		if (!statement(level)) flag = 0;
		gen("jmp", 0, cx1);//跳转至while初始位置
		code[cx2].a = cx;//回填假出口
	}
	else if (temp.word == "call") {
		temp = getword();
		if (temp.attribute != "id") {
			flag = 0;
			while (synchronous(15, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.attribute != "id") {
				parse_error(2, temp);
				backword();
			}
		}
		int proc_name_index = find_name_index(temp.word);//符号表搜索该过程名，返回其在符号表中的位置
		if (proc_name_index == -1) {//未定义就使用
			meaning_error(use_before_definition, temp);
		}
		SYM temp_proc = temp;
		temp = getword();
		if (temp.word != "(") {
			flag = 0;
			while (synchronous(16, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != "(") {
				parse_error(10, temp);
				backword();
			}
		}
		int num_actualPar = 0;//实际参数个数
		temp = getword();
		if (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits") {
			int dx = 3;//形式参数的起始偏移量
			backword();
			if (!exp(level)) flag = 0;
			num_actualPar++;
			gen("sto", -1, dx);//参数传递
			dx++;//偏移量++
			temp = getword();
			while (temp.word == "," || (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
				while (temp.word == ",") {
					if (!exp(level)) flag = 0;
					num_actualPar++;
					gen("sto", -1, dx);
					dx++;//偏移量++
					temp = getword();
				}
				backword();
				temp = getword();
				if (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits") {
					flag = 0;
					parse_error(5, temp);
					backword();
					if (!exp(level)) flag = 0;
					temp = getword();
				}
			}
			backword();
			temp = getword();
		}
		if (temp.word != ")") {
			flag = 0;
			while (synchronous(16, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != ")") {
				parse_error(11, temp);
				backword();
			}
		}
		if (num_actualPar != table[proc_name_index].address) {
			meaning_error(par_number_error, temp_proc);
		}
		gen("cal", level- table[proc_name_index].level, table[proc_name_index].value);//l域用于连接静态链，a域为后续指令的指针
	}
	else if (temp.word == "begin") {
		backword();
		if (!body(level)) flag = 0;;
	}
	else if (temp.word == "read") {
		temp = getword();
		if (temp.word != "(") {
			flag = 0;
			while (synchronous(12, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != "(") {
				parse_error(10, temp);
				backword();
			}
		}
		temp = getword();
		if (temp.attribute != "id") {
			while (synchronous(8, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.attribute != "id") {
				parse_error(6, temp);
				backword();
			}
		}
		int val_name_index = find_name_index(temp.word);//符号表搜索该变量，返回其在符号表中的位置
		int val_level;//该变量层次初始化
		int val_dx;//该变量偏移量初始化
		if (val_name_index == -1) {//未定义就使用
			meaning_error(use_before_definition, temp);
		}
		else if (table[val_name_index].type != variable) {//只能写入变量
			meaning_error(assignment_error, temp);
		}
		else {
			val_level = table[val_name_index].level;
			val_dx = table[val_name_index].address;
		}
		gen("red", 0, 0);//从命令行读取一个输入至于栈顶
		gen("sto", level - val_level, val_dx);//栈顶传至该变量
		temp = getword();
		while (temp.word == "," || temp.attribute == "id") {
			while (temp.word == ",") {
				temp = getword();
				if (temp.attribute != "id") {
					parse_error(0, temp);
					flag = 0;
					backword();
				}
				val_name_index = find_name_index(temp.word);//符号表搜索该变量，返回其在符号表中的位置
				if (val_name_index == -1) {//未定义就使用
					meaning_error(use_before_definition, temp);
				}
				else {
					val_level = table[val_name_index].level;
					val_dx = table[val_name_index].address;
				}
				gen("red", 0, 0);
				gen("sto", level - val_level, val_dx);
				temp = getword();
			}
			backword();
			temp = getword();
			if (temp.attribute == "id") {
				flag = 0;
				parse_error(5, temp);
				temp = getword();				
			}		
		}
		backword();
		temp = getword();
		if (temp.word != ")") {
			flag = 0;
			while (synchronous(12, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != ")") {
				parse_error(11, temp);
				backword();
			}
		}
	}
	else if (temp.word == "write") {
		temp = getword();
		if (temp.word != "(") {
			flag = 0;
			while (synchronous(12, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != "(") {
				parse_error(10, temp);
				backword();
			}
		}
		if (!exp(level)) flag = 0;
		gen("wrt", 0, 0);//输出栈顶至屏幕
		gen("opr", 0, 13);//输出换行符
		temp = getword();
		while (temp.word == "," || (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
			while (temp.word == ",") {
				if (!exp(level)) flag = 0;
				gen("wrt", 0, 0);//输出栈顶至屏幕
				gen("opr", 0, 13);//输出换行符
				temp = getword();
			}
			backword();
			temp = getword();
			if (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits") {
				flag = 0;
				parse_error(5, temp);		
				backword();
				if (!exp(level)) flag = 0;
				temp = getword();
			}
		}
		backword();
		temp = getword();
		if (temp.word != ")") {
			flag = 0;
			while (synchronous(12, temp)) {
				parse_error(0, temp);
				temp = getword();
			}
			if (temp.word != ")") {
				parse_error(11, temp);
				backword();
			}
		}
	}
	return flag;
}

//<body> → begin <statement>{;<statement>} end
bool body(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.word != "begin") {
		flag = 0;
		while (synchronous(13, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != "begin") {
			parse_error(12, temp);
			backword();
		}
	}
	if (!statement(level)) flag = 0;
	temp = getword();
	while (temp.word == ";" || (temp.attribute == "id" || temp.word == "if" || temp.word == "while" || temp.word == "call" || temp.word == "read" || temp.word == "write")) {
		while (temp.word == ";") {
			if (!statement(level)) flag = 0;
			temp = getword();
		}
		backword();
		temp = getword();
		if (temp.attribute == "id" || temp.word == "if" || temp.word == "while" || temp.word == "call" || temp.word == "read" || temp.word == "write") {
			flag = 0;
			parse_error(3, temp);
			backword();
			if (!statement(level)) flag = 0;
			temp = getword();
		}		
	}
	backword();
	temp = getword();
	if (temp.word != "end") {
		flag = 0;
		while (synchronous(13, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != "end") {
			parse_error(13, temp);
			backword();
		}
	}
	return flag;
}

//<proc> → procedure <id>（[<id>{,<id>}]）;<block>{;<proc>}
bool proc(int level) {
	int num_formalPar = 0;//形参个数
	bool flag = 1;
	SYM temp = getword();
	if (temp.word != "procedure") {
		flag = 0;
		parse_error(9, temp);
		while (synchronous(10, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		backword();
	}
	temp = getword();
	if (temp.attribute != "id") {
		flag = 0;
		while (synchronous(11, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.attribute != "id") {
			parse_error(2, temp);
			backword();
		}
	}
	int proc_name_index = tx;//该过程名在符号表中的指针
	if (find_name_index(temp.word) != -1) {//重定义错误
		meaning_error(redefinition, temp);
	}
	else {
		enter(temp.word, procedure, level - 1, -1, 0, 3);//过程名写入符号表，过程名属于上一层
	}
	int orginal_tx = tx;//当前tx值，过程结束后退栈至此，即只保留过程名在符号表中
	temp = getword();
	if (temp.word != "(") {
		flag = 0;
		while (synchronous(12, temp)) {
				parse_error(0, temp);
				temp = getword();
		}
		if (temp.word != "(") {
			parse_error(10, temp);
			backword();
		}
	}
	temp = getword();
	if (temp.attribute == "id") {
		num_formalPar++; //当前总空间为3+num_formalPar，偏移量为3 + num_formalPar - 1，其余同理
		int var_name_index = find_name_index(temp.word);
		if (var_name_index != -1 && table[var_name_index].type != variable) {//符号表里存在该名称的常量或过程名
			meaning_error(redefinition, temp);
		}
		else {
			enter(temp.word, variable, level, -1, 3 + num_formalPar - 1, -1);//形式参数写入符号表
		}
		temp = getword();
		while (temp.word == "," || temp.attribute == "id") {
			while (temp.word == ",") {
				temp = getword();
				if (temp.attribute != "id") {
					parse_error(0, temp);
					flag = 0;
					backword();
				}
				num_formalPar++;
				int var_name_index = find_name_index(temp.word);
				if (var_name_index != -1 && table[var_name_index].type != variable) {//符号表里存在该名称的常量或过程名
					meaning_error(redefinition, temp);
				}
				else {
					enter(temp.word, variable, level, -1, 3 + num_formalPar - 1, -1);//形式参数写入符号表
				}				
				temp = getword();
			}
			backword();
			temp = getword();
			if (temp.attribute == "id") {
				flag = 0;
				parse_error(5, temp);
				temp = getword();
			}			
		}
		backword();
		temp = getword();
	}
	if (temp.word != ")") {
		flag = 0;
		while (synchronous(2, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != ")") {
			parse_error(11, temp);
			backword();
		}
	}
	temp = getword();
	if (temp.word != ";") {
		flag = 0;
		while (synchronous(3, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != ";") {
			parse_error(3, temp);
			backword();
		}
	}
	table[proc_name_index].address = num_formalPar;//adrress域记录形参数量，检测参数个数不匹配的错误
	table[proc_name_index].size += num_formalPar;//更新该过程的size域，目前为3+形参数量
	if (!block(proc_name_index, level)) flag = 0;
	tx = orginal_tx;//恢复符号表，即符号表弹栈，只保留该过程的过程名
	temp = getword();
	while (temp.word == ";" || temp.word == "procedure") {
		while (temp.word == ";") {
			if (!proc(level)) flag = 0;//同层procedure
			temp = getword();
		}
		backword();
		temp = getword();
		if (temp.word == "procedure") {
			flag = 0;
			parse_error(3, temp);
			backword();
			if (!proc(level)) flag = 0;
			temp = getword();
		}
	}
	backword();
	return flag;
}

//<vardecl> → var <id>{,<id>};
bool vardecl(int proc_name_index, int level) {
	int dx = table[proc_name_index].size;//该过程当前所需空间（3+形参个数），等于即将写入的变量的偏移量
	bool flag = 1;
	SYM temp = getword();
	if (temp.word != "var") {
		flag = 0;
		parse_error(8, temp);
		while (synchronous(8, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		backword();
	}
	temp = getword();
	if (temp.attribute != "id") {
		flag = 0;
		while (synchronous(9, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.attribute != "id") {
			parse_error(2, temp);
			backword();
		}
	}
	int var_name_index = find_name_index(temp.word);
	if (var_name_index != -1 && table[var_name_index].type != variable) {//符号表里存在该名称的常量或过程名
		meaning_error(redefinition, temp);
	}
	else {
		enter(temp.word, variable, level, -1, dx, -1); //变量写入符号表，
	}
	dx++;//偏移量++
	temp = getword();
	while (temp.word == "," || temp.attribute == "id") {
		while (temp.word == ",") {
			temp = getword();
			if (temp.attribute != "id") {
				parse_error(0, temp);
				flag = 0;
				backword();
			}
			if (var_name_index != -1 && table[var_name_index].type != variable ) {//符号表里存在该名称的常量或过程名
				meaning_error(redefinition, temp);
			}
			else {
				enter(temp.word, variable, level, -1, dx, -1); //变量写入符号表，
			}
			dx++;//偏移量++
			temp = getword();
		}
		backword();
		temp = getword();
		if (temp.attribute == "id") {//此时语法分析已经出错，故不必再进行语义分析，即变量没有写入符号表
			flag = 0;
			parse_error(5, temp);
			temp = getword();
		}		
	}
	backword();
	temp = getword();
	if (temp.word != ";") {
		while (synchronous(3, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != ";") {
			parse_error(3, temp);
			backword();
		}
	}
	table[proc_name_index].size = dx;//更新当前过程的所需空间（3+形参个数+变量个数）
	return flag;
}

//<const> → <id>: = <integer>
bool func_const(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.attribute != "id") {
		flag = 0;
		while (synchronous(5, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.attribute != "id") {
			parse_error(2, temp);
			backword();
		}
	}
	SYM temp_const = temp;
	temp = getword();
	if (temp.word != ":=") {
		flag = 0;
		while (synchronous(6, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != ":=") {
			parse_error(6, temp);
			backword();
		}
	}
	temp = getword();
	if (temp.attribute != "digits") {
		flag = 0;	
		while (synchronous(7, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.attribute != "digits") {
			parse_error(7, temp);
			backword();
		}
	}
	int constpar_value = stoi(temp.word);//常变量值
	int constpar_name_index = find_name_index(temp_const.word);
	if (constpar_name_index != -1 && table[constpar_name_index].type != constant ) {//符号表里存在该名称的变量或过程名
		meaning_error(redefinition, temp_const);
	}
	else {
		enter(temp_const.word, constant, level, constpar_value, -1, -1);//常变量写入符号表
	}
	return flag;
}

//<condecl> → const <const>{,<const>};
bool condecl(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.word != "const") {
		flag = 0;
		parse_error(4, temp);
		while (synchronous(4, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		backword();
	}
	if (!func_const(level)) flag = 0;
	temp = getword();
	while (temp.word == "," || temp.attribute == "id") {//进入闭包
		while (temp.word == ",") {
			if (!func_const(level)) flag = 0;
			temp = getword();
		}
		backword();
		temp = getword();
		if (temp.attribute == "id") {//缺少","，处理后继续分析
			flag = 0;
			parse_error(5, temp);
			backword();
			if (!func_const(level)) flag = 0;
			temp = getword();
		}	
	}
	backword();
	temp = getword();
	if (temp.word != ";") {
		flag = 0;
		while (synchronous(3, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != ";") {
			parse_error(3, temp);
			backword();
		}
	}
	return flag;
}

//<block> → [<condecl>][<vardecl>][<proc>]<body>
bool block(int proc_name_index, int level) {//proc_name_index是对应的过程名在符号表中的指针
	int orginal_cx = cx;//当前cx值，即该jmp的label,用于回填该jmp
	gen("jmp", 0, 0);//等待回填
	bool flag = 1;
	SYM temp = getword();
	if (temp.word == "const") {
		backword();
		if (!condecl(level)) flag = 0;
		temp = getword();
	}
	if (temp.word == "var") {
		backword();
		if (!vardecl(proc_name_index, level)) flag = 0;
		temp = getword();
	}
	if (temp.word == "procedure") {
		backword();
		if (!proc(level+1)) flag = 0;//嵌套定义
		temp = getword();
	}
	backword();
	code[orginal_cx].a = cx;//回填jmp指令，跳转到当前的指令执行，即该block的body部分
	table[proc_name_index].value = orginal_cx;//当前过程的第一条指令，用于cal指令的a域
	gen("int", 0, table[proc_name_index].size);//为该过程分配空间
	if (!body(level)) flag = 0;
	gen("opr", 0, 0);//返回至调用指令处并退栈
	return flag;
}

//<prog> → program <id>;<block>
bool prog() {//返回值即flag代表该语句是否有语法错误，其余同理
	bool flag = 1;
	SYM temp = getword();
	if (temp.word != "program") {
		flag = 0;
		while (synchronous(1, temp)) {//同步
			parse_error(0,temp);//不是"program",认为是多余的符号
			temp = getword();
		}
		if (temp.word != "program") {
			parse_error(1, temp);//同步至后继符集仍然无"program",即缺少"program"
			backword();
		}
	}
	temp=getword();
	if (temp.attribute != "id") {
		flag = 0;
		while (synchronous(2, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.attribute != "id") {
			parse_error(2, temp);
			backword();
		}
	}
	if (find_name_index(temp.word) != -1) {//重定义错误
		meaning_error(redefinition, temp);
	}
	else{
		enter(temp.word, procedure, 0, 0, 0, 3);//主程序名写入符号表
	}
	temp = getword();
	if (temp.word != ";") {
		flag = 0;
		while (synchronous(3, temp)) {
			parse_error(0, temp);
			temp = getword();
		}
		if (temp.word != ";") {
			parse_error(3, temp);
			backword();
		}
	}
	if (!block(0, 0)) flag = 0;//0，0分别是主程序名在符号表中的指针是0，层次为0
	return flag;
}

bool parse() {
	parse_fp_in.open("middle_program.txt", ios::in);//从中间文件读取每个词语
	if (parse_fp_in.fail()) return 0;
	if (prog()) {//语法分析无错误
		cout << "no parse error"<<endl;
		return 1;
		parse_fp_in.close();
	}
	else {
		return 0;
		parse_fp_in.close();
	}
}

//解释器-----------------------------------------------------------------------------------------------------------
int STACK[10000];//运行栈

//静态链，通过层差和偏移量确定变量位置
int get_position(int B, int l, int a) {
	while (l > 0) {
		B = STACK[B+2];//B+2为静态链，B为基地址（动态链）
		l--;
	}
	return B + a;
}

//解释器实现
void interpreter() {
	int B = 0;//当前过程的基地址
	int P = 0;//下一条等待执行的指令的指针
	instruction I;//当前执行的指令
	int T = 0;//指栈STACK的栈顶
	do {
		I = code[P];
		P++;
		if (I.f == "lit") {//取立即数到栈顶
			STACK[T] = I.a;
			T++;
			
		}
		else if (I.f == "opr") {
			switch (I.a) {
			case 0: {
				P = STACK[B + 1];//取返回地址为下一条待执行的指令
				T = B;//弹栈
				B = STACK[B];//更新当前过程的基地址
				break;
			}
			case 1: {//~
				STACK[T - 1] = - STACK[T - 1];
				break;
			}
			case 2: {//+
				STACK[T - 2] = STACK[T - 2] + STACK[T - 1];
				T--;
				break;
			}
			case 3: {//-
				STACK[T - 2] = STACK[T - 2] - STACK[T - 1];
				T--;
				break;
			}
			case 4: {//*
				STACK[T - 2] = STACK[T - 2] + STACK[T - 1];
				T--;
				break;
			}
			case 5: {///
				STACK[T - 2] = STACK[T - 2] + STACK[T - 1];
				T--;
				break;
			}
			case 6: {//odd
				STACK[T - 1] = STACK[T - 1] % 2;
				T--;
				break;
			}
			case 7: {//=
				STACK[T - 2] = (STACK[T - 2] == STACK[T - 1] ? 1 : 0);
				T--;
				break;
			}
			case 8: {//<>
				STACK[T - 2] = (STACK[T - 2] != STACK[T - 1] ? 1 : 0);
				T--;
				break;
			}
			case 9: {//<
				STACK[T - 2] = (STACK[T - 2] < STACK[T - 1] ? 1 : 0);
				T--;
				break;
			}
			case 10: {//>=
				STACK[T - 2] = (STACK[T - 2] >= STACK[T - 1] ? 1 : 0);
				T--;
				break;
			}
			case 11: {//>
				STACK[T - 2] = (STACK[T - 2] > STACK[T - 1] ? 1 : 0);
				T--;
				break;
			}
			case 12: {//<=
				STACK[T - 2] = (STACK[T - 2] <= STACK[T - 1] ? 1 : 0);
				T--;
				break;
			}
			case 13: {//输出换行符
				cout << endl;
			}	  
			}
		}
		else if (I.f == "lod") {//变量值传至栈顶
			STACK[T] = STACK[get_position(B, I.l, I.a)];
			T++;
		}
		else if (I.f == "sto") {//栈顶值传至变量
			if (I.l == -1) {//sto的l域为-1时表示参数传递
				STACK[T - 1 + I.a] = STACK[T - 1];
			}
			else {//栈顶值传至变量
				STACK[get_position(B, I.l, I.a)] = STACK[T - 1];
			}
			T--;
		}
		else if (I.f == "cal") {//过程调用
			STACK[T+2] = get_position(B, I.l, 0);//连接静态链
			STACK[T] = B;//连接动态链
			B = T;//更新当前过程的基地址
			STACK[T + 1] = P;//返回地址为调用结束后待执行的指令的指针
			P = I.a;//跳转至新过程的第一条指令
		}
		else if (I.f == "int") {//开辟空间
			T += I.a;
		}
		else if (I.f=="jmp") {//无条件跳转
			P = I.a;
		}
		else if (I.f == "jpc") {//有条件跳转
			if(STACK[T-1]==0)	P = I.a;
			T--;
		}
		else if (I.f == "red") {//从命令行读数据至栈顶
			cin >> STACK[T];
			T++;
		}
		else if (I.f == "wrt") {//从栈顶写数据至命令行
			cout << STACK[T - 1];
		}
	}while (P != 0);//指令解释完毕
}

int main() {
	if (lex()) {
		if (parse()) {//无语法错误时才进行中间代码的解释执行
			cout << "no meaning error" << endl;
			//for (int i = 0; i < cx; i++) {//输出中间代码
			//	cout << i << " " << code[i].f << " " << code[i].l << " " << code[i].a << endl;
			//}
			interpreter();//中间代码的解释执行
		}
	}
}
	