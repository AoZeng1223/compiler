#include <iostream>
#include <fstream>
#include <string>
using namespace std;

//�ؼ���
string reserved_words[] = { 
	"program","const","var","procedure","begin","end",
	"if","then","else","while","do","call","read","write","odd"
};

//һ������
typedef struct SYM {
	string word;//����
	string attribute;//����
	int row;//������
	int column;//������
} SYM;

//��������
typedef enum {
	id, reservedwords, digits, op, aop, mop, lop, sop
}attribute;

//�﷨����----------------------------------------------------------------------------------------------------------

//��������
typedef enum {
	id_error, op_error, invalid_char//id������Ҫ�������������Ҫ�󣬷Ƿ��ַ�
}lex_error_type;

//�Ƿ�����ĸ
bool is_alpha(char a) {
	if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z')) return true;
	else return false;
}

//�Ƿ�������
bool is_digit(char a) {
	if ((a >= '0' && a <= '9')) return true;
	else return false;
}

//�Ƿ��ǹؼ���
bool is_reserved(string s) {
	for (auto i = 0; i < sizeof(reserved_words); i++) {
		if (s == reserved_words[i]) return true;
	}
	return false;
}

//�ʷ�����������
void lex_error(lex_error_type lex_error_type, int row, int column) {
	if (lex_error_type == id_error) cout << "id_error in row" << row << " column" << column << endl;
	if (lex_error_type == op_error) cout << "op_error in row" << row << " column" << column << endl;
	if (lex_error_type == invalid_char) cout << "invalid_char in row" << row << " column" << column << endl;
	return;
}

//���Ӳ���,�ַ���β׷��һ���ַ�
void concat(string &s, char a) {
	s += a;
}

