// Tests for the linked list interface in list.h.
//
// The same file is linked against each implementation, so both must pass
// exactly the same tests:
//   make test_poor   # builds and runs build/test_poor_taste
//   make test_good   # builds and runs build/test_good_taste
//
// Sanitizers (-fsanitize=address,undefined, set in the Makefile) catch leaks,
// use-after-free and NULL derefs that a CHECK alone would miss.
#include "list.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// The Makefile passes -DIMPL_NAME=\"poor_taste\" etc. so the output says which
// implementation is under test.
#ifndef IMPL_NAME
#define IMPL_NAME "unknown"
#endif

// ---------------------------------------------------------------------------
// Tiny test "framework": a CHECK macro that records failures and keeps going
// (unlike assert(), which aborts the whole program on the first failure).
// ---------------------------------------------------------------------------

static int checks_run = 0;
static int checks_failed = 0;
static int tests_run = 0;
static int tests_failed = 0;

// Run one test function and count it as failed if any CHECK inside it failed.
#define RUN(test)                                         \
    do {                                                  \
        int failed_before = checks_failed;                \
        tests_run++;                                      \
        test();                                           \
        if (checks_failed != failed_before) {             \
            tests_failed++;                               \
            fprintf(stderr, "  FAIL %s\n\n", #test);      \
        }                                                 \
    } while (0)

#define CHECK(cond)                                                                        \
    do {                                                                                   \
        checks_run++;                                                                      \
        if (!(cond)) {                                                                     \
            checks_failed++;                                                               \
            fprintf(stderr, "%s:%d: in %s: CHECK failed: %s\n", __FILE__, __LINE__, __func__, \
                    #cond);                                                                \
        }                                                                                  \
    } while (0)

// Build a list from literal values: LIST(1, 2, 3)
#define LIST(...) list_from_array((const int[]){__VA_ARGS__}, sizeof((const int[]){__VA_ARGS__}) / sizeof(int))

// Check that a non-empty list holds exactly these values: CHECK_LIST(head, 1, 2, 3)
// (For an empty list, use CHECK(head == NULL).)
#define CHECK_LIST(head, ...) \
    CHECK(list_matches((head), (const int[]){__VA_ARGS__}, sizeof((const int[]){__VA_ARGS__}) / sizeof(int)))

// Builds the list by hand with node_new, so tests don't depend on push_back.
static Node* list_from_array(const int* vals, size_t n) {
    Node* head = NULL;
    Node* tail = NULL;
    for (size_t i = 0; i < n; ++i) {
        Node* node = node_new(vals[i]);
        if (tail == NULL) {
            head = node;
        } else {
            tail->next = node;
        }
        tail = node;
    }
    return head;
}

// True if the list holds exactly `expected`, in order. Prints both on mismatch.
static bool list_matches(const Node* head, const int* expected, size_t n) {
    const Node* walk = head;
    size_t i = 0;
    while (walk != NULL && i < n && walk->val == expected[i]) {
        walk = walk->next;
        i++;
    }
    if (walk == NULL && i == n) {
        return true;
    }
    fprintf(stderr, "    expected: ");
    for (size_t j = 0; j < n; ++j) {
        fprintf(stderr, "%d -> ", expected[j]);
    }
    fprintf(stderr, "NULL\n    actual:   ");
    for (walk = head; walk != NULL; walk = walk->next) {
        fprintf(stderr, "%d -> ", walk->val);
    }
    fprintf(stderr, "NULL\n");
    return false;
}

// ---------------------------------------------------------------------------
// node_new
// ---------------------------------------------------------------------------

static void test_node_new(void) {
    Node* node = node_new(42);
    CHECK(node != NULL);
    CHECK(node->val == 42);
    CHECK(node->next == NULL);
    free(node);
}

// ---------------------------------------------------------------------------
// list_len
// ---------------------------------------------------------------------------

static void test_list_len_empty(void) {
    CHECK(list_len(NULL) == 0);
}

static void test_list_len_one(void) {
    Node* head = LIST(5);
    CHECK(list_len(head) == 1);
    list_free(&head);
}

static void test_list_len_many(void) {
    Node* head = LIST(1, 2, 3, 4);
    CHECK(list_len(head) == 4);
    list_free(&head);
}

// ---------------------------------------------------------------------------
// list_free / list_print (mostly "doesn't crash"; ASan reports any leak)
// ---------------------------------------------------------------------------

static void test_list_free_empty(void) {
    Node* head = NULL;
    list_free(&head);
    CHECK(head == NULL);
}

static void test_list_free_sets_head_null(void) {
    Node* head = LIST(1, 2, 3);
    list_free(&head);
    CHECK(head == NULL);
}

static void test_list_free_twice_is_safe(void) {
    // The second call sees an empty list, so nothing is freed twice.
    Node* head = LIST(1, 2);
    list_free(&head);
    list_free(&head);
    CHECK(head == NULL);
}

static void test_list_print_smoke(void) {
    // Output isn't captured; this only checks that printing doesn't crash.
    Node* head = LIST(1, 2);
    list_print(NULL);
    list_print(head);
    list_free(&head);
    CHECK(true);
}

// ---------------------------------------------------------------------------
// push_front
// ---------------------------------------------------------------------------

static void test_push_front_empty(void) {
    Node* head = NULL;
    push_front(&head, 1);
    CHECK_LIST(head, 1);
    list_free(&head);
}

static void test_push_front_many(void) {
    Node* head = LIST(3);
    push_front(&head, 2);
    push_front(&head, 1);
    CHECK_LIST(head, 1, 2, 3);
    list_free(&head);
}

// ---------------------------------------------------------------------------
// push_back
// ---------------------------------------------------------------------------

static void test_push_back_empty(void) {
    Node* head = NULL;
    push_back(&head, 1);
    CHECK_LIST(head, 1);
    list_free(&head);
}

static void test_push_back_one(void) {
    Node* head = LIST(1);
    push_back(&head, 2);
    CHECK_LIST(head, 1, 2);
    list_free(&head);
}

static void test_push_back_many(void) {
    Node* head = LIST(1, 2);
    push_back(&head, 3);
    push_back(&head, 4);
    CHECK_LIST(head, 1, 2, 3, 4);
    list_free(&head);
}

// ---------------------------------------------------------------------------
// insert_at
// ---------------------------------------------------------------------------

static void test_insert_at_empty_idx0(void) {
    Node* head = NULL;
    CHECK(insert_at(&head, 0, 9) == true);
    CHECK_LIST(head, 9);
    list_free(&head);
}

static void test_insert_at_empty_idx1_fails(void) {
    Node* head = NULL;
    CHECK(insert_at(&head, 1, 9) == false);
    CHECK(head == NULL);
}

static void test_insert_at_front(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(insert_at(&head, 0, 9) == true);
    CHECK_LIST(head, 9, 1, 2, 3);
    list_free(&head);
}

static void test_insert_at_middle(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(insert_at(&head, 1, 9) == true);
    CHECK_LIST(head, 1, 9, 2, 3);
    list_free(&head);
}

static void test_insert_at_end(void) {
    // idx == len is valid: appends.
    Node* head = LIST(1, 2, 3);
    CHECK(insert_at(&head, 3, 9) == true);
    CHECK_LIST(head, 1, 2, 3, 9);
    list_free(&head);
}

static void test_insert_at_past_end_fails(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(insert_at(&head, 4, 9) == false);
    CHECK(insert_at(&head, 100, 9) == false);
    CHECK_LIST(head, 1, 2, 3);  // list unchanged
    list_free(&head);
}

// ---------------------------------------------------------------------------
// find
// ---------------------------------------------------------------------------

static void test_find_empty(void) {
    CHECK(find(NULL, 1) == NULL);
}

static void test_find_head(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(find(head, 1) == head);
    list_free(&head);
}

static void test_find_tail(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(find(head, 3) == head->next->next);
    list_free(&head);
}

static void test_find_missing(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(find(head, 4) == NULL);
    list_free(&head);
}

static void test_find_returns_first_duplicate(void) {
    Node* head = LIST(1, 7, 7);
    CHECK(find(head, 7) == head->next);
    list_free(&head);
}

// ---------------------------------------------------------------------------
// pop_front
// ---------------------------------------------------------------------------

static void test_pop_front_empty(void) {
    Node* head = NULL;
    int out = -12345;
    CHECK(pop_front(&head, &out) == false);
    CHECK(head == NULL);
    CHECK(out == -12345);  // out untouched on failure
}

static void test_pop_front_one(void) {
    Node* head = LIST(5);
    int out = 0;
    CHECK(pop_front(&head, &out) == true);
    CHECK(out == 5);
    CHECK(head == NULL);
}

static void test_pop_front_many(void) {
    Node* head = LIST(1, 2, 3);
    int out = 0;
    CHECK(pop_front(&head, &out) == true);
    CHECK(out == 1);
    CHECK_LIST(head, 2, 3);
    list_free(&head);
}

static void test_pop_front_until_empty(void) {
    Node* head = LIST(1, 2);
    int out = 0;
    CHECK(pop_front(&head, &out) && out == 1);
    CHECK(pop_front(&head, &out) && out == 2);
    CHECK(pop_front(&head, &out) == false);
    CHECK(head == NULL);
}

// ---------------------------------------------------------------------------
// remove_value
// ---------------------------------------------------------------------------

static void test_remove_value_empty(void) {
    Node* head = NULL;
    CHECK(remove_value(&head, 1) == false);
    CHECK(head == NULL);
}

static void test_remove_value_only_node(void) {
    Node* head = LIST(1);
    CHECK(remove_value(&head, 1) == true);
    CHECK(head == NULL);
}

static void test_remove_value_head(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(remove_value(&head, 1) == true);
    CHECK_LIST(head, 2, 3);
    list_free(&head);
}

static void test_remove_value_middle(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(remove_value(&head, 2) == true);
    CHECK_LIST(head, 1, 3);
    list_free(&head);
}

static void test_remove_value_tail(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(remove_value(&head, 3) == true);
    CHECK_LIST(head, 1, 2);
    list_free(&head);
}

static void test_remove_value_missing(void) {
    Node* head = LIST(1, 2, 3);
    CHECK(remove_value(&head, 4) == false);
    CHECK_LIST(head, 1, 2, 3);  // list unchanged
    list_free(&head);
}

static void test_remove_value_first_duplicate_only(void) {
    Node* head = LIST(7, 1, 7);
    CHECK(remove_value(&head, 7) == true);
    CHECK_LIST(head, 1, 7);
    list_free(&head);
}

static void test_remove_value_only_node_missing(void) {
    Node* head = LIST(1);
    CHECK(remove_value(&head, 2) == false);
    CHECK_LIST(head, 1);
    list_free(&head);
}

static void test_remove_value_until_empty(void) {
    // Removes head, then tail, then the last remaining node.
    Node* head = LIST(1, 2, 3);
    CHECK(remove_value(&head, 1) == true);
    CHECK(remove_value(&head, 3) == true);
    CHECK(remove_value(&head, 2) == true);
    CHECK(head == NULL);
    CHECK(remove_value(&head, 2) == false);
}

// ---------------------------------------------------------------------------
// Model test: thousands of random operations, each applied both to the
// linked list and to a plain array (the "model", which is obviously correct).
// After every operation the two must agree. This catches edge cases nobody
// thought to write a test for, and is a strong way to show that the poor and
// good taste versions really behave the same.
// ---------------------------------------------------------------------------

#define MODEL_CAP 64

typedef struct {
    int vals[MODEL_CAP];
    size_t len;
} Model;

static void model_insert(Model* m, size_t idx, int val) {
    for (size_t i = m->len; i > idx; --i) {
        m->vals[i] = m->vals[i - 1];
    }
    m->vals[idx] = val;
    m->len++;
}

static void model_remove_at(Model* m, size_t idx) {
    for (size_t i = idx; i + 1 < m->len; ++i) {
        m->vals[i] = m->vals[i + 1];
    }
    m->len--;
}

// Index of the first `val`, or m->len if missing.
static size_t model_find(const Model* m, int val) {
    size_t i = 0;
    while (i < m->len && m->vals[i] != val) {
        i++;
    }
    return i;
}

// Deterministic pseudo-random numbers, so a failure is reproducible.
static unsigned rng_state = 12345;
static unsigned rng(unsigned bound) {
    rng_state = rng_state * 1103515245u + 12345u;
    return (rng_state >> 16) % bound;
}

static void test_model_random_ops(void) {
    Node* head = NULL;
    Model m = {.len = 0};

    for (int step = 0; step < 5000; ++step) {
        int val = (int)rng(8); // small range, so duplicates and hits are common
        unsigned op = rng(6);

        // Keep the model from overflowing: when full, only remove.
        if (m.len == MODEL_CAP && op <= 2) {
            op = 4;
        }

        switch (op) {
        case 0:
            push_front(&head, val);
            model_insert(&m, 0, val);
            break;
        case 1:
            push_back(&head, val);
            model_insert(&m, m.len, val);
            break;
        case 2: {
            size_t idx = rng((unsigned)m.len + 2); // sometimes len + 1: must fail
            bool ok = insert_at(&head, idx, val);
            CHECK(ok == (idx <= m.len));
            if (idx <= m.len) {
                model_insert(&m, idx, val);
            }
            break;
        }
        case 3: {
            int out = -1;
            bool ok = pop_front(&head, &out);
            CHECK(ok == (m.len > 0));
            if (m.len > 0) {
                CHECK(out == m.vals[0]);
                model_remove_at(&m, 0);
            }
            break;
        }
        case 4: {
            size_t idx = model_find(&m, val);
            bool ok = remove_value(&head, val);
            CHECK(ok == (idx < m.len));
            if (idx < m.len) {
                model_remove_at(&m, idx);
            }
            break;
        }
        case 5: {
            Node* found = find(head, val);
            size_t idx = model_find(&m, val);
            CHECK((found != NULL) == (idx < m.len));
            CHECK(found == NULL || found->val == val);
            break;
        }
        }

        CHECK(list_len(head) == m.len);
        if (!list_matches(head, m.vals, m.len)) {
            checks_failed++;
            fprintf(stderr, "    model mismatch at step %d (op %u, val %d)\n", step, op, val);
            break; // one mismatch is enough; later steps would all fail too
        }
    }
    list_free(&head);
}

// ---------------------------------------------------------------------------

int main(void) {
    printf("testing implementation: %s\n", IMPL_NAME);

    RUN(test_node_new);

    RUN(test_list_len_empty);
    RUN(test_list_len_one);
    RUN(test_list_len_many);

    RUN(test_list_free_empty);
    RUN(test_list_free_sets_head_null);
    RUN(test_list_free_twice_is_safe);
    RUN(test_list_print_smoke);

    RUN(test_push_front_empty);
    RUN(test_push_front_many);

    RUN(test_push_back_empty);
    RUN(test_push_back_one);
    RUN(test_push_back_many);

    RUN(test_insert_at_empty_idx0);
    RUN(test_insert_at_empty_idx1_fails);
    RUN(test_insert_at_front);
    RUN(test_insert_at_middle);
    RUN(test_insert_at_end);
    RUN(test_insert_at_past_end_fails);

    RUN(test_find_empty);
    RUN(test_find_head);
    RUN(test_find_tail);
    RUN(test_find_missing);
    RUN(test_find_returns_first_duplicate);

    RUN(test_pop_front_empty);
    RUN(test_pop_front_one);
    RUN(test_pop_front_many);
    RUN(test_pop_front_until_empty);

    RUN(test_remove_value_empty);
    RUN(test_remove_value_only_node);
    RUN(test_remove_value_head);
    RUN(test_remove_value_middle);
    RUN(test_remove_value_tail);
    RUN(test_remove_value_missing);
    RUN(test_remove_value_first_duplicate_only);
    RUN(test_remove_value_only_node_missing);
    RUN(test_remove_value_until_empty);

    RUN(test_model_random_ops);

    printf("[%s] %d tests, %d failed (%d checks, %d failed)\n", IMPL_NAME, tests_run,
           tests_failed, checks_run, checks_failed);
    return tests_failed == 0 ? 0 : 1;
}
