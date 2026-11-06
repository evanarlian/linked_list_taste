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
    // make dummy list on the stack that points to the front
    Node dummy = {.val = -1, .next = *head};
    Node* prev = &dummy; // now prev is never NULL
    Node* walk = dummy.next;
    while (walk != NULL) {
        prev = walk;
        walk = walk->next;
    }
    Node* new_node = node_new(val);
    prev->next = new_node;
    *head = dummy.next;
}

bool insert_at(Node** head, size_t idx, int val) {
    // walk until we find a stopping point
    // make dummy list on the stack that points to the front
    Node dummy = {.val = -1, .next = *head};
    Node* prev = &dummy;
    Node* walk = dummy.next;
    size_t curr_idx = 0;
    while (!(walk == NULL || curr_idx == idx)) {
        prev = walk;
        walk = walk->next;
        curr_idx += 1;
    }
    // at this point, either walk is pointing at NULL or reached the target index first
    // the curr_idx must be checked first since if both conditions are true, the insert op is still valid
    if (curr_idx != idx) {
        return false;
    }
    Node* new_node = node_new(val);
    new_node->next = walk;
    prev->next = new_node;
    *head = dummy.next;
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
    Node* front = *head;
    if (front == NULL) {
        return false;
    }
    *head = front->next;
    *out = front->val;
    free(front);
    return true;
}

// this is the linus torvalds poor taste example in TED
// modified a little bit because the arguments are different
bool remove_value(Node** head, int val) {
    // make dummy list on the stack that points to the front
    Node dummy = {.val = -1, .next = *head};
    Node* prev = &dummy;
    Node* walk = dummy.next;
    while (!(walk == NULL || walk->val == val)) {
        prev = walk;
        walk = walk->next;
    }
    if (walk == NULL) {
        return false;
    }
    prev->next = walk->next;
    free(walk);
    *head = dummy.next;
    return true;
}
