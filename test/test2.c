#include<stdio.h>

int main()
{
    int i, n;
    scanf("%d", &n);
    printf("%d %d %d\n", 1, 1, n);
    for(i = 1; i <= n; i++)
    {
        printf("%d %d %d\n", 1, i%2 ? (i+1)/2 : n-(i-1)/2, i);
        if (i == n/2)
            printf("%d %d\n", 6, n);
    }
    printf("%d %d\n", 4, 1);
    for (i = 1; i <= n; i++)
    {
        printf("%d \n", 2);
    }
    printf("\n %d \n", 8);
}
