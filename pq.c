#include "pq.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int doubleGT(const void *a, const void *b);

/* Hash Map spec */
#define __DS__HM__NORMAL__        0x00000000
#define __DS__HM__OUT_OF_MEM__    0x00000001
#define __DS__HM__KEY_EXIST__     0x00000002
#define __DS__HM__KEY_NOT_EXIST__ 0x00000004
struct hm_t;

struct hm_t *hmAlloc();
int hash(int sz, void *obj);
int hmInit(struct hm_t *pThis, size_t objSize);
int hmFree(struct hm_t *pThis);
int hmSize(struct hm_t *pThis);
int hmInsert(struct hm_t *pThis, void *pObj);
int hmDelete(struct hm_t *pThis, void *pObj);
int hmKeyExist(struct hm_t *pThis, void *pKey);
int hmUnion(struct hm_t *pThis1, struct hm_t *pThis2);
void hmDebug(struct hm_t*);
/* Hash Map spec end */

inline char* getAddr(void *array, int i, size_t tSize){
	return array+i*tSize;
}
inline void getItem(void *array, int i, void* pRet, size_t tSize){
	memcpy(pRet, array+i*tSize, tSize);
}
inline void putItem(void *array, int i, const void* pRet, size_t tSize){
	memcpy(array+i*tSize, pRet, tSize);
}

/* Very Slow Priority Queue implementation */
struct pq_t{
    struct node *root;
    size_t keySize, objSize, size, cap;
    struct hm_t *pObjToIndex;
    int (*cmp)(const void*, const void*);
    int dynamic;
};

/* node struct */
struct node{
    struct node *left;
    struct node *right;
    struct node *parent;
    void *key;
    void *obj;
    size_t grade;
};

struct node * newNode(void *keyIn, size_t keySize, void *objIn, size_t objSize){
    struct node* new = malloc(sizeof(struct node));
    new->key = malloc(keySize);
    new->obj = malloc(objSize);
    memcpy(new->key, keyIn, keySize);
    memcpy(new->obj, objIn, objSize);
    new->left = NULL;
    new->right = NULL;
    new->parent = NULL;
    new->grade = 1;
    return new;
}

struct node *treeUnion(struct node* pThis1, struct node *pThis2, struct pq_t* pq_t){
    struct node *pq = NULL, *itr = NULL;
    if (pq_t->cmp(pThis1->key, pThis2->key) >= 0)
    {
        pq = pThis1;
    }
    else
    {
        pq = pThis2;
    }
    while (1)
    {
        if (pq_t->cmp(pThis1->key, pThis2->key) >= 0)
        {
            if (itr == NULL)
                itr = pThis1;
            else {
                itr->right = pThis1;
                itr->grade = pThis1->grade+1;
                pThis1->parent = itr;
                itr = itr->right;
            }
            if (itr->right == NULL)
            {
                itr->right = pThis2;
                itr->grade = pThis2->grade+1;
                pThis2->parent = itr;
                itr = itr->right;
                break;
            }
            else
            {
                pThis1 = itr->right;
            }
        }
        else
        {
            if (itr == NULL)
                itr = pThis2;
            else {
                itr->right = pThis2;
                itr->grade = pThis2->grade+1;
                pThis2->parent = itr;
                itr = itr->right;
            }
            if (itr->right == NULL)
            {
                itr->right = pThis1;
                itr->grade = pThis1->grade+1;
                pThis1->parent = itr;
                itr = itr->right;
                break;
            }
            else
            {
                pThis2 = itr->right;
            }
        }
    }
    while (itr->right != NULL)
        itr = itr->right;
    while (itr->parent != NULL)
    {
        itr = itr->parent;
        if (itr->left==NULL)
        {
            itr->left = itr->right;
            itr->right = NULL;
            itr->grade = 1;
        }
        else
        {
            if (itr->left->grade < itr->right->grade)
            {
                void *temp = itr->right;
                itr->right = itr->left;
                itr->grade = itr->right->grade+1;
                itr->left = temp;
            }
        }
    }
    return pq;
}
void destory(struct node *root){
    if (root == NULL)
        return;
    if (root->left != NULL)
        destory(root->left);
    if (root->right != NULL)
        destory(root->right);
    free(root->key);
    free(root->obj);
    free(root);
}
/*node opearend end*/
/*
void pqDebug(struct pq_t* pThis){
    printf("size/cap: %zu/%zu\nkey,obj size: %zu, %zu\n", pThis->size, pThis->cap, pThis->keySize, pThis->objSize);
    size_t i;
    printf("keyArray, objArray: %x, %x\n", pThis->keyArray, pThis->objArray);
    printf("key:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__DOUBLE__, pThis->keyArray + i*pThis->keySize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
    printf("obj:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__SHORT__, pThis->objArray + i*pThis->objSize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
    printf("pObjToIndex:\n");
    hmDebug(pThis->pObjToIndex);
}
*/

