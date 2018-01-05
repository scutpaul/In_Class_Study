#include<iostream>
#include<fstream>
#include<cstring>
#include<cstdio>
#include<algorithm>
#include<string>
#include<set>
#include<mpi.h>
#include<windows.h>
#include<ctime>

#define Max_N 110000000
#define N 100

using namespace std;
char Text[Max_N];
char Pattern[N];
int Next[N + 1];
int Newnext[N];
int maxprefixlen;
int match[Max_N];
int buf[Max_N];

int m;
int n;
int task;
int cnt = 0;
clock_t start_time, end_time;
int base;


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
void kmp(char *T, char *P, int textlen, int matched_num) {
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
	int rank, p, tag = 1;
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	memset(Pattern, 0, sizeof(Pattern));
	/*Part0*/
	if (rank == 0) {
		ifstream load_text("./kmp_text.txt", ios::in);
		ifstream load_pattern("./kmp_pattern.txt", ios::in);
		load_text >> Text;
		load_pattern >> Pattern;
		load_text.close();
		load_pattern.close();
		start_time = clock();
		m = strlen(Pattern);
		built_next(Pattern);
		n = strlen(Text);
		task = (int)(n + p - 1) / p;   //间隔
		for (int i = n; i < p*task; i++) { Text[i] = '%'; }
		Text[task*p] = '\0';
		for (int i = 1; i < p; i++) {
			MPI_Send(&task, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			MPI_Send(&m, 1, MPI_INT, i, tag, MPI_COMM_WORLD);
			MPI_Send(Pattern, m, MPI_CHAR, i, tag, MPI_COMM_WORLD);
			MPI_Send(Next, m + 2, MPI_INT, i, tag, MPI_COMM_WORLD);
			MPI_Send(Newnext, m+1, MPI_INT, i, tag, MPI_COMM_WORLD);
			MPI_Send(Text + i*task, task, MPI_CHAR, i, tag, MPI_COMM_WORLD);
		}
	}
	for (int i = 1; i < p; i++) {
		if (rank == i) {
			MPI_Recv(&task, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&m, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&Pattern, m, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&Next, m + 2, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&Newnext, m+1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&Text, task, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
		}
	}
	//void kmp(char *T,char *P,int textlen,int matched_num,int *match)
	for (int i = 0; i < p; i++) {
		if (rank == i) {
			base = i*task;
			kmp(Text, Pattern, task, 0);
		}
	}
	for (int i = 0; i < p - 1; i++) {
		if (rank == i) {
			MPI_Send(&maxprefixlen, 1, MPI_INT, i + 1, tag, MPI_COMM_WORLD);
		}
	}

	for (int i = 1; i < p; i++) {
		if (rank == i) {
			MPI_Recv(&maxprefixlen, 1, MPI_INT, i - 1, tag, MPI_COMM_WORLD, &status);
			if (maxprefixlen != 0)
				kmp(Text, Pattern, m - 1, maxprefixlen);
		}
	}
	tag++;
	for (int i = 1; i < p; i++) {
		if (rank == i) {
			MPI_Send(&cnt, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
			MPI_Send(match, cnt, MPI_INT, 0, tag, MPI_COMM_WORLD);
		}
	}

	if (rank == 0) {
		cout << rank << endl;
		int num;
		for (int i = 1; i < p; i++) {
			MPI_Recv(&num, 1, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(buf, num, MPI_INT, i, tag, MPI_COMM_WORLD, &status);
			for (int j = 0; j < num; j++) {
				match[cnt++] = buf[j];
			}
		}
		end_time = clock();
	}

	if (rank == 0) {
		int running_time = end_time - start_time;
		cout << "***********并行KMP*********" << endl;
		cout << "Text long:" << n << "     Pattern long:" << m << endl;
		cout << "running_time:" << running_time <<"ms"<< endl;
		cout << "匹配数量：" << cnt << "   匹配位置：" << endl;
		set<int> ss;
		for (int i = 0; i < cnt; i++) {
			ss.insert(match[i]);
		}
		set<int>::iterator it;
		for (it = ss.begin(); it != ss.end(); it++) {
			cout << (*it) << ",";
			//for (int i = *it; i < *it + m; i++) cout << Text[i];
		}
		cout << endl;
	}
	MPI_Finalize();
	return 0;
}