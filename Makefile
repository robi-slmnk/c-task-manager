opt1:
	clang -Wall -c main.c -o main.o
	clang -Wall -c process_core.c -o process_core.o
	clang -Wall process_core.o main.o -o task_manager