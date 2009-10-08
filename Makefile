all:
	gcc -ggdb -Wall -Werror -pedantic -I/usr/include/taglib -ltag_c taglib-scanner.c -o taglib-scanner

clean:
	rm -f taglib-scanner
