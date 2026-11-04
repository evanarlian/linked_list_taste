# Singly linked list challenge

Functions to write, ordered from easy to hard. These are only the prototypes and what each should do; the implementations are yours.

## Level 1: basics

| Function | Behavior |
|---|---|
| `Node* node_new(int val)` | `malloc` a node, set `val`, set `next = NULL`. Return `NULL` if `malloc` fails. |
| `void list_print(const Node* head)` | Print like `1 -> 2 -> 3 -> NULL`. |
| `size_t list_len(const Node* head)` | Count the nodes. |
| `void list_free(Node** head)` | Free every node, then set `*head = NULL` so the caller isn't left holding a dangling pointer. Think about the order: once you free a node, you can't read its `next`. |

## Level 2: insertion

| Function | Behavior |
|---|---|
| `void push_front(Node** head, int val)` | Insert at the start. |
| `void push_back(Node** head, int val)` | Insert at the end. |
| `bool insert_at(Node** head, size_t idx, int val)` | Insert so the new node ends up at position `idx`. Return `false` if `idx > len`. |

Before you start, ask yourself why these take `Node**` instead of `Node*`. Hint: what happens to the caller's `head` when the list is empty?

## Level 3: lookup and removal

| Function | Behavior |
|---|---|
| `Node* find(Node* head, int val)` | Return the first matching node, or `NULL`. |
| `bool pop_front(Node** head, int* out)` | Remove the first node and store its value in `*out`. Return `false` if the list is empty. |
| `bool remove_value(Node** head, int val)` | Remove the first node whose value is `val`. |

## The "good taste" challenge

In a 2016 TED interview, Linus Torvalds showed two versions of linked-list removal:

- **Version 1** tracks a `prev` pointer and needs an `if` to handle removing the head separately.
- **Version 2** walks the list with a pointer-to-pointer (`Node** indirect`), so the head is just like any other link and the special case disappears.

Do it in two steps:

1. Write `remove_value` the obvious way first, with `prev` and the head special case.
2. Then rewrite it without any `if` for the head. The `Node** head` parameter is a hint for how to start.

Once that works, try `push_back` and `insert_at` the same way. The same trick removes their special cases too.

## Edge cases to test for every function

- an empty list (`head == NULL`)
- a list with one node
- an operation on the first node
- an operation on the last node
- a value that isn't in the list

Run with `-fsanitize=address,undefined` while testing. It catches use-after-free, leaks, and `NULL` dereferences as soon as they happen.
