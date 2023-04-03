#将每个 .c 文件编译生成对应的可执行程序
Srcs := $(wildcard *.c) 
Objs := $(patsubst %.c, %, $(Srcs))
CC   := gcc
CFLAGS := -Wall -g -lpthread

Out: $(Objs)

%: %.c
	$(CC) $< -o $@ $(CFLAGS)

.PHONY: Out clean

clean: 
	$(RM) $(Objs)  
	

