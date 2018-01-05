#include<iostream>
#include<string>
#include<algorithm>
#include<fstream>
#include<cstdio>

using namespace std;

string pat;
string text;

int main(){
    int n,m;
    scanf("%d%d",&n,&m);
    ofstream f1,f2;
    f1.open("./kmp_text.txt");
    f2.open("./kmp_pattern.txt");
    for(int i=1;i<=n;i++){
        char c='a'+(rand()%26);
        text+=c;
    }
    for(int i=1;i<=m;i++){
    //    char c='a';
        char c='a'+(rand()%26);
        pat+=c;
    }
    f1<<text<<'\n';
    f2<<pat<<'\n';
    return 0;
}
