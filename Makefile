# 编译器
CC = arm-buildroot-linux-gnueabihf-gcc

# 编译选项
CFLAGS = -I/usr/local/imx6ull/curl/include -I/usr/local/imx6ull/ssl/include
LDFLAGS = -L/usr/local/imx6ull/curl/lib -L/usr/local/imx6ull/ssl/lib
LIBS = -lcurl -lssl -lcrypto

# 源文件
SRCS = main.c ./http/*.c ./curl/*.c

# 目标文件
OBJS = $(SRCS:.c=.o)

# 可执行文件
TARGET = TestCurl

# 默认目标
all: $(TARGET)

# 生成可执行文件
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

# 编译源文件
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)

# 伪目标
.PHONY: all clean
