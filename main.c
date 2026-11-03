#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int val;
    struct Node* next;
};
typedef struct Node Node;

Node* node_new(int val) {
    Node* node = (Node*)malloc(sizeof(Node));
    node->val = val;
    node->next = NULL;
    return node;
}

void list_print(const Node* const head) {
    const Node* walk = head;
    while (walk != NULL) {
        printf("%d -> ", walk->val);
        walk = walk->next;
    }
    printf("NULL\n");
}

size_t list_len(Node* head) {
    size_t length = 0;
    Node* walk = head;
    while (walk != NULL) {
        length += 1;
        walk = walk->next;
    }
    return length;
}

void list_free(Node* head) {
    Node* walk = head;
    while (walk != NULL) {
        Node* prev = walk;
        walk = walk->next;
        free(prev);
    }
}

void push_front(Node** head, Node* newnode) {
    // TODO fix later
    newnode->next = head;
    return newnode;
    if (head == NULL) {
    }
}
int main() {
    // make a lot
    const int**const**const** a = NULL;
    return 0;
}