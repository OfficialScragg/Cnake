main:
	gcc cnake.c -lncurses -o cnake

run: main
	./gamscii

debug:
	gcc -g cnake.c -o cnake-debug
	gdb cnake-debug
	rm cnake-debug

clean:
	rm cnake