//�ʷ�����
const int maxnum_per_row = 200;//ÿһ������200���ַ�
bool lex() {
	int flag = 1;
	fstream lex_fp_in, lex_fp_out;
	lex_fp_in.open("origin_program.txt", ios::in);//����Դ�ļ�
	lex_fp_out.open("middle_program.txt", ios::out | ios::trunc);//���ÿ��������������м��ļ�
	if (lex_fp_in.fail() || lex_fp_out.fail()) return 0;
	char arr[maxnum_per_row];
	int row = 1, column = 0;
	while (lex_fp_in.getline(arr, sizeof(arr), '\n')) {
		string temp;
		while (column < sizeof(arr)) {
			if (arr[column] == '\0') {//���ж�ȡ���
				break;
			}
			if (arr[column] == ' '|| arr[column] == '\t') {//���Կո���Ʊ��
				column++;
				continue;
			}
			if (is_alpha(arr[column])) {//����ĸ
				while (is_alpha(arr[column]) || is_digit(arr[column])) {
					concat(temp, arr[column]);
					column++;
				}
				column--;
				if (is_reserved(temp)) lex_fp_out << temp << " reservedwords" << " " << row << " " << column + 1 << endl;//�ǹؼ���
				else lex_fp_out << temp << " id" << " " << row << " " << column + 1 << endl;//��id
				temp.erase(temp.begin(), temp.end());
			}
			else if (is_digit(arr[column])) {
				while (is_digit(arr[column])) {
					concat(temp, arr[column]);
					column++;
				}
				if (is_alpha(arr[column])) {
					lex_error(id_error, row, column);//id������Ҫ��
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
					lex_error(op_error, row, column);//�����������Ҫ��
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
			else {//�Ƿ��ַ�
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

//�﷨�������������-------------------------------------------------------------------------------------------------

fstream parse_fp_in;//�﷨�����ļ�ָ��
streampos pos;//�������ļ��л���һ������
int current_row = 1, current_column = 1;//��ǰ���������У���
int pre_row = 1, pre_column = 1;//��һ�����������У���

//��ȡһ������
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

//����һ������
void backword() {
	parse_fp_in.seekg(pos);
	current_row = pre_row;
	current_column = pre_column;
}

//�﷨����������
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

//�﷨����ͬ��
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

//��������
typedef enum {
	constant,variable,procedure
}id_type;

//���ű���һ����¼
typedef struct record {
	string name;//������
	id_type type;//�������ࣺ��������������������
	int level;//����,�����������ڲ�Σ���������Ϊ-1
	int value;//��������ֵ���������ĳ�ʼָ��λ�ã���������Ϊ-1
	int address;//�����͹����������ƫ��������������Ϊ-1
	int size;//������������ռ䣬��������Ϊ-1
}record;

//һ��ָ��
typedef struct instruction {
	string f;
	int l;
	int a;
}instruction;

record table[1000];//���ű�
int tx = 0;//���ű�ָ��
instruction code[1000];//�м���봢��
int cx = 0;//�м���봢����ָ��

//ջ�Ľṹ����̬�������ص�ַ����̬������ʽ��������������������Ϊ->��

//�м��������
void gen(string f, int l, int a) {
	code[cx].f = f;
	code[cx].l = l;
	code[cx].a = a;
	cx++;
}

//д����ű�
void enter(string name, id_type type, int level, int value, int address, int size) {
	table[tx].name = name;
	table[tx].type = type;
	table[tx].level = level;
	table[tx].value = value;
	table[tx].address = address;
	table[tx].size = size;
	tx++;
}

//�ڷ��ű����������󣬷�����ָ��
int find_name_index(string name) {
	for (int i = tx-1; i >=0; i--) {//��ջ����ջ���������ض���ʱ�������ȷ��ʵ����¶���
		if (table[i].name == name) return i;
	}
	return -1;
}

//��������
bool exp(int level);
bool body(int level);
bool block(int proc_name_index, int level);

//���������������
typedef enum {
	use_before_definition,//δ�����ʹ��
	redefinition,//�ض���
	assignment_error,//��ͼ�ԷǱ�����ֵ
	par_number_error,//������������
	proc_in_exp//�����������ڱ��ʽ��
}meaning_error_type;

//�������������
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

//<factor> �� <id>|<integer>|(<exp>)
bool factor(int level) {
	bool flag = 1;//��ǵ�ǰ����﷨�����Ƿ����
	SYM temp = getword();
	if (temp.attribute == "id") {
		int val_name_index = find_name_index(temp.word);//���ű������ñ������������ڷ��ű��е�λ��
		if (val_name_index == -1) {//δ�����ʹ��
			meaning_error(use_before_definition, temp); 
		}
		else {
			if (table[val_name_index].type == constant) {//������
				gen("lit", 0, table[val_name_index].value);//���������滻Ϊ������
			}
			else if (table[val_name_index].type == variable) {//����
				int val_level = table[val_name_index].level;//�ñ����Ĳ��
				int val_dx = table[val_name_index].address;//�ñ�����ƫ����
				gen("lod", level - val_level, val_dx);//ȡ����ֵ����ջ��
			}
			else {//������
				meaning_error(proc_in_exp, temp);
			}
		}
	}
	else if (temp.attribute == "digits") {//������
		gen("lit", 0, stoi(temp.word));//ȡ��������ջ��
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

//<term> �� <factor>{<mop><factor>}
bool terms(int level) {
	bool flag = 1;
	if (!factor(level)) flag = 0;
	SYM temp = getword();
	while (temp.attribute == "mop" || (temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
		while (temp.attribute == "mop") {
			int mop_type;//��¼mop���ͣ�* or /)
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

//<exp> �� [+|-]<term>{<aop><term>}
bool exp(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.attribute != "aop") backword();
	if (temp.word == "-") gen("opr", 0, 1);//ȡ�෴��
	if (!terms(level)) flag = 0;
	temp = getword();
	while (temp.attribute == "aop" || (temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
		while (temp.attribute == "aop") {
			int aop_type ;//��¼aop���ͣ�+ or -)
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

//<lexp> �� <exp> <lop> <exp>| odd <exp>
bool lexp(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.word == "odd") {
		if(!exp(level)) flag=0;
		gen("opr", 0, 6);//�ж���ż��
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
		int lop_type;//��¼lop����
		if (temp.word == "=") lop_type = 7;//ջ�����ջ���Ƿ����
		else if (temp.word == "<>") lop_type = 8;//ջ�����ջ���Ƿ񲻵�
		else if (temp.word == "<") lop_type = 9;//��ջ���Ƿ�С��ջ��
		else if (temp.word == ">=") lop_type = 10;//��ջ���Ƿ���ڵ���ջ��
		else if (temp.word == ">") lop_type = 11;//��ջ���Ƿ����ջ��
		else if (temp.word == "<=") lop_type = 12;//��ջ���Ƿ�С�ڵ���ջ��
		if (!exp(level)) flag = 0;
		gen("opr", 0, lop_type);
	}
	return flag;
}

//<statement> �� <id> : = <exp>
//			| if <lexp> then <statement>[else <statement>]
//			| while <lexp> do <statement>
//			| call <id>��[<exp>{, <exp>}]��
//			| <body>
//			| read(<id>{��<id>})
//			| write(<exp>{, <exp>})
bool statement(int level) {
	bool flag = 1;
	SYM temp = getword();
	if (temp.attribute == "id") {
		int val_name_index = find_name_index(temp.word);//���ű������ñ������������ڷ��ű��е�λ��
		int val_level;//�ñ����Ĳ��
		int val_dx;//�ñ�����ƫ����
		if (val_name_index == -1) {//δ�����ʹ��
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
		gen("sto", level - val_level, val_dx);//ȡջ��Ԫ�ظ����ñ���
	}
	else if (temp.word == "if") {
		if (!lexp(level)) flag = 0;
		int cx1 = cx;//Ϊ��jpc��丽�ϱ�ǩ���ڻ���
		gen("jpc", 0, 0);//��ת���ٳ��ڣ��ȴ�����
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
		code[cx1].a = cx;//����ٳ���
		temp = getword();
		if (temp.word == "else" || temp.attribute == "id" || temp.word == "if" || temp.word == "while" || temp.word == "call" || temp.word == "begin" || temp.word == "read" || temp.word == "write") {
			int cx2 = cx;//��ʱ����else��Ϊ��jmp��丽�ϱ�ǩ���ڻ���
			gen("jmp", 0, 0);//��ת������ڣ��ȴ�����
			code[cx1].a = cx;//�ٳ�����Ҫ���£���jmpָ��ĺ�һ��ָ��
			if (temp.word != "else") {
				flag = 0;
				parse_error(15, temp);
				backword();
			}
			if (!statement(level)) flag = 0;
			code[cx2].a = cx;//���������
		}
		else backword();
	}
	else if (temp.word == "while") {
		int cx1 = cx;//��while�ĳ�ʼλ�ã����ڻ���
		if (!lexp(level)) flag = 0;
		int cx2 = cx;//Ϊ��jpc��丽�ϱ�ǩ���ڻ���
		gen("jpc", 0, 0);//��ת���ٳ��ڣ��ȴ�����
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
		gen("jmp", 0, cx1);//��ת��while��ʼλ��
		code[cx2].a = cx;//����ٳ���
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
		int proc_name_index = find_name_index(temp.word);//���ű������ù��������������ڷ��ű��е�λ��
		if (proc_name_index == -1) {//δ�����ʹ��
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
		int num_actualPar = 0;//ʵ�ʲ�������
		temp = getword();
		if (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits") {
			int dx = 3;//��ʽ��������ʼƫ����
			backword();
			if (!exp(level)) flag = 0;
			num_actualPar++;
			gen("sto", -1, dx);//��������
			dx++;//ƫ����++
			temp = getword();
			while (temp.word == "," || (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
				while (temp.word == ",") {
					if (!exp(level)) flag = 0;
					num_actualPar++;
					gen("sto", -1, dx);
					dx++;//ƫ����++
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
		gen("cal", level- table[proc_name_index].level, table[proc_name_index].value);//l���������Ӿ�̬����a��Ϊ����ָ���ָ��
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
		int val_name_index = find_name_index(temp.word);//���ű������ñ������������ڷ��ű��е�λ��
		int val_level;//�ñ�����γ�ʼ��
		int val_dx;//�ñ���ƫ������ʼ��
		if (val_name_index == -1) {//δ�����ʹ��
			meaning_error(use_before_definition, temp);
		}
		else if (table[val_name_index].type != variable) {//ֻ��д�����
			meaning_error(assignment_error, temp);
		}
		else {
			val_level = table[val_name_index].level;
			val_dx = table[val_name_index].address;
		}
		gen("red", 0, 0);//�������ж�ȡһ����������ջ��
		gen("sto", level - val_level, val_dx);//ջ�������ñ���
		temp = getword();
		while (temp.word == "," || temp.attribute == "id") {
			while (temp.word == ",") {
				temp = getword();
				if (temp.attribute != "id") {
					parse_error(0, temp);
					flag = 0;
					backword();
				}
				val_name_index = find_name_index(temp.word);//���ű������ñ������������ڷ��ű��е�λ��
				if (val_name_index == -1) {//δ�����ʹ��
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
		gen("wrt", 0, 0);//���ջ������Ļ
		gen("opr", 0, 13);//������з�
		temp = getword();
		while (temp.word == "," || (temp.attribute == "aop" || temp.word == "(" || temp.attribute == "id" || temp.attribute == "digits")) {
			while (temp.word == ",") {
				if (!exp(level)) flag = 0;
				gen("wrt", 0, 0);//���ջ������Ļ
				gen("opr", 0, 13);//������з�
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

//<body> �� begin <statement>{;<statement>} end
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

//<proc> �� procedure <id>��[<id>{,<id>}]��;<block>{;<proc>}
bool proc(int level) {
	int num_formalPar = 0;//�βθ���
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
	int proc_name_index = tx;//�ù������ڷ��ű��е�ָ��
	if (find_name_index(temp.word) != -1) {//�ض������
		meaning_error(redefinition, temp);
	}
	else {
		enter(temp.word, procedure, level - 1, -1, 0, 3);//������д����ű�������������һ��
	}
	int orginal_tx = tx;//��ǰtxֵ�����̽�������ջ���ˣ���ֻ�����������ڷ��ű���
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
		num_formalPar++; //��ǰ�ܿռ�Ϊ3+num_formalPar��ƫ����Ϊ3 + num_formalPar - 1������ͬ��
		int var_name_index = find_name_index(temp.word);
		if (var_name_index != -1 && table[var_name_index].type != variable) {//���ű�����ڸ����Ƶĳ����������
			meaning_error(redefinition, temp);
		}
		else {
			enter(temp.word, variable, level, -1, 3 + num_formalPar - 1, -1);//��ʽ����д����ű�
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
				if (var_name_index != -1 && table[var_name_index].type != variable) {//���ű�����ڸ����Ƶĳ����������
					meaning_error(redefinition, temp);
				}
				else {
					enter(temp.word, variable, level, -1, 3 + num_formalPar - 1, -1);//��ʽ����д����ű�
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
	table[proc_name_index].address = num_formalPar;//adrress���¼�β�������������������ƥ��Ĵ���
	table[proc_name_index].size += num_formalPar;//���¸ù��̵�size��ĿǰΪ3+�β�����
	if (!block(proc_name_index, level)) flag = 0;
	tx = orginal_tx;//�ָ����ű������ű�ջ��ֻ�����ù��̵Ĺ�����
	temp = getword();
	while (temp.word == ";" || temp.word == "procedure") {
		while (temp.word == ";") {
			if (!proc(level)) flag = 0;//ͬ��procedure
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

//<vardecl> �� var <id>{,<id>};
bool vardecl(int proc_name_index, int level) {
	int dx = table[proc_name_index].size;//�ù��̵�ǰ����ռ䣨3+�βθ����������ڼ���д��ı�����ƫ����
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
	if (var_name_index != -1 && table[var_name_index].type != variable) {//���ű�����ڸ����Ƶĳ����������
		meaning_error(redefinition, temp);
	}
	else {
		enter(temp.word, variable, level, -1, dx, -1); //����д����ű�
	}
	dx++;//ƫ����++
	temp = getword();
	while (temp.word == "," || temp.attribute == "id") {
		while (temp.word == ",") {
			temp = getword();
			if (temp.attribute != "id") {
				parse_error(0, temp);
				flag = 0;
				backword();
			}
			if (var_name_index != -1 && table[var_name_index].type != variable ) {//���ű�����ڸ����Ƶĳ����������
				meaning_error(redefinition, temp);
			}
			else {
				enter(temp.word, variable, level, -1, dx, -1); //����д����ű�
			}
			dx++;//ƫ����++
			temp = getword();
		}
		backword();
		temp = getword();
		if (temp.attribute == "id") {//��ʱ�﷨�����Ѿ������ʲ����ٽ������������������û��д����ű�
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
	table[proc_name_index].size = dx;//���µ�ǰ���̵�����ռ䣨3+�βθ���+����������
	return flag;
}

//<const> �� <id>: = <integer>
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
	int constpar_value = stoi(temp.word);//������ֵ
	int constpar_name_index = find_name_index(temp_const.word);
	if (constpar_name_index != -1 && table[constpar_name_index].type != constant ) {//���ű�����ڸ����Ƶı����������
		meaning_error(redefinition, temp_const);
	}
	else {
		enter(temp_const.word, constant, level, constpar_value, -1, -1);//������д����ű�
	}
	return flag;
}

//<condecl> �� const <const>{,<const>};
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
	while (temp.word == "," || temp.attribute == "id") {//����հ�
		while (temp.word == ",") {
			if (!func_const(level)) flag = 0;
			temp = getword();
		}
		backword();
		temp = getword();
		if (temp.attribute == "id") {//ȱ��","��������������
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

//<block> �� [<condecl>][<vardecl>][<proc>]<body>
bool block(int proc_name_index, int level) {//proc_name_index�Ƕ�Ӧ�Ĺ������ڷ��ű��е�ָ��
	int orginal_cx = cx;//��ǰcxֵ������jmp��label,���ڻ����jmp
	gen("jmp", 0, 0);//�ȴ�����
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
		if (!proc(level+1)) flag = 0;//Ƕ�׶���
		temp = getword();
	}
	backword();
	code[orginal_cx].a = cx;//����jmpָ���ת����ǰ��ָ��ִ�У�����block��body����
	table[proc_name_index].value = orginal_cx;//��ǰ���̵ĵ�һ��ָ�����calָ���a��
	gen("int", 0, table[proc_name_index].size);//Ϊ�ù��̷���ռ�
	if (!body(level)) flag = 0;
	gen("opr", 0, 0);//����������ָ�����ջ
	return flag;
}

//<prog> �� program <id>;<block>
bool prog() {//����ֵ��flag���������Ƿ����﷨��������ͬ��
	bool flag = 1;
	SYM temp = getword();
	if (temp.word != "program") {
		flag = 0;
		while (synchronous(1, temp)) {//ͬ��
			parse_error(0,temp);//����"program",��Ϊ�Ƕ���ķ���
			temp = getword();
		}
		if (temp.word != "program") {
			parse_error(1, temp);//ͬ������̷�����Ȼ��"program",��ȱ��"program"
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
	if (find_name_index(temp.word) != -1) {//�ض������
		meaning_error(redefinition, temp);
	}
	else{
		enter(temp.word, procedure, 0, 0, 0, 3);//��������д����ű�
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
	if (!block(0, 0)) flag = 0;//0��0�ֱ������������ڷ��ű��е�ָ����0�����Ϊ0
	return flag;
}

bool parse() {
	parse_fp_in.open("middle_program.txt", ios::in);//���м��ļ���ȡÿ������
	if (parse_fp_in.fail()) return 0;
	if (prog()) {//�﷨�����޴���
		cout << "no parse error"<<endl;
		return 1;
		parse_fp_in.close();
	}
	else {
		return 0;
		parse_fp_in.close();
	}
}

//������-----------------------------------------------------------------------------------------------------------
int STACK[10000];//����ջ

//��̬����ͨ������ƫ����ȷ������λ��
int get_position(int B, int l, int a) {
	while (l > 0) {
		B = STACK[B+2];//B+2Ϊ��̬����BΪ����ַ����̬����
		l--;
	}
	return B + a;
}

//������ʵ��
void interpreter() {
	int B = 0;//��ǰ���̵Ļ���ַ
	int P = 0;//��һ���ȴ�ִ�е�ָ���ָ��
	instruction I;//��ǰִ�е�ָ��
	int T = 0;//ָջSTACK��ջ��
	do {
		I = code[P];
		P++;
		if (I.f == "lit") {//ȡ��������ջ��
			STACK[T] = I.a;
			T++;
			
		}
		else if (I.f == "opr") {
			switch (I.a) {
			case 0: {
				P = STACK[B + 1];//ȡ���ص�ַΪ��һ����ִ�е�ָ��
				T = B;//��ջ
				B = STACK[B];//���µ�ǰ���̵Ļ���ַ
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
			case 13: {//������з�
				cout << endl;
			}	  
			}
		}
		else if (I.f == "lod") {//����ֵ����ջ��
			STACK[T] = STACK[get_position(B, I.l, I.a)];
			T++;
		}
		else if (I.f == "sto") {//ջ��ֵ��������
			if (I.l == -1) {//sto��l��Ϊ-1ʱ��ʾ��������
				STACK[T - 1 + I.a] = STACK[T - 1];
			}
			else {//ջ��ֵ��������
				STACK[get_position(B, I.l, I.a)] = STACK[T - 1];
			}
			T--;
		}
		else if (I.f == "cal") {//���̵���
			STACK[T+2] = get_position(B, I.l, 0);//���Ӿ�̬��
			STACK[T] = B;//���Ӷ�̬��
			B = T;//���µ�ǰ���̵Ļ���ַ
			STACK[T + 1] = P;//���ص�ַΪ���ý������ִ�е�ָ���ָ��
			P = I.a;//��ת���¹��̵ĵ�һ��ָ��
		}
		else if (I.f == "int") {//���ٿռ�
			T += I.a;
		}
		else if (I.f=="jmp") {//��������ת
			P = I.a;
		}
		else if (I.f == "jpc") {//��������ת
			if(STACK[T-1]==0)	P = I.a;
			T--;
		}
		else if (I.f == "red") {//�������ж�������ջ��
			cin >> STACK[T];
			T++;
		}
		else if (I.f == "wrt") {//��ջ��д������������
			cout << STACK[T - 1];
		}
	}while (P != 0);//ָ��������
}

int main() {
	if (lex()) {
		if (parse()) {//���﷨����ʱ�Ž����м����Ľ���ִ��
			cout << "no meaning error" << endl;
			//for (int i = 0; i < cx; i++) {//����м����
			//	cout << i << " " << code[i].f << " " << code[i].l << " " << code[i].a << endl;
			//}
			interpreter();//�м����Ľ���ִ��
		}
	}
}
	