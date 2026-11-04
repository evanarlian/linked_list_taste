#pragma once
#include <stdbool.h>
#include <stdlib.h>

typedef struct Node Node;
struct Node {
    int val;
    Node* next;
};

typedef struct LinkedList LinkedList;
struct LinkedList {
    Node* head;
};

Node* node_new(int val);
void list_print(const Node* const head);
size_t list_len(const Node* head);
void list_free(Node** head);
void push_front(Node** head, int val);
void push_back(Node** head, int val);
bool insert_at(Node** head, size_t idx, int val);
Node* find(Node* head, int val);
bool pop_front(Node** head, int* out);
bool remove_value(Node** head, int val);
