#include<iostream>
#include<iomanip>
#include<fstream>   //oftream
#include<string>
#include<map>
#include<vector>
#include<algorithm>
#define N 100
#define token_N 1000
#define Default_char 20
using namespace std;
enum type {skey,id,singlebound,doublebound,conststring,constnumber};
string key[35] = { "and","array","begin","bool","call","case","char","constant","dim","do","else","end","false","for",
							"if","input","integer","not","of","or","output","procedure","program","read","real","repeat","set","stop",
							"then","to","true","until","var","while","write" };
struct token {
	type kind;
	string values;
	int kid;
};
vector<token> tokenv;
map<char, int>single;
map<char, int>doubleb;
map<string, int>idmap;
map<string, int>stringmap;
int idindex;
int iserror;
char str[N];
int index;
int line;

void error_show(int kind) {
	if (kind == 1) {  //note_error
		cout << "line:" << line << "note without \"* / \"" << endl;
	}
	else if (kind == 2) {
		cout << "line:" << line << "string without end\'" << endl;
	}
	else if (kind == 3) {
		cout << "line:" << line << "   index:" << index + 1 << "出现非法字符！" << endl;
	}

	exit(0);
}
void delete_space() {
	while (str[index] == ' ') {
		index++;
	}
}
void scan_note() {/*note*/
	if (str[index] == '/'&& str[index+1] == '*') {
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
	tokenv.push_back(i);
}
void scan_id_key() {
	int id_start = index;
	token i;
	while (str[index] >= 'a'&&str[index] <= 'z' || str[index] >= 'A'&&str[index] <= 'Z' || str[index] >= '0'&&str[index] <= '9') {
		index+=1;
	}
	string s(&str[id_start], &str[index]);
	//cout << s;
	if (lower_bound(key, key+35, s)!=key+35 && s == *lower_bound(key, key+35, s)) {
		i.kind = skey;
		i.kid = lower_bound(key, key + 35, s) - key+1;
		//cout << i.kid << endl;
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
	tokenv.push_back(i);

}
void scan_singlebound() {
	token i;
	i.kind = singlebound;
	i.kid = single[str[index]];
	tokenv.push_back(i);
	index++;
}
void scan_doublebound() {
	bool single_double = false;
	int kid = -1;
	token i;
	if (str[index] == '*'&&str[index + 1] == '/'){
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
		tokenv.push_back(i);
		index++;
		return;
	}
	
	i.kind = doublebound;
	i.kid = kid;
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
		if (tokenv[i].kid==36)
			cout << "(36," << idmap[tokenv[i].values] << ") ";
		else if(tokenv[i].kid == 37)
			cout << "(37," << idmap[tokenv[i].values] << ") ";
		else if (tokenv[i].kid == 38)
			cout << "(38," << stringmap[tokenv[i].values] << ") ";
		else
			cout << "("<<tokenv[i].kid<<",-) ";
		if ((i + 1) % 5 == 0)cout << endl;
	}
}
void main(int argc, char * argv[]) {
	login_show();  //show creator
	init();
	string filename;
	if (argc != 2) {
		cout << "usage: " << argv[0] << "  <filename>\n\ndefaulf:.\\SampleTest.txt\n";
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
		//scanning();
		line++;
		index = 0;
		while (str[index] != 0) {
			scanning();
		}
	}
	output();
	//return 0;
}