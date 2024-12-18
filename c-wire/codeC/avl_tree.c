#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl_tree.h"

static int max(int a, int b) {
    return (a > b) ? a : b;
}

static int height(Node* N) {
    if (N == NULL)
        return 0;
    return N->height;
}

static Node* newNode(const char* id, double capacity, double consumption) {
    Node* node = (Node*)malloc(sizeof(Node));
    if(!node) return NULL;
    node->id = strdup(id);
    node->capacity = capacity;
    node->consumption = consumption;
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    return node;
}

static Node* rightRotate(Node* y) {
    Node* x = y->left;
    Node* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

static Node* leftRotate(Node* x) {
    Node* y = x->right;
    Node* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

static int getBalance(Node* N) {
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

Node* insert(Node* node, const char* id, double capacity, double consumption) {
    if (id == NULL || strlen(id) == 0)
        return node;

    if (node == NULL)
        return newNode(id, capacity, consumption);

    int cmp = strcmp(id, node->id);
    if (cmp == 0) {
        // MàJ capacité si supérieure
        if (capacity > node->capacity) {
            node->capacity = capacity; 
        }
        // On additionne la consommation
        node->consumption += consumption;
    } else if (cmp < 0) {
        node->left = insert(node->left, id, capacity, consumption);
    } else {
        node->right = insert(node->right, id, capacity, consumption);
    }

    node->height = 1 + max(height(node->left), height(node->right));
    int balance = getBalance(node);

    // Rotations AVL
    if (balance > 1 && strcmp(id, node->left->id) < 0)
        return rightRotate(node);

    if (balance < -1 && strcmp(id, node->right->id) > 0)
        return leftRotate(node);

    if (balance > 1 && strcmp(id, node->left->id) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    if (balance < -1 && strcmp(id, node->right->id) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

void inorderTraversal(Node* root, FILE* output) {
    if (root == NULL)
        return;
    inorderTraversal(root->left, output);
    fprintf(output, "%s:%.0f:%.0f\n", root->id, root->capacity, root->consumption);
    inorderTraversal(root->right, output);
}

void freeTree(Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root->id);
    free(root);
}
