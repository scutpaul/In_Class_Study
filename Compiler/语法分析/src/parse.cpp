#include<iostream>
#include<iomanip>
#include<fstream>   //oftream
#include<string>
#include<map>
#include<vector>
#include<algorithm>
#include <sstream>
#define N 100
#define token_N 1000
#define Default_char 20
using namespace std;
enum type { skey, id, singlebound, doublebound, conststring, constnumber };
string key[35] = { "and","array","begin","bool","call","case","char","constant","dim","do","else","end","false","for",
"if","input","integer","not","of","or","output","procedure","program","read","real","repeat","set","stop",
"then","to","true","until","var","while","write" };
enum  tokentype {
	HEAD,
	/* reserved words */
	AND, ARRAY, BEGIN, BOOL, CALL, CASE, CHAR, CONSTANT, DIM, DO,
	ELSE, END, RFALSE, FOR, IF, INPUT, INTEGER, NOT, OF, OR, OUTPUT,
	PROCEDURE, PROGRAM, READ, REAL, REPEAT, SET, STOP, THEN, TO, RTRUE, UNTIL, VAR, WHILE, WRITE,
	/* multicharacter tokens */
	ID, NUM,
	/* special symbols */
	CONSTANTSTRING, LPAREN, RPAREN, TIMES, COMMENTEND, PLUS, COMMA, MINUS,
	DOT, DOTDOT, OVER, COMMENTBEGIN, COLON, ASSIGN, SEMI, LT, LEQ,
	NEQ, EQ, GT, GEQ, LMPAREN, RMPAREN,
	COMMENT, ENDFILE, ERROR,JUMPNZ,JUMP
};
struct token {
	type kind;
	string values;
	int kid;
	int line;
	int index;
};
vector<token> tokenv;
map<char, int>single;
map<char, int>doubleb;
map<string, int>idmap;
map<string, int>stringmap;
map<string, int>symbol;           //声明表  A:kid
map<string, string>assign_form;   //赋值表  A:values
map<tokentype, int>stmt;
int idindex;
int iserror;
//string compiler_file;
char str[N];
int index;
int line;
int tmpvaluecode = 1;

