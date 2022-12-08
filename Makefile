#
#   Made by Noah Van Miert
#   6/12/2022
#
#   Apollo Compiler
#

EXEC = apollo
SRC = src
SOURCES = $(wildcard $(SRC)/*.c $(SRC)/**/*.c $(SRC)/**/**/*.c)
OBJECTS = $(SOURCES:.c=.o)
FLAGS = -g -Wall -Wextra -Werror -pedantic
COMPILER = clang
BIN = bin

$(BIN)/$(EXEC): $(OBJECTS)
	$(COMPILER) $(FLAGS) $(OBJECTS) -o $(BIN)/$(EXEC)

run:
	./$(BIN)/$(EXEC)

clean:
	rm $(BIN)/apollo
	rm $(SRC)/*.o
	rm $(SRC)/lexer/token/token.o
	rm $(SRC)/lexer/lexer.o
	rm $(SRC)/parser/ast/ast.o
	rm $(SRC)/parser/parser.o
	rm $(SRC)/compiler/compiler.o
	rm $(SRC)/parser/logging/logging.o