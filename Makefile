# Copyright 2018 Jacob Weightman
#
# This file is part of corewars-1984.
#
# corewars-1984 is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3
# of the License, or (at your option) any later version.
#
# corewars-1984 is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, see <http://www.gnu.org/licenses/>.
#
# Author: Jacob Weightman <jacobdweightman@gmail.com>

COMPILER=gcc
LEX=lex
YACC=yacc
PYTHON=python3

C_FLAGS=-Wall -Wextra -pedantic -Wconversion

LIB=lib
SOURCE=src
TMP=tmp
TEST=tests
OUTPUT=build

.PHONY: all assembler mars test asm_test mars_test examples clean

all: assembler mars

assembler: $(TMP)/y.tab.c $(TMP)/lex.yy.c $(TMP)/program.h $(SOURCE)/assembler.c
	@mkdir -p build
	$(COMPILER) $(TMP)/lex.yy.c $(TMP)/y.tab.c $(SOURCE)/assembler.c -o $(OUTPUT)/assembler

mars: $(SOURCE)/mars.c $(SOURCE)/mars.h $(SOURCE)/program.h $(SOURCE)/main.c
	@mkdir -p build
	$(COMPILER) $(C_FLAGS) $(SOURCE)/mars.c $(SOURCE)/main.c -o $(OUTPUT)/mars

$(TMP)/y.tab.c: $(SOURCE)/redcode.y
	@mkdir -p $(TMP)
	$(YACC) -d $(SOURCE)/redcode.y -o $(TMP)/y.tab.c

$(TMP)/lex.yy.c: $(SOURCE)/redcode.l
	@mkdir -p $(TMP)
	$(LEX) -o $(TMP)/lex.yy.c $(SOURCE)/redcode.l

$(TMP)/program.h: $(SOURCE)/program.h
	@mkdir -p $(TMP)
	cp $(SOURCE)/program.h $(TMP)

test: asm_test mars_test
	./$(TMP)/asm_test
	./$(TMP)/mars_test

asm_test: assembler $(TEST)/asm_test.c
	$(COMPILER) $(TMP)/lex.yy.c $(TMP)/y.tab.c ./$(LIB)/unity/unity.c $(TEST)/asm_test.c -o $(TMP)/asm_test

mars_test: mars $(TEST)/mars_test.c
	$(COMPILER) $(C_FLAGS) $(SOURCE)/mars.c ./$(LIB)/unity/unity.c $(TEST)/mars_test.c -o $(TMP)/mars_test

examples: program

clean:
	rm -r $(OUTPUT)
	rm -r $(TMP)