struct Quaternion {
	int begincode;
	int chain;
	tokentype op;
	string t1, t2,t3;
};
vector<Quaternion>Quaternion_list;
int nextstat = 0;
struct Node {
	int codebegin;
	string place;
	int chain;
	int chain_true;
	int chain_false;
};
/*-----------函数function声明区--------------*/
Node arithmetic_basic();
Node arithmetic_factor();
Node arithmetic_item();
Node arithmetic_expression();
/*-------------*/
Node bool_expression();
Node bool_factor();
Node bool_item();
Node bool_basic();
/*-------------*/
Node assign_statement();
Node if_statement();
Node while_stmt();
Node repeat_statement();
Node compound_statement();
Node statement();	
Node statement_form();
/*-------------*/
void id_form(vector<string> &t_symbol);
void variable_define();
void variable_declaration();
void program_statement();
bool check_stat_end();
bool check_relation_symbol();
void print_all_quaternion();
void print_quaternion(Quaternion it, int index);
void parse_error(string error_message);
/*---------------------------------------*/
void error_show(int kind) {
	if (kind == 1) {  //note_error
		cout << "line:" << line << "note without \"* / \"" << endl;
	}
	else if (kind == 2) {
		cout << "line:" << line << "string without end\'" << endl;
	}
	else if (kind == 3) {
		cout << "line:" << line << "   index:" << index + 1 << "unknown token" << endl;
	}

	exit(0);
}
void delete_space() {
	while (str[index] == ' ') {
		index++;
	}
}
void scan_note() {/*note*/
	if (str[index] == '/'&& str[index + 1] == '*') {
		bool quit_note = false;
		index += 2;
		while (str[index] != 0) {
			if (str[index] == '*'&&str[index + 1] == '/') {
				quit_note = true;
				index += 2;
				break;
			}
			else {
				index++;
				continue;
			}
		}
		if (!quit_note) error_show(1);
	}
	else;
}
void scan_string() {
	if (str[index] == '\'') {
		bool quit_string = false;
		index++;
		int string_start = index;
		int string_end = index;
		while (str[index] != 0) {
			if (str[index] == '\'') {
				string_end = index;
				index++;
				token i;
				i.kind = conststring;
				i.kid = 38;
				string s(&str[string_start], &str[string_end]);
				i.values = s;
				if (stringmap.count(s) == 0) {
					stringmap[s] = idindex;
					idindex++;
				}
				//charcpy(i.values, str, string_start, string_end);
				i.line = line;
				i.index = index;
				tokenv.push_back(i);
				quit_string = true;
				break;
			}
			else {
				index++;
				continue;
			}
		}
		if (!quit_string)error_show(2);
	}
}
void scan_number() {
	int number_start = index;
	while (str[index] >= '0'&&str[index] <= '9') {
		index++;
	}
	token i;
	i.kind = constnumber;
	i.kid = 37;
	string s(&str[number_start], &str[index]);
	i.values = s;
	if (idmap.count(s) == 0) {
		idmap[s] = idindex;
		idindex++;
	}
	i.line = line;
	i.index = index;
	tokenv.push_back(i);
}
void scan_id_key() {
	int id_start = index;
	token i;
	while (str[index] >= 'a'&&str[index] <= 'z' || str[index] >= 'A'&&str[index] <= 'Z' || str[index] >= '0'&&str[index] <= '9') {
		index++;
	}
	string s(&str[id_start], &str[index]);
	if (lower_bound(key, key + 35, s) != key + 35 && s == *lower_bound(key, key + 35, s)) {
		i.kind = skey;
		i.kid = lower_bound(key, key + 35, s) - key + 1;
	}
	else {
		i.kind = id;
		i.kid = 36;
		i.values = s;
		if (idmap.count(s) == 0) {
			idmap[s] = idindex;
			idindex++;
		}
	}
	i.line = line;
	i.index = index;
	tokenv.push_back(i);

}
void scan_singlebound() {
	token i;
	i.kind = singlebound;
	i.kid = single[str[index]];
	index++;
	i.line = line;
	i.index = index;
	tokenv.push_back(i);
}
void scan_doublebound() {
	bool single_double = false;
	int kid = -1;
	token i;
	if (str[index] == '*'&&str[index + 1] == '/') {
		kid = 42;
	}
	else if (str[index] == '<'&&str[index + 1] == '=') {
		kid = 54;
	}
	else if (str[index] == '>'&&str[index + 1] == '=') {
		kid = 58;
	}
	else if (str[index] == ':'&&str[index + 1] == '=') {
		kid = 51;
	}
	else if (str[index] == '<'&&str[index + 1] == '>') {
		kid = 55;
	}
	else if (str[index] == '.'&&str[index + 1] == '.') {
		kid = 47;
	}
	else {
		i.kind = singlebound;
		i.kid = doubleb[str[index]];
		index++;
		i.line = line;
		i.index = index;
		tokenv.push_back(i);
		return;
	}

	i.kind = doublebound;
	i.kid = kid;
	i.line = line;
	i.index = index + 1;
	tokenv.push_back(i);
	index += 2;
}
void scanning() {
	delete_space();
	scan_note();
	scan_string();
	if (str[index] != 0) {
		if (single.count(str[index]) != 0) {
			scan_singlebound();
		}
		else if (str[index] >= '0'&&str[index] <= '9') {
			scan_number();
		}
		else if (str[index] >= 'a'&&str[index] <= 'z' || str[index] >= 'A'&&str[index] <= 'Z') {
			scan_id_key();
		}
		else if (doubleb.count(str[index]) != 0) {
			scan_doublebound();
		}
		else if (str[index] == ' ') {

		}
		else {
			error_show(3);
		}
	}
}
void login_show() {
	cout << "--------****SAMPLE_COMPILER****--------" << endl;
	cout << "----陈浩鑫_201530311042_15计科一班-----" << endl;
	cout << "---------------------------------------" << endl;
}
void init() {
	iserror = false;
	//compiler_file = "D:\\scanning_test.txt";
	index = 0;
	line = 0;
	single['('] = 39;
	single[')'] = 40;
	single['+'] = 43;
	single[','] = 44;
	single['-'] = 45;
	single[';'] = 52;
	single['='] = 56;
	single['['] = 59;
	single[']'] = 60;
	doubleb['*'] = 41;
	doubleb['.'] = 46;
	doubleb['/'] = 48;
	doubleb[':'] = 50;
	doubleb['<'] = 53;
	doubleb['>'] = 57;
	idindex = 1;

}
void output() {
	for (unsigned i = 0;i < tokenv.size();i++) {
		if (tokenv[i].kid == 36)
			cout << "(36," << idmap[tokenv[i].values] << ") ";
		else if (tokenv[i].kid == 37)
			cout << "(37," << idmap[tokenv[i].values] << ") ";
		else if (tokenv[i].kid == 38)
			cout << "(38," << stringmap[tokenv[i].values] << ") ";
		else
			cout << "(" << tokenv[i].kid << ",-) ";
		if ((i + 1) % 5 == 0)cout << endl;
	}
}
void parse_error(string error_message) {
	cout << "Parse error!!!:  Line:" << tokenv[idindex].line << "  index:" << tokenv[idindex].index << endl;
	cout << "Error Message:  " << error_message << endl;
	exit(0);
}
/*匹配token*/
bool matchtoken(tokentype t) {
	if (tokenv[idindex].kid == t) {
		return true;
	}
	return false;
}
/*整合*/
int merge(int p1, int p2) {    //拉链
	if (p2 == 0)return p1;
	int q=Quaternion_list[p2].chain;
	int p = p2;
	while (q != 0) {
		p = q;
		q = Quaternion_list[p].chain;
	}
	Quaternion_list[p].chain = p1;
	return p2;

}
/*回填*/
void backpatch(int p1, int t) {   //回填
	if (p1 == 0)return;
	int q=Quaternion_list[p1].chain;
	int p = p1;
	while (q != 0) {
		Quaternion_list[p].chain=t;
		p = q;
		q= Quaternion_list[p].chain;
	}
	Quaternion_list[p].chain = t;
}
/*产生新变量*/
string newtemp() {
	ostringstream s;
	s << "T" << tmpvaluecode;
	tmpvaluecode++;
	return s.str();
}
/*打印四元式*/
void print_quaternion(Quaternion it,int index) {
	cout << "(" << index << ")  (";
	switch (it.op) {
	case HEAD:
		cout << "program," << it.t1 << "," << "-,-)";
			break;
	case ASSIGN:
		cout << ":= ,  " << it.t1 << "  ,  -  ,  " << it.t2 << ")";
		break;
	case JUMPNZ:
		cout << "jnz,  " << it.t1 << "  ,  -  ,  " << it.chain << ")";
		break;
	case JUMP:
		cout << "j  ,  -  ,  -  ,  " << it.chain << ")";
		break;
	case EQ:
		cout << "j= " << ",  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.chain << ")";
		break;
	case GT:
		cout << "j> " << ",  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.chain << ")";
		break;
	case GEQ:
		cout << "j>="  << ",  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.chain << ")";
		break;
	case LT:
		cout << "j< "  << ",  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.chain << ")";
		break;
	case LEQ:
		cout << "j<=" << ",  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.chain << ")";
		break;
	case NEQ:
		cout << "j<>"<< ",  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.chain << ")";
		break;
	case TIMES:
		cout << "*  ,  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.t3 << ")";
		break;
	case PLUS:
		cout << "+  ,  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.t3 << ")";
		break;
	case MINUS:
		cout <<"-  ,  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.t3 << ")";
		break;
	case OVER:
		cout <<"/  ,  " << it.t1 << "  ,  " << it.t2 << "  ,  " << it.t3 << ")";
		break;
	case ENDFILE:
		cout << "sys,  -  ,  -  ,  -)";
	default:
		break;
	}
	cout << endl;
}
/*产生四元式并将之入栈*/
void emit(tokentype tkop, string t1, string t2, int chain, string t3 = "0") {
	//cout << "DEBUG-emit:" << endl;
	Quaternion tmp_fp;
	tmp_fp.op = tkop;
	tmp_fp.chain = chain;
	tmp_fp.t1 = t1;
	tmp_fp.t2 = t2;
	tmp_fp.t3 = t3;
	
	nextstat++;
	Quaternion_list.push_back(tmp_fp);
	//fourout();
	//cout << "------------------" << endl;
}
bool check_stat_end() {
	return (matchtoken(END) || matchtoken(ELSE) || matchtoken(UNTIL) || matchtoken(SEMI));
}
bool check_relation_symbol() {
	return (matchtoken(LEQ) || matchtoken(LT) || matchtoken(EQ) || matchtoken(GT) || matchtoken(GEQ)||matchtoken(NEQ));
}
/*<算术量> → <整数>│<标识符>│（ <算术表达式> ）*/
Node arithmetic_basic() {
	Node this_return;
	token this_token = tokenv[idindex];
	switch (this_token.kid)
	{
	case NUM:
		this_return.place = this_token.values;
		idindex++;
		return this_return;
		break;
	case ID:
		if (symbol.count(this_token.values) != 0) {
			if (symbol[this_token.values] == INTEGER) {
				if (assign_form.count(this_token.values) != 0) {
					this_return.place = this_token.values;
					idindex++;
					return this_return;
				}
				else {
					parse_error("arithmetic basic:must be assign");
				}
			}
			else {
				parse_error("arithmetic basic:must be int");
			}
		}
		else {
			parse_error("arithmetic basic:must be declared");
		}
		break;
	case LPAREN:
		idindex++;
		this_return.place = arithmetic_expression().place;
		if (matchtoken(RPAREN)) {
			idindex++;
			return this_return;
		}
		else {
			parse_error("arithmetic basic error:must with a Rightpart");
		}
		break;
	default:
		parse_error("arithmetic basic error : error");
		break;
	}
}
/*<因子> → <算术量>│- <因子>*/
Node arithmetic_factor() {
	if (matchtoken(NUM) || matchtoken(ID) || matchtoken(LPAREN)) {
		return arithmetic_basic();
	}
	else if (matchtoken(MINUS)) {
		idindex++;
		Node temp = arithmetic_basic();
		Node temp2;
		Node new_tmp;
		temp2.place = "0";
		new_tmp.place = newtemp();
		emit(MINUS, temp2.place, temp.place,0, new_tmp.place);
		return new_tmp;
	}
	else {
		parse_error("arithmetic factor error:start error");
	}
}
/*<项> → <因子><项中>     <项中> → ε│/ <因子>│*<因子 */
Node arithmetic_item() {
	if (matchtoken(NUM) || matchtoken(ID) || matchtoken(LPAREN)||matchtoken(MINUS) ){
		Node tmp = arithmetic_factor();
		if (matchtoken(TIMES) || matchtoken(OVER)) {
			tokentype tmptype = (tokentype)tokenv[idindex].kid;
			idindex++;
			Node tmp2 = arithmetic_factor();
			Node new_tmp;
			new_tmp.place = newtemp();
			emit(tmptype, tmp.place, tmp2.place, 0, new_tmp.place);
			return new_tmp;
		}
		else {
			return tmp;
		}
	}
	else {
		parse_error("arithmetic_item error:start error");
	}
}
/*<算术表达式> → <项> <算术表达式中>    <算术表达式中> → ε │+<项>│- <项>*/
Node arithmetic_expression() {
	if (matchtoken(NUM) || matchtoken(ID) || matchtoken(LPAREN)||matchtoken(MINUS)) {
		Node tmp1 = arithmetic_item();
		if (matchtoken(PLUS) || matchtoken(MINUS)) {
			tokentype tmptype = (tokentype)tokenv[idindex].kid;
			idindex++;
			Node tmp2 = arithmetic_item();
			Node new_tmp;
			new_tmp.place = newtemp();
			new_tmp.codebegin = tmp1.codebegin;
			emit(tmptype, tmp1.place, tmp2.place, 0,new_tmp.place);
			return new_tmp;
		}
		else {
			return tmp1;
		}
	}
	else {
		parse_error("arithmetic error:no a legel arithmetic expression");
	}
}
/*<算术表达式> <关系符> <算术表达式>*/
Node bool_exp_symbol_exp() {
	Node S,exp1,exp2;
	S.codebegin = nextstat;
	if (matchtoken(INTEGER) || matchtoken(ID) || matchtoken(LPAREN)||matchtoken(MINUS))
		exp1 = arithmetic_expression();
	else {
		parse_error("bool_exp_symbol_exp:start error");
	}
	if (!check_relation_symbol()) {
		cout << "tset:" << tokenv[idindex].kid;
		parse_error("bool_exp_symbol_exp:lost relation symbol");
	}
	token rop_symbol = tokenv[idindex];
	idindex++;
	if (matchtoken(NUM) || matchtoken(ID) || matchtoken(LPAREN)||matchtoken(MINUS))
		exp2 = arithmetic_expression();
	else {
		parse_error("bool_exp_symbol_exp:statement start error" );
	}
	S.chain_true = nextstat;
	emit((tokentype)rop_symbol.kid, exp1.place, exp2.place, 0);
	S.chain_false = nextstat;
	emit(JUMP, "0", "0", 0);
	return S;
}
/*<布尔量> → <布尔常量>│<标识符>│（ <布尔表达式> ）│<算术表达式> <关系符> <算术表达式>*/
Node bool_basic() {
	Node S;
	Node temp;
	S.codebegin = nextstat;
	token temp_id = tokenv[idindex];
	int temp_id_index = idindex + 1;
	int rpa = 1;
	int temp_to_idindex;
	switch (tokenv[idindex].kid)
	{
	case RTRUE:
		idindex++;
		S.chain_true = S.codebegin;
		S.chain_false = 0;
		emit(JUMP, "0", "0", 0);
		break;
	case RFALSE:
		idindex++;
		S.chain_false = S.codebegin;
		S.chain_true = 0;
		emit(JUMP, "0", "0", 0);
		break;
	case ID:
		idindex++;
		if (!(check_relation_symbol() || matchtoken(PLUS) || matchtoken(MINUS) || matchtoken(TIMES) || matchtoken(OVER))) {
			idindex--;
			if (symbol.count(temp_id.values) == 0) {
				parse_error("bool basic:id should be declared");
			}
			if (symbol[temp_id.values] != BOOL) {
				parse_error("bool basic:type of id should be bool");
			}
			if (assign_form.count(temp_id.values) == 0) {
				parse_error("bool basic:id should be assgin");
			}
			emit(JUMPNZ, temp_id.values, "0", 0);
			emit(JUMP, "0", "0", 0);
			S.chain_true = S.codebegin;
			S.chain_false = S.codebegin + 1;
		}
		else {
			idindex--;
			Node temp = bool_exp_symbol_exp();
			S.chain_true = temp.chain_true;
			S.chain_false = temp.chain_false;
		}
		break;
	case LPAREN:
		while (rpa != 0) {
			if (tokenv[temp_id_index].kid == ENDFILE) {
				parse_error("bool basic: lost the right part \")\"!!");
			}
			if (tokenv[temp_id_index].kid == LPAREN)
				rpa++;
			else if (tokenv[temp_id_index].kid == RPAREN)
				rpa--;
			temp_id_index++;
		}
		temp_to_idindex = idindex;
		idindex = temp_id_index;
		if (check_relation_symbol()) {
			idindex = temp_to_idindex;
			Node temp = bool_exp_symbol_exp();
			S.chain_true = temp.chain_true;
			S.chain_false = temp.chain_false;
		}
		else {
			idindex = temp_to_idindex;
			idindex++;
			Node tmp = bool_expression();
			if (!matchtoken(RPAREN)) {
				parse_error("bool basic:lost )");
			}
			idindex++;
			S.chain_true = tmp.chain_true;
			S.chain_false = tmp.chain_false;
		}
		break;
	case MINUS:
	case INTEGER:
		temp = bool_exp_symbol_exp();
		S.chain_true = temp.chain_true;
		S.chain_false = temp.chain_false;
		break;
	default:
		parse_error("bool basic:wrong type");
		break;
	}
	return S;
}
/*<布因子> → <布尔量>│not <布因子>*/
Node bool_factor() {
	if (!(matchtoken(RTRUE) || matchtoken(RFALSE) || matchtoken(ID) || matchtoken(LPAREN) || matchtoken(INTEGER)||matchtoken(NOT))) {
		parse_error("bool_factor error:start error");
	}
	Node S;
	if (matchtoken(NOT)) {
		S = bool_factor();
		S.chain_false = S.chain_true;
		S.chain_true = S.chain_false;
	}
	else {
		S = bool_basic();
	}
	return S;
}
/*<布尔项> → <布因子><布尔项中>       <布尔项中> → ε│and <布因子>*/
Node bool_item() {
	if (!(matchtoken(RTRUE) || matchtoken(RFALSE) || matchtoken(ID) || matchtoken(LPAREN) || matchtoken(INTEGER))) {
		parse_error("bool_item error:start error");
	}
	Node S = bool_factor();
	if (matchtoken(AND)) {
		idindex++;
		Node tmp = bool_item();
		backpatch(S.chain_true, tmp.codebegin);
		S.chain_false = merge(S.chain_false, tmp.chain_false);
		S.chain_true = tmp.chain_true;
	}
	else if (matchtoken(OR) || matchtoken(THEN) || matchtoken(DO) || check_stat_end()) {
	}
	else {
		//parse_error("bool_item error:end error");
	}
	return S;
}
/*<布尔表达式> → <布尔项><布尔表达式中>     <布尔表达式中> →ε│or <布尔项>*/
Node bool_expression() {
	if (!(matchtoken(RTRUE) || matchtoken(RFALSE) || matchtoken(ID) || matchtoken(LPAREN) || matchtoken(INTEGER))){
		parse_error("bool_expression error:start error");
	}
	Node S = bool_item();
	if (matchtoken(OR)) {
		idindex++;
		Node tmp = bool_item();
		backpatch(S.chain_false, tmp.codebegin);
		S.chain_true = merge(S.chain_true, tmp.chain_true);
		S.chain_false = tmp.chain_false;
	}
	else if(matchtoken(THEN)||matchtoken(DO)||check_stat_end()){
		
	}
	else {

		//parse_error("bool_expression error:end error");
	}
	return S;
}
/*<if句>→ if <布尔表达式> then <语句>│if <布尔表达式> then <语句> else <语句>*/
Node if_statement() {
	Node S;
	S.codebegin = nextstat;
	idindex++;
	Node this_bool_exp = bool_expression();
	if (!matchtoken(THEN)) {
		parse_error("if_statement error:lost then ");
	}
	else
		idindex++;
	Node this_stat1 = statement();
	Node this_stat2;
	backpatch(this_bool_exp.chain_true, this_stat1.codebegin);
	if (matchtoken(ELSE)) {
		idindex++;
		int temp_jump = nextstat;
		emit(JUMP, "0", "0", 0);
		this_stat2 = statement();
		backpatch(this_bool_exp.chain_false, this_stat2.codebegin);
		int temp_chain = merge(this_stat1.chain, temp_jump);
		S.chain = merge(temp_chain,this_stat2.chain);
	}
	else if (check_stat_end()) {
		S.chain = merge(this_bool_exp.chain_false, this_stat1.chain);
	}
	else {
		//parse_error("if_statement error:end error");
	}
	return S;
}
/*<while句> → while <布尔表达式> do <语句>*/
Node while_stmt() {
	idindex++;
	Node this_bool_exp = bool_expression();
	backpatch(this_bool_exp.chain_true, nextstat);
	if (matchtoken(DO))
		idindex++;
	else {
		parse_error("while_statement error :lost DO");
	}
	Node this_stat = statement();
	backpatch(this_stat.chain, this_bool_exp.codebegin);
	emit(JUMP, "0", "0", this_bool_exp.codebegin);
	Node S;
	S.codebegin = this_bool_exp.codebegin;
	S.chain = this_bool_exp.chain_false;
	return S;
}
/*<repeat句> → repeat <语句> until <布尔表达式>*/
Node repeat_statement() {
	idindex++;
	Node this_stat = statement();
	backpatch(this_stat.chain, nextstat); //回填
	if (matchtoken(UNTIL))
		idindex++;
	else {
		parse_error("repeat_statement error :lost UNTIL");
	}
	Node this_bool_exp = bool_expression();
	backpatch(this_bool_exp.chain_false, this_stat.codebegin);
	Node S;
	S.codebegin = this_stat.codebegin;
	S.chain = this_bool_exp.chain_true;
	return S;
}
/*<赋值句> → <标识符> := <算术表达式>*/
Node assign_statement() {
	Node S;
	S.codebegin = nextstat;
	string this_id = tokenv[idindex].values;
	if (symbol.count(this_id) == 0) {
		parse_error("assignment error:variable must be declaraced");
	}
	if (symbol[this_id] != INTEGER) {
		parse_error("assignment error:variable must be integer");
	}
	idindex++;
	if (matchtoken(ASSIGN))
		idindex++;
	else {
		parse_error("assignment error:must with\":=\"");
	}
	Node exp = arithmetic_expression();
	
	assign_form[this_id] = exp.place;
	S.chain = 0;
	emit(ASSIGN, exp.place, this_id, 0);
	return S;
}
/*<语句> → <赋值句>│<if句>│<while句>│<repeat句>│<复合句>*/
Node statement() {
	Node S;
	switch (tokenv[idindex].kid)
	{
	case 36:  //<赋值句>
		S = assign_statement();
		break;
	case 15:  //<if句>
		S = if_statement();
		break;
	case 34:  //<while句>
		S = while_stmt();
		break;
	case 26: //<repeat句>
		S = repeat_statement();
		break;
	case 3: //<复合句>
		S = compound_statement();
		break;
	default:
		parse_error("statement  error: statement begin with a wrong token");
		break;
	}
	return S;
}
/*<语句表> → <语句> ；<语句表>│<语句>*/
Node statement_form() {
	Node S;
	S = statement();
	if (matchtoken(SEMI)) {
		idindex++;
		backpatch(S.chain, nextstat);   //回填
		S = statement_form();                //新的语句表
	}
	else if (matchtoken(IF) || matchtoken(WHILE) || matchtoken(REPEAT) || matchtoken(ID) || matchtoken(BEGIN))
		parse_error("statement_form error: must end with \";\"");
	//else parse_error("statement_form error: must end with \"end\"");
	return S;
}
/*<复合句> → begin <语句表> end*/
Node compound_statement() {
	Node S;
	if (matchtoken(BEGIN))
		idindex++;
	else
		parse_error("compound_statement error: must begin with \"begin\"");
	S = statement_form();
	if (matchtoken(END))
		idindex++;
	else parse_error("compound_statement error: must end with\"end\"");
	return S;
}
/*<标识符表> → <标识符> (，<标识符表>│ε)*/
void id_form(vector<string> &t_symbol) {
	if (matchtoken(ID))    //<标识符>
		t_symbol.push_back(tokenv[idindex].values);
	else parse_error("identifier form error:must have id!");
	idindex++;
	if (matchtoken(COMMA)) {   //，
		idindex++;
		id_form(t_symbol);
	}
	//else if (matchtoken(COLON));
	//else parse_error("identifier form error:must end with :");
}
/*<变量定义> → <标识符表> ：<类型> (；<变量定义>│ε)*/
void variable_define() {
	vector<string> tmp_symbol;   //<标识符表> 
	id_form(tmp_symbol);
	if (matchtoken(COLON))       //：
		idindex++;
	else parse_error("variable_define error: must have \":\"");
	if (matchtoken(CHAR) || matchtoken(BOOL) || matchtoken(INTEGER)) {   //<类型> 
		for (size_t i = 0;i<tmp_symbol.size();i++) {
			symbol[tmp_symbol[i]] = tokenv[idindex].kid;
		}
		idindex++;
	}
	else parse_error("variable_define type error: must int,bool,char");
	if (matchtoken(SEMI)) {    //；
		idindex++;
		if (!matchtoken(BEGIN)) {   //<变量定义>|空
			variable_define();
		}
	}
	//else parse_error("variable_define error: must end with:\";\"");
}
/*<变量说明> → var <变量定义>│ε*/
void variable_declaration() {
	if (matchtoken(BEGIN)) {             //判断是否为空
	}
	else if (matchtoken(VAR)) {           //var <变量定义>
		idindex++;
		variable_define();                    //<变量定义>
	}
	else
		parse_error("variable declaration error :must begin with var or no make it");
}
/*<程序> → program <标识符> ；<变量说明> <复合语句>*/
void program_statement() {                     
	idindex = 0; 
	if (matchtoken(PROGRAM)) {            // program
		idindex++;
	}else
		parse_error("program must begin with key\"program\".");
	token start_program_id;
	if (matchtoken(ID)) {                 //<标识符>
		start_program_id = tokenv[idindex];
		idindex++;
	}else 
		parse_error("program must with id");
	if (matchtoken(SEMI)) {                 //；
		idindex++;
		emit(HEAD, start_program_id.values, "0", 0);
	}else
		parse_error("program must have ;");
	variable_declaration();                             //<变量说明> 
	Node S = compound_statement();                   //<复合语句>
	backpatch(S.chain, nextstat);           //回填Chain
	if (matchtoken(DOT)) {
		cout << "编译成功！！！" << endl;
	}
	else
		parse_error("program must end with .");
	emit(ENDFILE, "0", "0", 0);
}
void print_all_quaternion() {
	for (size_t i = 0;i < Quaternion_list.size();i++) {
		print_quaternion(Quaternion_list[i],i);
	}
}
void main(int argc, char * argv[]) {
	login_show();  //show creator
	init();
	string filename;
	if (argc != 2) {
		cout << "usage: "<<argv[0]<<"  <filename>\n\ndefaulf:.\\SampleTest.txt\n" ;
		filename = "SampleTest.txt";
	}
	else {
		filename = argv[1];
	}
	ifstream loadfile(filename, ios::out | ios::in);
	if (!loadfile) {
		perror("open error");
		exit(1);
	}
	while (loadfile.getline(str, N)) {
		line++;
		index = 0;
		while (str[index] != 0) {
			scanning();
		}
	}
	//output();
	token i;
	i.kid = 62;
	tokenv.push_back(i);
	program_statement();
	print_all_quaternion();
	//return 0;
}