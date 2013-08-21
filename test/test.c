#include<stdio.h>

int main()
{
    int i, n;
    scanf("%d", &n);
    printf("%d %d %d\n", 1, 1, n);
    for(i = 1; i <= n; i++)
    {
        printf("%d %d %d\n", 1, i%2 ? (i+1)/2 : n-(i-1)/2, i);
    }
    for (i = 1; i <= n; i++)
    {
        printf("%d %s", 2, (i%10 == 0)? "\n":"");
    }
    printf("%d\n", 8);
}
