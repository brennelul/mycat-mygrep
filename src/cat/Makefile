GCC = gcc -Wall -Werror -Wextra

.PHONY: all clean rebuild mycat

all: mycat
mycat:
	$(GCC) cat.c -o mycat
clean:
	rm mycat
rebuild: clean all
