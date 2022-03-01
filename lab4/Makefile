# 定义宏变量 CC = gcc
CC = gcc
SRC = src/
RES = res/
INCLUDE = -I./include/
TARGET = main

SRCS = $(SRC)main.c $(SRC)stringtool.c \
	   $(SRC)cpu.c $(SRC)meminfo.c
OBJS = $(SRCS:.c=.o)

# $@	目标文件   
# $^	所有的依赖文件  
# $<	第一个依赖文件
#	@echo TARGET:$@
#	@echo OBJECTS:$^
$(TARGET):$(OBJS)
	$(CC) -o $@ $^

clean:
	rm -rf $(TARGET) $(OBJS)

%.o:%.c
	$(CC) $(INCLUDE) -o $@ -c $<
