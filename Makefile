CC = gcc
CFLAGS = -g -Wall

all: server

server: server.o parser.tab.o lex.yy.o ast.o
	$(CC) $(CFLAGS) -o server server.o parser.tab.o lex.yy.o ast.o

# Ensure parser.tab.c and parser.tab.h are generated before server.o
parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

# Ensure lex.yy.c is generated after parser.tab.h
lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

# Specify that server.o depends on parser.tab.h
server.o: server.c parser.tab.h
	$(CC) $(CFLAGS) -c server.c

ast.o: ast.c ast.h
	$(CC) $(CFLAGS) -c ast.c

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

clean:
	rm -f server *.o parser.tab.* lex.yy.c
