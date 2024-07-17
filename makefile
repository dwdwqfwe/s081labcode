src=$(wildcard ./*.c)
dst=$(patsubst %.c,%,$(src))
all:$(dst)
$(dst):%:%.c
	gcc $^ -o $@ -g -levent
# $(dst):$(src)
# 	gcc -c $^ -o $@ -g
# src = $(wildcard ./*.c)
# dst = $(patsubst %.c, %.o, $(src))
# executables = $(patsubst %.c, %, $(src))

# # all: $(executables)

# $(dst): %.o: %.c
# 	gcc -c $< -o $@ -g
