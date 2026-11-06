# Usage:
#   make          build everything into build/
#   make test_poor / test_good   run the tests against one implementation
#   make run_poor  / run_good    run the demo (main.c) with one implementation
#   make clean    delete build/

CC     := gcc
CFLAGS := -std=c23 -Wall -Wextra -g -fsanitize=address,undefined
BUILD  := build
IMPLS  := poor_taste good_taste

TESTS := $(IMPLS:%=$(BUILD)/test_%)
MAINS := $(IMPLS:%=$(BUILD)/main_%)

.PHONY: all clean test_poor test_good test_sentinel run_poor run_good main_sentinel

all: $(TESTS) $(MAINS)

# $* is the part matched by %, e.g. "good_taste" for build/test_good_taste,
# so this links test_list.c against good_taste_list.c.
$(BUILD)/test_%: test_list.c %_list.c list.h | $(BUILD)
	$(CC) $(CFLAGS) -DIMPL_NAME=\"$*\" test_list.c $*_list.c -o $@

$(BUILD)/main_%: main.c %_list.c list.h | $(BUILD)
	$(CC) $(CFLAGS) main.c $*_list.c -o $@

# "| $(BUILD)" is an order-only prerequisite: the directory must exist,
# but its timestamp changing does not force a rebuild.
$(BUILD):
	mkdir -p $@

# One implementation at a time. $< is the first prerequisite (the binary).
test_poor: $(BUILD)/test_poor_taste
	./$<

test_good: $(BUILD)/test_good_taste
	./$<

test_sentinel: $(BUILD)/test_sentinel
	./$<

run_poor: $(BUILD)/main_poor_taste
	./$<

run_good: $(BUILD)/main_good_taste
	./$<

run_sentinel: $(BUILD)/main_sentinel
	./$<

clean:
	rm -rf $(BUILD)
