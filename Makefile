CFLAGS=-std=c11 -pedantic -Wall -ggdb #-pg
OPT_CFLAGS=$(CFLAGS) -O3 -g # -pg
LIBS=-lcunit

SUPPORTFILES= trie.h trie.c

TESTS=trie_test.c

.PHONY: test
test: $(patsubst %.c,%,$(TESTS))
        for t in $^; do ./$$t ; done

%_test: %_test.c $(TESTS)
        gcc -o $@ $(OPT_CFLAGS)  $(filter %.c,$^) $(LIBS) $(SUPPORTFILES)
        ./trie_test

