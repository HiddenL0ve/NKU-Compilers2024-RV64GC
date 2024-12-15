#include<stdio.h>
int main()
 {
    int x = 0;
    int n = 11;
    int i = 0, S = 0;
    while(i < n){
    if(i==10){
    S = S*2;
    }
    S = S + 2;
    i = i + 1;
    }
    printf("%d",S);
    return S;
 }