struct pq_t *pqAlloc(){
    return malloc(sizeof(struct pq_t));
}

int pqInit(struct pq_t *pThis, size_t keySize, size_t objSize, size_t cap, int (*cmp)(const void*, const void*)){
    if(cap != 0){
        pThis->dynamic = 0;
        pThis->cap = cap;
    }
    else
    {
        pThis->dynamic = 1;
        pThis->cap = 4;
    }
    pThis->root = NULL;
    pThis->keySize = keySize;
    pThis->objSize = objSize;
    pThis->size = 0;
    /*pThis->pObjToIndex = hmAlloc();*/
    pThis->cmp = cmp;
    /*hmInit(pThis->pObjToIndex, objSize);*/
    return __DS__PQ__NORMAL__;
}
int pqFree(struct pq_t *pThis){
    /*if(pThis->pObjToIndex != NULL)*/
        /*hmFree(pThis->pObjToIndex);*/
    if (pThis->root != NULL)
        destory(pThis->root);
    free(pThis);
}

size_t pqCap(struct pq_t *pThis){
    return pThis->cap;
}
size_t pqSize(struct pq_t *pThis){
    return pThis->size;
}
size_t pqKeySize(struct pq_t *pThis){
    return pThis->keySize;
}
size_t pqObjSize(struct pq_t *pThis){
    return pThis->objSize;
}
int pqEmpty(struct pq_t *pThis){
    return pThis->size == 0;
}

int pqInsert(struct pq_t *pThis, void *pKey, void *pObj){
    if(pThis->size == pThis->cap)
    {
        if (pThis->dynamic)
            pThis->cap *= 2;
        else
            return __DS__PQ__FULL__;
    }
    /*if(hmKeyExist(pThis->pObjToIndex, pObj))*/
        /*return __DS__PQ__OBJ_EXIST__;*/
    if (pThis->size == 0){
        pThis->root = newNode(pKey, pThis->keySize, pObj, pThis->objSize);
        /*hmInsert(pThis->pObjToIndex, pObj);*/
        pThis->size++;
        return __DS__PQ__NORMAL__;
    }
    struct node * new;
    new =  newNode(pKey, pThis->keySize, pObj, pThis->objSize);
    pThis->root = treeUnion(pThis->root, new, pThis);
    /*hmInsert(pThis->pObjToIndex, pObj);*/
    pThis->size++;
    return __DS__PQ__NORMAL__;
}
int pqExtractMax(struct pq_t *pThis, void *pRetKey, void *pRetObj){
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    memcpy(pRetKey, pThis->root->key, pThis->keySize);
    memcpy(pRetObj, pThis->root->obj, pThis->objSize);
    /*hmDelete(pThis->pObjToIndex, pThis->root->obj);*/
    struct node *left = pThis->root->left;
    struct node *right = pThis->root->right;
    free(pThis->root->key);
    free(pThis->root->obj);
    free(pThis->root);
    if (right == NULL){
        if (left == NULL){
            pThis->size--;
            return __DS__PQ__NORMAL__;
        }
        pThis->root = left;
    }
    else{
        right->parent = NULL;
        left->parent = NULL;
        pThis->root = treeUnion(left, right, pThis);
    }
    pThis->size--;
    return __DS__PQ__NORMAL__;
}

int pqMax(struct pq_t *pThis, void *pRetKey, void *pRetObj){
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    memcpy(pRetKey, pThis->root->key, pThis->keySize);
    memcpy(pRetObj, pThis->root->obj, pThis->objSize);
    return __DS__PQ__NORMAL__;
}

int pqUnion(struct pq_t *pThis1, struct pq_t *pThis2){
    if(pThis2->size == 0)
        return __DS__PQ__NORMAL__;
    int isSamePQ = (pThis1->keySize == pThis2->keySize) || (pThis1->objSize == pThis2->objSize);
    if(!isSamePQ)
        return __DS__PQ__DIFF_SIZE__;
    if(!pThis1->dynamic && (pThis1->size + pThis2->size > pThis1->cap))
        return __DS__PQ__FULL__;
    while ( pThis1->cap < (pThis1->size+pThis2->size))
        pThis1->cap *= 2;
    pThis1->root = treeUnion(pThis1->root, pThis2->root, pThis1);
    pThis2->root = NULL;
    pThis1->size += pThis2->size;
    /*hmUnion(pThis1->pObjToIndex, pThis2->pObjToIndex);*/
    return __DS__PQ__NORMAL__;

}

/* Very Slow Hash Map Implementation */
#define MIN_HASH_CAP 1024
struct hm_t{
    struct h_node **objlist;
    struct h_node **objend;
    size_t size, objSize;
};

