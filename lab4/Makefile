# 定义宏变量 CC = gcc
CC = gcc
SRC = src/
RES = res/
LIB = `pkg-config gtk+-2.0 --cflags --libs`
INCLUDE = -I./include/
TARGET = main.out

SRCS = $(SRC)main.c $(SRC)stringtool.c \
	   $(SRC)cpu.c $(SRC)meminfo.c \
	   $(SRC)process.c $(SRC)graph.c \
	   $(SRC)sysinfo.c
OBJS = $(SRCS:.c=.o)

# $@	目标文件   
# $^	所有的依赖文件  
# $<	第一个依赖文件
#	@echo TARGET:$@
#	@echo OBJECTS:$^
$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(LIB)

clean:
	rm -rf $(TARGET) $(OBJS) 

%.o:%.c
	$(CC) $(INCLUDE) -o $@ -c $< $(LIB)
