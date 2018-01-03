COMPILER=gcc
C_FLAGS=-Wall -Wextra -pedantic
LEX=lex
SOURCE=src
TMP=tmp
OUTPUT=build

.PHONY: all assembler mars clean

all: assembler

assembler: $(TMP)/y.tab.c $(TMP)/lex.yy.c $(TMP)/program.h
	@mkdir -p build
	$(COMPILER) $(TMP)/lex.yy.c $(TMP)/y.tab.c -o $(OUTPUT)/assembler

mars: $(SOURCE)/mars.c $(SOURCE)/mars.h $(SOURCE)/program.h
	@mkdir -p build
	$(COMPILER) $(SOURCE)/mars.c -o $(OUTPUT)/mars

$(TMP)/y.tab.c: $(SOURCE)/redcode.y
	@mkdir -p $(TMP)
	yacc -d $(SOURCE)/redcode.y -o $(TMP)/y.tab.c

$(TMP)/lex.yy.c: $(SOURCE)/redcode.l
	@mkdir -p $(TMP)
	lex -o $(TMP)/lex.yy.c $(SOURCE)/redcode.l

$(TMP)/program.h: $(SOURCE)/program.h
	@mkdir -p $(TMP)
	cp $(SOURCE)/program.h $(TMP)

clean:
	rm -r $(OUTPUT)
	rm -r $(TMP)