struct hm_t *hmAlloc(){
    return malloc(sizeof(struct hm_t));
}

struct h_node{
    void *obj;
    struct h_node* next;
};

/*
void hmDebug(struct hm_t* pThis){
    printf("size/cap: %zu/%zu\nkey,val size: %zu, %zu\n", pThis->size, pThis->cap, pThis->keySize, pThis->keySize);
    size_t i;
    printf("key:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__SHORT__, pThis->keyArray + i*pThis->keySize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
    printf("val:");
    for(i=0; i<pThis->size; i++){
        printData(__DS__SHORT__, pThis->valArray + i*pThis->valSize);
        printf("%c", (i!=(pThis->size-1))?' ':'\n');
    }
}
*/
int hash(int sz, void *obj)
{
    short i = *( ((char *)obj) );
    return (int)(i&(MIN_HASH_CAP-1));
}

struct h_node *new_h_node(void *obj, size_t size){
    struct h_node *new = malloc(sizeof(struct h_node));
    new->obj = malloc(size);
    memcpy(new->obj, obj, size);
    new->next = NULL;
    return new;
}
int hmInit(struct hm_t *pThis, size_t objSize){
    pThis->size = 0;
    pThis->objSize = objSize;
    pThis->objlist = malloc(sizeof(struct h_node *)*MIN_HASH_CAP);
    pThis->objend = malloc(sizeof(struct h_node *)*MIN_HASH_CAP);
    if (pThis->objlist == NULL || pThis->objend == NULL)
    {
        return __DS__HM__OUT_OF_MEM__;
    }
    int i;
    for (i = 0; i < MIN_HASH_CAP; i++)
    {
        pThis->objlist[i] = NULL;
        pThis->objend[i] = NULL;
    }
    return __DS__HM__NORMAL__;
}
int hmFree(struct hm_t *pThis){
    if (pThis->objlist)
    {
        int i;
        for (i = 0; i < MIN_HASH_CAP; i++)
        {
            if (pThis->objlist[i])
                free(pThis->objlist[i]);
        }
        free(pThis->objlist);
    }
    free(pThis);
}

int hmSize(struct hm_t *pThis){
    return pThis->size;
}
int hmInsert(struct hm_t *pThis, void *pObj){
    if(hmKeyExist(pThis, pObj))
        return __DS__HM__KEY_EXIST__;
    int val = hash(pThis->objSize, pObj);
    if (pThis->objend[val] == NULL)
    {
        pThis->objend[val] = pThis->objlist[val] = new_h_node(pObj, pThis->objSize);
    }
    else
    {
        pThis->objend[val]->next = new_h_node(pObj, pThis->objSize);
        pThis->objend[val] = pThis->objend[val]->next;
    }
    pThis->size++;
    return __DS__HM__NORMAL__;
}
int hmDelete(struct hm_t *pThis, void *pObj){
    int val = hash(pThis->objSize, pObj);
    struct h_node* itr = pThis->objlist[val];
    while (1)
    {
        if (memcmp(itr->obj, pObj, pThis->size) == 0)
        {
            free(itr);
            itr = NULL;
            return __DS__HM__NORMAL__;
        }
        if (itr->next == NULL)
            return __DS__HM__KEY_NOT_EXIST__;
        if (memcmp(itr->next->obj, pObj, pThis->size) == 0)
        {
            struct h_node *temp = itr->next;
            itr->next = itr->next->next;
            free(temp);
            return __DS__HM__NORMAL__;
        }
        itr = itr->next;
    }

}
int hmKeyExist(struct hm_t *pThis, void *pObj){
    int val = hash(pThis->objSize, pObj);
    struct h_node* itr = pThis->objlist[val];
    while (1)
    {
        if (itr == NULL)
            return 0;
        if (memcmp(itr->obj, pObj, pThis->size) == 0)
            return 1;
        if (itr->next == NULL)
            return 0;
        if (memcmp(itr->next->obj, pObj, pThis->size == 0))
            return 1;
        itr = itr->next;
    }
}
int hmUnion(struct hm_t *pThis1, struct hm_t *pThis2)
{
    int i;
    for(i = 0; i < MIN_HASH_CAP; i++)
    {
        if (pThis1->objend[i] != NULL || pThis2->objlist[i] != NULL)
        {
            if (pThis1->objend[i] == NULL)
            {
                pThis1->objlist[i] = pThis2->objlist[i];
                pThis1->objend[i] = pThis2->objend[i];
            }
            if (pThis2->objlist[i] == NULL)
                continue;
            pThis1->objend[i]->next = pThis2->objlist[i];
            pThis1->objend[i] = pThis2->objend[i];
        }
    }
    pThis2->objlist = pThis2->objend = NULL;
    return __DS__HM__NORMAL__;
}
/* Hash Map Implementation End */
