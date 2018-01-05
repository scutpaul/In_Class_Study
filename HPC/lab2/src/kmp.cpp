#include<iostream>
#include<fstream>
#include<cstring>
#include<cstdio>
#include<algorithm>
#include<string>
#include<ctime>

#define Max_N 110000000
#define N 10000

using namespace std;

int match[Max_N];
char Text[Max_N];
char Pattern[N];
int Next[N + 1];
int Newnext[N];
int maxprefixlen;
int m;
int cnt = 0;
int n;
int rank1;
int base;
clock_t start_time, end_time;

void built_next(char *P) {
	Next[1] = Newnext[1] = 0;
	int j = 2;
	while (j <= m + 1) {
		int i = Next[j - 1];
		while (i != 0 && P[i - 1] != P[j - 2]) {
			i = Next[i];
		}
		Next[j] = i + 1;
		if (j != m + 1) {
			if (P[j - 1] != P[i]) {
				Newnext[j] = i + 1;
			}
			else {
				Newnext[j] = Newnext[i + 1];
			}
		}
		j++;
	}
}
void kmp(char *T, char *P, int textlen, int matched_num, int *match) {
	int i = 1;
	int j = matched_num + 1;
	while (i <= textlen) {
		while (j != 0 && P[j - 1] != T[i - 1]) {
			j = Newnext[j];
		}
		if (j == m) {
			match[cnt++] = base + i - m;
			j = Next[m + 1];
			i++;
		}
		else {
			i++;
			j++;
		}
	}
	maxprefixlen = j - 1;
}

int main(int argc, char** argv) {
	/*Part0*/
	ifstream load_text("./kmp_text.txt", ios::in);
	ifstream load_pattern("./kmp_pattern.txt", ios::in);

	/*Part0*/
	rank1 = 0;
	base = rank1;
	load_text >> Text;
	load_pattern >> Pattern;
	load_text.close();
	load_pattern.close();
	start_time = clock();
	m = strlen(Pattern);
	built_next(Pattern);
	/*1-(p-1)*/
	n = strlen(Text);
	kmp(Text, Pattern, n, 0, match);
	end_time = clock();
	int running_time = end_time - start_time;
	cout << "***********����KMP*********" << endl;
	cout << "Text long:" << n << "     Pattern long:" << m << endl;
	cout << "running_time:" << running_time << "ms" << endl;
	cout << "ƥ��������" << cnt <<"   ƥ��λ�ã�"<< endl;
	for (int i = 0;i<cnt;i++) {
		cout << match[i] << ",";
	}
	cout << endl;
	return 0;
}

