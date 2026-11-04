#include "list.h"
#include <stdio.h>

int main() {
    Node* head = node_new(0);
    push_back(&head, 1);
    push_back(&head, 2);
    push_back(&head, 3);
    push_back(&head, 4);
    push_back(&head, 5);
    list_print(head);

    // for testing insert_at
    insert_at(&head, 2, 100);
    list_print(head);

    // remove middle item
    int to_be_removed = 2;
    printf("want to remove: %d\n", to_be_removed);
    bool ok = remove_value(&head, to_be_removed);
    printf("removed? %s\n", ok ? "y" : "n");
    list_print(head);

    // remove front item
    to_be_removed = 0;
    printf("want to remove: %d\n", to_be_removed);
    ok = remove_value(&head, to_be_removed);
    printf("removed? %s\n", ok ? "y" : "n");
    list_print(head);

    // remove missing item
    to_be_removed = -1;
    printf("want to remove: %d\n", to_be_removed);
    ok = remove_value(&head, to_be_removed);
    printf("removed? %s\n", ok ? "y" : "n");
    list_print(head);

    // free to prevent asan going wild
    list_free(&head);
    return 0;
}