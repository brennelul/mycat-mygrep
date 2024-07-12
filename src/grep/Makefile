GCC = gcc -Wall -Werror -Wextra

.PHONY: all clean rebuild mygrep

all: mygrep
mygrep:
	$(GCC) grep.c -o mygrep 
clean:
	rm mygrep
rebuild: clean all
