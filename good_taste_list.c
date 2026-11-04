#include "list.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

Node* node_new(int val) {
    Node* node = malloc(sizeof(Node)); // C does not need (Node*) cast here
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

size_t list_len(const Node* head) {
    size_t length = 0;
    const Node* walk = head;
    while (walk != NULL) {
        length += 1;
        walk = walk->next;
    }
    return length;
}

void list_free(Node** head) {
    Node* walk = *head;
    while (walk != NULL) {
        Node* to_be_deleted = walk;
        walk = walk->next;
        free(to_be_deleted);
    }
    *head = NULL;
}

void push_front(Node** head, int val) {
    Node* new_node = node_new(val);
    Node* old_head = *head;
    new_node->next = old_head;
    *head = new_node;
}

void push_back(Node** head, int val) {
    // TODO(good taste): can be made cleaner?
    Node* new_node = node_new(val);
    Node* walk = *head;
    Node* prev = NULL;
    while (walk != NULL) {
        prev = walk;
        walk = walk->next;
    }
    if (prev == NULL) {
        *head = new_node;
    } else {
        prev->next = new_node;
    }
}

bool insert_at(Node** head, size_t idx, int val) {
    // TODO(good taste): can be made cleaner?
    // this can be very messy but we can make it less messy by doing double walk
    size_t len = list_len(*head);
    // 0 1 2 3, length = 4, that means max index is 4
    if (idx > len) {
        return false;
    }
    // idx 0 is special case
    if (idx == 0) {
        push_front(head, val);
        return true;
    }
    // we need to put prev on idx-1, lets say idx=2, p must be at 1
    // 0 1 2 n
    // w       ; init
    // p w     ; i=0
    //   p w   ; i=1
    Node* walk = *head;
    Node* prev = NULL;
    for (size_t i = 0; i < idx; ++i) {
        prev = walk;
        walk = walk->next;
    }
    Node* new_node = node_new(val);
    prev->next = new_node;
    new_node->next = walk;
    return true;
}

Node* find(Node* head, int val) {
    Node* walk = head;
    while (!(walk == NULL || walk->val == val)) {
        walk = walk->next;
    }
    return walk;
}

bool pop_front(Node** head, int* out) {
    // TODO(good taste): can be made cleaner?
    Node* front = *head;
    if (front == NULL) {
        return false;
    }
    *head = front->next;
    *out = front->val;
    free(front);
    return true;
}

// this is the linus torvalds good taste example in TED
// modified a little bit because the arguments are different
bool remove_value(Node** indirect, int val) {
    // linus calls Node** head as Node** indirect
    while (!(*indirect == NULL || (*indirect)->val == val)) {
        indirect = &(*indirect)->next;
    }
    if (*indirect == NULL) {
        return false;
    }
    // .. and just remove it
    Node* to_be_deleted = *indirect;
    *indirect = to_be_deleted->next;
    free(to_be_deleted);
    return true;
}
