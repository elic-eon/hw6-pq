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
int hmInit(struct hm_t *pThis, size_t keySize, size_t valSize);
int hmFree(struct hm_t *pThis);

int hmSize(struct hm_t *pThis);
int hmInsert(struct hm_t *pThis, void *pKey, void *pVal);
int hmDelete(struct hm_t *pThis, void *pKey);
int hmGet(struct hm_t *pThis, void *pKey, void *pRetVal);
int hmSet(struct hm_t *pThis, void *pKey, void *pNewVal);
int hmKeyExist(struct hm_t *pThis, void *pKey);
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
    pThis->pObjToIndex = hmAlloc();
    pThis->cmp = cmp;
    hmInit(pThis->pObjToIndex, objSize, sizeof(size_t));
    return __DS__PQ__NORMAL__;
}

int pqFree(struct pq_t *pThis){
    if(pThis->pObjToIndex != NULL)
        hmFree(pThis->pObjToIndex);
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
    if(hmKeyExist(pThis->pObjToIndex, pObj))
        return __DS__PQ__OBJ_EXIST__;
    if (pThis->size == 0){
        pThis->root = newNode(pKey, pThis->keySize, pObj, pThis->objSize);
        pThis->size++;
        return __DS__PQ__NORMAL__;
    }
    struct node * new;
    new =  newNode(pKey, pThis->keySize, pObj, pThis->objSize);
    pThis->root = treeUnion(pThis->root, new, pThis);
    hmInsert(pThis->pObjToIndex, pObj, &(pThis->size));
    pThis->size++;
    return __DS__PQ__NORMAL__;
}
int pqExtractMax(struct pq_t *pThis, void *pRetKey, void *pRetObj){
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    memcpy(pRetKey, pThis->root->key, pThis->keySize);
    memcpy(pRetObj, pThis->root->obj, pThis->objSize);
    hmDelete(pThis->pObjToIndex, pThis->root);
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
    /*
    size_t size = pThis1->size;
    size_t i;
    for(i=0; i<pThis2->size; i++){
        void *pObj = getAddr(pThis2->objArray, i, pThis2->objSize);
        size_t newIndex = i+size;
        if(hmInsert(pThis1->pObjToIndex, pObj, &newIndex)== __DS__HM__KEY_EXIST__){
            size_t j;
            for(j=0; j<i; j++){
                pObj = getAddr(pThis2->objArray, j, pThis2->objSize);
                hmDelete(pThis1->pObjToIndex, pObj);
            }
            return __DS__PQ__OBJ_EXIST__;
        }
    }
    for(i=0; i<pThis2->size; i++){
        size_t newIndex = i+size;
        void *pObj = getAddr(pThis2->objArray, i, pThis2->objSize);
        void *pKey = getAddr(pThis2->keyArray, i, pThis2->keySize);
        void *pNewObj = getAddr(pThis1->objArray, newIndex, pThis1->objSize);
        void *pNewKey = getAddr(pThis1->keyArray, newIndex, pThis1->keySize);
        memcpy(pNewObj, pObj, pThis1->objSize);
        memcpy(pNewKey, pKey, pThis1->keySize);
    }
    */
    pThis1->root = treeUnion(pThis1->root, pThis2->root, pThis1);
    pThis2->root = NULL;
    pThis1->size += pThis2->size;
    return __DS__PQ__NORMAL__;

}

