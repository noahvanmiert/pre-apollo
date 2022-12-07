EXEC = apollo
SRC = src
SOURCES = $(wildcard $(SRC)/*.c $(SRC)/**/*.c $(SRC)/**/**/*.c)
OBJECTS = $(SOURCES:.c=.o)
FLAGS = -g -Wall -Wextra -Werror -pedantic
COMPILER = clang
BIN = bin

$(BIN)/$(EXEC): $(OBJECTS)
	$(COMPILER) $(OBJECTS) $(FLAGS) -o $(BIN)/$(EXEC)

run:
	./$(BIN)/$(EXEC)

clean:
	rm $(BIN)/apollo
	rm $(SRC)/*.o
	rm $(SRC)/lexer/token/token.o
	rm $(SRC)/lexer/lexer.o
	rm $(SRC)/parser/ast/ast.o