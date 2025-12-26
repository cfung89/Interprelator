CC = gcc
CC_FLAGS = -Wall -Wextra
BIN_DIR = ./bin

all: setup ast.o evaluator.o lexer.o main.o parser.o repl.o token.o util.o
	@$(CC) $(CC_FLAGS) -o	$(BIN_DIR)/main \
							$(BIN_DIR)/ast.o \
							$(BIN_DIR)/evaluator.o \
							$(BIN_DIR)/lexer.o \
							$(BIN_DIR)/main.o \
							$(BIN_DIR)/parser.o \
							$(BIN_DIR)/repl.o \
							$(BIN_DIR)/token.o \
							$(BIN_DIR)/util.o \
							-lm
	@echo -e "\nCompiled to $(BIN_DIR)/main"

ast.o: ast.c ast.h
	$(CC) $(CC_FLAGS) -c ast.c -o $(BIN_DIR)/ast.o

evaluator.o: evaluator.c evaluator.h
	$(CC) $(CC_FLAGS) -c evaluator.c -o $(BIN_DIR)/evaluator.o

lexer.o: lexer.c lexer.h
	$(CC) $(CC_FLAGS) -c lexer.c -o $(BIN_DIR)/lexer.o

main.o: 
	$(CC) $(CC_FLAGS) -c main.c -o $(BIN_DIR)/main.o

parser.o: parser.c parser.h
	$(CC) $(CC_FLAGS) -c parser.c -o $(BIN_DIR)/parser.o

repl.o: repl.c repl.h
	$(CC) $(CC_FLAGS) -c repl.c -o $(BIN_DIR)/repl.o

token.o: token.c token.h
	$(CC) $(CC_FLAGS) -c token.c -o $(BIN_DIR)/token.o

util.o: util.c util.h
	$(CC) $(CC_FLAGS) -c util.c -o $(BIN_DIR)/util.o

setup:
	@mkdir -p $(BIN_DIR)

debug: all
	valgrind --leak-check=full --show-leak-kinds=all $(BIN_DIR)/main

run: all
	@$(BIN_DIR)/main



clean:
	@rm -rf $(BIN_DIR)
	@echo "Cleaned."
