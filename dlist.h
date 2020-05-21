#ifndef DLIST_H
#define DLIST_H


#include <stdlib.h>
#include <stdio.h>

struct node {
    struct node * pre;
    struct node * next;
    double data;
};
struct DList {
    struct node * head;
    struct node * tail;
    int len;
};

DList * CreateList();
void DelList(DList * list);
void InsertList(DList * list, double data);
void ChangeData(DList * list, double data);
#endif // DLIST_H
