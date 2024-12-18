#ifndef AVL_TREE_H
#define AVL_TREE_H

#include <stdio.h>

typedef struct Node {
    char* id;
    double capacity;
    double consumption;
    int height;
    struct Node* left;
    struct Node* right;
} Node;

Node* insert(Node* node, const char* id, double capacity, double consumption);
void inorderTraversal(Node* root, FILE* output);
void freeTree(Node* root);

#endif
