opt1:
	clang -Wall -c main.c -o main.o
	clang -Wall -c mem_manager.c -o mem_manager.o
	clang -Wall -c process_core.c -o process_core.o
	clang -Wall -c rstrings.c -o rstrings.o
	clang -Wall process_core.o main.o rstrings.o mem_manager.o -o task_manager