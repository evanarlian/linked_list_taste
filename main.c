#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

struct Node {
    int val;
    struct Node* next;
};
typedef struct Node Node;

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

void list_free(Node* head) {
    Node* walk = head;
    while (walk != NULL) {
        Node* prev = walk;
        walk = walk->next;
        free(prev);
    }
}

void push_front(Node** head, int val) {
    // Node ** act as another struct wrapping Node*. This mindset will make thinking easier (TODO is this right?)
    // for example LinkedList { Node* node } and then we accept LinkedList* instead
    Node* new_node = node_new(val);
    Node* old_head = *head;
    new_node->next = old_head;
    *head = new_node;
}

void push_back(Node** head, int val) {
    Node* new_node = node_new(val);
    // we need to walk
    Node* walk = *head;
    Node* prev = NULL;
    while (walk != NULL) {
        prev = walk;
        walk = walk->next;
    }
    // at this point walk is NULL and prev can be NULL or one before walk
    if (prev == NULL) {
        *head = new_node;
    } else {
        prev->next = new_node;
    }
}

bool insert_at(Node** head, size_t idx, int val) {
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

// return the first matching or null
Node* find(Node* head, int val) {
    // why is this a Node*? interesting choice
    Node* walk = head;
    while (!(walk == NULL || walk->val == val)) {
        walk = walk->next;
    }
    return walk;
}
// Remove the first node and store its value in *out. Return false if the list is empty.
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
// Remove the first node whose value is val.
// this is the question of linus torvalds taste in TED
bool remove_value(Node** head, int val) {
    Node* walk = *head;
    Node* prev = NULL;
    while (!(walk == NULL || walk->val == val)) {
        prev = walk;
        walk = walk->next;
    }
    // either we found val (walk is at val) or walk is null
    // 0 1 2 3 n
    //       p w  ; remove -1 (walk is null, not found)
    // w          ; remove 0
    //   p w      ; remove 2
    if (walk == NULL) {
        return false;
    }
    if (prev == NULL) {
        int out;
        pop_front(head, &out);
    } else {
        prev->next = walk->next;
        free(walk);
    }
    return true;
}

int main() {
    Node* head = node_new(0);
    list_print(head);
    push_front(&head, 1);
    push_front(&head, 2);
    list_print(head);
    printf("current list length: %zu\n", list_len(head));

    push_back(&head, 3);
    push_back(&head, 4);
    list_print(head);
    printf("current list length: %zu\n", list_len(head));

    insert_at(&head, 3, 7);
    insert_at(&head, 3, 7);
    insert_at(&head, 100, 700);
    list_print(head);
    printf("current list length: %zu\n", list_len(head));

    Node* found_node = find(head, 4);
    if (found_node) {
        printf("node found with value: %d\n", found_node->val);
    } else {
        printf("node not found\n");
    }
    list_print(head);
    printf("current list length: %zu\n", list_len(head));

    int out;
    bool pop_front_ok = pop_front(&head, &out);
    if (pop_front_ok) {
        printf("pop front ok with value: %d\n", out);
    } else {
        printf("pop front fails due to empty\n");
    }
    list_print(head);
    printf("current list length: %zu\n", list_len(head));

    bool remove_value_ok = remove_value(&head, 4);
    if (remove_value_ok) {
        printf("remove value ok\n");
    } else {
        printf("remove value fails due to not found\n");
    }
    list_print(head);
    printf("current list length: %zu\n", list_len(head));

    list_free(head);
    return 0;
}