/* bonus2 */
int pqExtractMin(struct pq_t *pThis, void *pRetKey, void *pRetObj){
  /*
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    void *min = pThis->keyArray, *iter;
    size_t min_i = 0, i;
    for(i=1; i<pThis->size; i++){

        iter = getAddr(pThis->keyArray, i, pThis->keySize);
        if((*(pThis->cmp))(min, iter) > 0){
            min = iter;
            min_i = i;
        }
    }
    getItem(pThis->keyArray, min_i, pRetKey, pThis->keySize);
    getItem(pThis->objArray, min_i, pRetObj, pThis->objSize);

    for(i=min_i; i<pThis->size-1; i++){
        void *iter1 = getAddr(pThis->keyArray, i, pThis->keySize);
        void *iter2 = getAddr(pThis->keyArray, i+1, pThis->keySize);
        memcpy(iter1, iter2, pThis->keySize);
        iter1 = getAddr(pThis->objArray, i, pThis->objSize);
        iter2 = getAddr(pThis->objArray, i+1, pThis->objSize);
        memcpy(iter1, iter2, pThis->objSize);
        hmSet(pThis->pObjToIndex, iter1, &i);
    }
    hmDelete(pThis->pObjToIndex, pRetObj);
    pThis->size--;
    */
    return __DS__PQ__NORMAL__;
}
int pqMin(struct pq_t *pThis, void *pRetKey, void *pRetObj){
  /*
    if(pThis->size == 0)
        return __DS__PQ__EMPTY__;
    void *min = pThis->keyArray, *iter;
    size_t min_i = 0, i;
    for(i=1; i<pThis->size; i++){

        iter = getAddr(pThis->keyArray, i, pThis->keySize);
        if((*(pThis->cmp))(min, iter) > 0){
            min = iter;
            min_i = i;
        }
    }
    getItem(pThis->keyArray, min_i, pRetKey, pThis->keySize);
    getItem(pThis->objArray, min_i, pRetObj, pThis->objSize);

    */
    return __DS__PQ__NORMAL__;
}

/* bonus3 */
int pqGetKey(struct pq_t *pThis, void *pObj, void *pRetKey){
  /*
    size_t index;
    if(hmGet(pThis->pObjToIndex, pObj, &index) == __DS__HM__KEY_NOT_EXIST__ )
        return __DS__PQ__OBJ_NOT_EXIST__;
    getItem(pThis->keyArray, index, pRetKey, pThis->keySize);
    */
    return __DS__PQ__NORMAL__;
}

int pqChangeKey(struct pq_t *pThis, void *pObj, void *pNewKey){
  /*
    size_t index;
    if(hmGet(pThis->pObjToIndex, pObj, &index) == __DS__HM__KEY_NOT_EXIST__)
        return __DS__PQ__OBJ_NOT_EXIST__;
    putItem(pThis->keyArray, index, pNewKey, pThis->keySize);
    */
    return __DS__PQ__NORMAL__;
}

/* Very Slow Hash Map Implementation */
#define MIN_HASH_CAP 8
struct hm_t{
    void *keyArray;
    void *valArray;
    size_t size, cap, keySize, valSize;
};

struct hm_t *hmAlloc(){
    return malloc(sizeof(struct hm_t));
}

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

int hmInit(struct hm_t *pThis, size_t keySize, size_t valSize){
    pThis->size = 0;
    pThis->cap = MIN_HASH_CAP;
    pThis->keySize = keySize;
    pThis->valSize = valSize;

    pThis->keyArray = malloc(pThis->keySize*pThis->cap);
    if(pThis->keyArray == NULL)
        return __DS__HM__OUT_OF_MEM__;
    pThis->valArray = malloc(pThis->valSize*pThis->cap);
    if(pThis->valArray == NULL){
        free(pThis->keyArray);
        return __DS__HM__OUT_OF_MEM__;
    }
}
int hmFree(struct hm_t *pThis){
    if(pThis->keyArray)
        free(pThis->keyArray);
    if(pThis->valArray)
        free(pThis->valArray);
    free(pThis);
}

