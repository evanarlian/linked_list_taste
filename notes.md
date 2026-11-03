# notes

## how to read pointers

Rule of thumb:
* We separate declaration in 2 parts `const int* const` becomes `const int` + `* const`. Now we can clearly see which one is constant.
* The same parts logic can be extended to any number of pointers, such as `const int***const*const*`, which is `const int` + `*` + `*` + `*const` + `*const` + `*`.
* The `const int` can be written as `int const`, so that means `const int* const` is the same as `int const* const`, but I don't like the alternatives.
* Just remember that `const` attaches to whatever on its left, but when the left is missing, const looks at its immediate right. That is why `const int` is the same as `int const`.
* Read from the right. `int* const a` means: a is a constant pointer to non-constant int.

Examples:
* `int*` means non-constant pointer to non-constant data.
    ```c
    int eggs = 4;
    int apples = 9;
    int* food = &eggs;
    *food = 5;  // allowed
    food = &apples;  // allowed
    ```
* `const int*` means non-constant pointer to constant data.
    ```c
    int eggs = 4;
    int apples = 9;
    const int* food = &eggs;
    // *food = 5;  // NOT allowed
    food = &apples;  // allowed
    ```
* `int* const` means constant pointer to non-constant data.
    ```c
    int eggs = 4;
    int apples = 9;
    int* const food = &eggs;
    *food = 5;  // allowed
    // food = &apples;  // NOT allowed
    ```
* `const int* const` means constant pointer to constant data.
    ```c
    int eggs = 4;
    int apples = 9;
    const int* const food = &eggs;
    // *food = 5;  // NOT allowed
    // food = &apples;  // NOT allowed
    ```

You need to remember, when I say "constant data", I mean "constant data THROUGH the lens of the pointer". In this case, constant data according to `food`. In theory I can still change eggs with another less strict pointer.
```c
int eggs = 4;
int apples = 9;

const int* const food = &eggs;
// *food = 5;  // NOT allowed
// food = &apples;  // NOT allowed

int* sneaky = &eggs;
*sneaky = 5;  // allowed!!
sneaky = &apples;  // allowed!!
```
