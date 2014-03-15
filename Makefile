CC='gcc'
FLAGS='-Wall'
LIBS=''
all: scheduler
	@echo 'Construccion Finalizada'
scheduler: Quiz.c
	$(CC) $(FLAGS) Quiz.c -o scheduler
	@echo 'Compilando'
clear:
	rm scheduler
