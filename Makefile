all:
	gcc -ggdb -Wall -Werror -pedantic -I/usr/include/taglib -ltag_c tagscanner.c -o tagscanner

clean:
	rm -f tagscanner
