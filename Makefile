all:
	make -C ./src/cat
	make -C ./src/grep
	mv ./src/cat/mycat ./src/grep/mygrep .
clean:
	rm -f mycat mygrep