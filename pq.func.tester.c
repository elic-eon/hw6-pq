#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"pq.h"
#include"utility.h"

typedef struct pq_t pq;

int doubleGT(const void *a, const void *b)
{
	double da=*(double*)a;
	double db=*(double*)b;
    // printf("da, db: %f, %f\n", da, db);
	if(da==db)return 0;
	return (da>db)?1:-1;
}

int shortGT(const void *a, const void *b)
{
	short da=*(short*)a;
	short db=*(short*)b;
	if(da==db)return 0;
	return (da>db)?1:-1;
}

int main()
{
	struct pq_t *p1=NULL, *p2=NULL;
	char cmd[10240];
	size_t cap, keySize, objSize;
	int ret;
	void *data1, *data2;
	int opChoice, pqChoice = 1;
    struct pq_t *now_pq;
    now_pq = p1;
	int keyType, objType;
    int (*cmp)(const void *, const void*);
	data1=malloc(sizeof(double)*10);
	data2=malloc(sizeof(double)*10);
	for(;;)
	{
		if(now_pq==NULL)
		{
			printf("The pq%d needs to be initialized.\n", pqChoice);
            if(pqChoice == 1){
                printf("Init step 1. key type: d for double, others for short:\n");
                scanf("%s",cmd);
                keyType=(cmd[0]=='d')?__DS__DOUBLE__:__DS__SHORT__;
                printf("Init step 2. object type: d for double, others for short:\n");
                scanf("%s",cmd);
                objType=(cmd[0]=='d')?__DS__DOUBLE__:__DS__SHORT__;
            }
            else
                printf("pq2 has same key/object size and cmp as pq1\n");

			printf("Init step 3. Capacity: ");
			scanf("%zu",&cap);
            if(pqChoice == 1)
                now_pq = p1 = pqAlloc();
            else
                now_pq = p2 = pqAlloc();

			if(keyType == __DS__DOUBLE__){
                keySize = sizeof(double);
                cmp = doubleGT; 
            }
            else{
                keySize = sizeof(short);
                cmp = shortGT;
            }
			if(objType == __DS__DOUBLE__)
                objSize = sizeof(double);
            else
                objSize = sizeof(short);

            ret=pqInit(now_pq, keySize, objSize, cap, cmp);
			if(ret)
			{
				puts("Not enough memory.");
				pqFree(now_pq);
				now_pq=NULL;
                if(pqChoice == 1)
                    p1 = NULL;
                else{
                    p2 = NULL;
                    now_pq = p1;
                    pqChoice = 1;
                }
			}
			else
				puts("Initialization done.");
		}
		else
		{
            // printf("now_pq:%x, pq1:%x, pq2:%x\n", now_pq, p1, p2);
			printf("size/capacity: %zu/%zu\n", pqSize(now_pq), pqCap(now_pq));
			printf("now is in Priority Queue %d:\n", pqChoice);
			size_t keySize=pqKeySize(now_pq);
			size_t objSize=pqObjSize(now_pq);
			printf("Valid operations: 1)insert, 2)extract max, 3)max, 4)union, 5)empty,\n");
            printf("                  6)switch to another pq, 7)free 2 pqs, 8)quit\n");
			for(opChoice=0;opChoice<=0||opChoice>8;sscanf(cmd,"%d",&opChoice))
				scanf("%s",cmd);
			if(opChoice==1)
			{
				printf("Input a %s value for key: ",keySize==sizeof(double)?"double":"short");
				getData(keyType,data1);
				printf("Input a %s value for obj: ",objSize==sizeof(double)?"double":"short");
				getData(objType,data2);
				ret=pqInsert(now_pq, data1, data2);
				if(ret == __DS__PQ__FULL__) printf("The pq%d is full!\n", pqChoice);
				else if(ret == __DS__PQ__OBJ_EXIST__){ 
                    printf("The object ");
                    printData(objType, data2);
                    printf(" is in the pq%d now!\n", pqChoice);
                }
			}
			else if(opChoice>=2 && opChoice<=3)
			{
                if(opChoice == 2) ret=pqExtractMax(now_pq, data1, data2);
                else if(opChoice == 3) ret=pqMax(now_pq, data1, data2);
                
                // printf("pq op!\n");
				if(!ret)
				{
					printf("The result is <");
					printData(keyType, data1);
                    printf(", ");
					printData(objType, data2);
					puts(">.");
				}
				else
					printf("The pq%d is empty!\n", pqChoice);
			}
            else if(opChoice==4){
                if(p2 == NULL){
                    printf("pq2 is empty. you can't union it.\n");
                }
                else{
                    printf("Choose you want to union(p1, p2) or union(p2, p1).\n");
                    printf("Input 1 for former, else for latter.\n");
                    char cmd2[1024] = {0};
                    int choice;
                    scanf("%s", cmd2);
                    choice = (cmd2[0]=='1')?1:2;
                    if(choice == 1){
                        ret = pqUnion(p1, p2);
                    }
                    else{
                        ret = pqUnion(p2, p1);
                        if(ret == __DS__PQ__NORMAL__){
                            p1 = p2;
                        }
                    }
                    if(ret == __DS__PQ__NORMAL__){
                        p2 = NULL;
                        pqChoice = 1;
                        now_pq = p1;
                    }
                    else if(ret == __DS__PQ__FULL__)
                        printf("pq1 doesn't have enough place to merge pq2.\n");
                    else if(ret == __DS__PQ__DIFF_SIZE__)
                        printf("You can union two different type priority queues.\n");
                }
            }
			else if(opChoice==5)
			{
				if(pqEmpty(now_pq)) printf("The pq%d is empty.\n", pqChoice);
				else printf("The pq%d is not empty.\n", pqChoice);
			}
            else if(opChoice==6){
                pqChoice = (pqChoice==1)?2:1;
                now_pq = (pqChoice==1)?p1:p2;
            }
			else if(opChoice==7)
			{
                if(p2 != NULL)
                    pqFree(p2);
                if(p1 != NULL)
                    pqFree(p1);
				p1 = p2 = NULL;
                pqChoice = 1;
                now_pq = p1;
			}
			else
				break;
		}
	}
    free(data1);
    free(data2);
	return 0;
}
