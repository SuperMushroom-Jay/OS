# 定义宏变量 CC = gcc
CC = gcc

# 执行不同函数时，修改 FUN 值即可，现在是编译 目标程序 mul.out
FUN = mul
TARGET = $(FUN).out
LIB = `pkg-config gtk+-2.0 --cflags --libs`

SRCS = $(FUN).c show.c
OBJS = $(SRCS:.c=.o)

# $@	目标文件   
# $^	所有的依赖文件  
# $<	第一个依赖文件
#	@echo TARGET:$@
#	@echo OBJECTS:$^
$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LIB)

clean:
	rm -rf *.o

%.o:%.c
	$(CC) -o $@ -c $< $(LIB)