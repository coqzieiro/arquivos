all:
	gcc -Wall -o programaTrab *.c

run: all
	./programaTrab

clean:
	rm *.exe