int hmExpand(struct hm_t *pThis){
    if(pThis->size == pThis->cap){
        pThis->cap = 2*pThis->cap;

        pThis->keyArray = realloc(pThis->keyArray, pThis->keySize*pThis->cap);
        if(pThis->keyArray == NULL)
            return __DS__HM__OUT_OF_MEM__;
        pThis->valArray = realloc(pThis->valArray, pThis->valSize*pThis->cap);
        if(pThis->valArray == NULL){
            free(pThis->keyArray);
            return __DS__HM__OUT_OF_MEM__;
        }
    }
    return __DS__HM__NORMAL__;
}
int hmShrink(struct hm_t *pThis){
    if(pThis->size*4 <= pThis->cap && pThis->size > MIN_HASH_CAP){
        pThis->cap = pThis->cap/2;

        pThis->keyArray = realloc(pThis->keyArray, pThis->keySize*pThis->cap);
        if(pThis->keyArray == NULL)
            return __DS__HM__OUT_OF_MEM__;
        pThis->valArray = realloc(pThis->valArray, pThis->valSize*pThis->cap);
        if(pThis->valArray == NULL){
            free(pThis->keyArray);
            return __DS__HM__OUT_OF_MEM__;
        }
    }
    return __DS__HM__NORMAL__;
}
int hmSize(struct hm_t *pThis){
    return pThis->size;
}
int hmInsert(struct hm_t *pThis, void *pKey, void *pVal){
    if(hmKeyExist(pThis, pKey))
        return __DS__HM__KEY_EXIST__;
    if(pThis->size == pThis->cap)
        if(hmExpand(pThis) == __DS__HM__OUT_OF_MEM__ )
            return __DS__HM__OUT_OF_MEM__;
    memcpy(pThis->keyArray + pThis->size*pThis->keySize, pKey, pThis->keySize);
    memcpy(pThis->valArray + pThis->size*pThis->valSize, pVal, pThis->valSize);
    pThis->size++;
    return __DS__HM__NORMAL__;
}
int hmFindKey(struct hm_t *pThis, void *pKey){
    int i;
    for(i=0; i<pThis->size; i++){
        void *iter = pThis->keyArray + i*pThis->keySize;
        if(memcmp(iter, pKey, pThis->keySize) == 0)
            break;
    }
    return i;
}
int hmDelete(struct hm_t *pThis, void *pKey){
    int i = hmFindKey(pThis, pKey);
    if(i == pThis->size)
        return __DS__HM__KEY_NOT_EXIST__;
    if(i != pThis->size-1){
        void *leftEnd = pThis->keyArray + i*pThis->keySize;
        void *rightStart = pThis->keyArray + (i+1)*pThis->keySize;
        size_t rightSize = pThis->keySize*(pThis->size-(i+1));
        memmove(leftEnd, rightStart, rightSize);

        leftEnd = pThis->valArray + i*pThis->valSize;
        rightStart = pThis->valArray + (i+1)*pThis->valSize;
        rightSize = pThis->valSize*(pThis->size-(i+1));
        memmove(leftEnd, rightStart, rightSize);
    }
    pThis->size--;
    if(pThis->size*4 <= pThis->cap && pThis->size > MIN_HASH_CAP)
        if(hmShrink(pThis) == __DS__HM__OUT_OF_MEM__ )
            return __DS__HM__OUT_OF_MEM__;

    return __DS__HM__NORMAL__;

}
int hmGet(struct hm_t *pThis, void *pKey, void *pRetVal){
    int i = hmFindKey(pThis, pKey);
    if(i == pThis->size)
        return __DS__HM__KEY_NOT_EXIST__;
    memcpy(pRetVal, pThis->valArray + i*pThis->valSize, pThis->valSize);
    return __DS__HM__NORMAL__;
}
int hmSet(struct hm_t *pThis, void *pKey, void *pNewVal){
    int i = hmFindKey(pThis, pKey);
    if(i == pThis->size)
        return __DS__HM__KEY_NOT_EXIST__;
    memcpy(pThis->valArray + i*pThis->valSize, pNewVal, pThis->valSize);
    return __DS__HM__NORMAL__;
}
int hmKeyExist(struct hm_t *pThis, void *pKey){
    int i = hmFindKey(pThis, pKey);
    if(i == pThis->size)
        return 0;
    return 1;
}
/* Hash Map Implementation End */
