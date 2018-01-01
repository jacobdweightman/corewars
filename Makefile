COMPILER=gcc
C_FLAGS=-Wall -Wextra -pedantic
LEX=lex
SOURCE=src
TMP=tmp
OUTPUT=build

.PHONY: all assembler clean

all: assembler

assembler: $(TMP)/lex.yy.c $(TMP)/redcode.h $(SOURCE)/assembler.c
	@mkdir -p build
	$(COMPILER) $(SOURCE)/assembler.c $(TMP)/lex.yy.c -o $(OUTPUT)/assembler

$(TMP)/lex.yy.c: $(SOURCE)/redcode.l
	@mkdir -p $(TMP)
	lex -o $(TMP)/lex.yy.c $(SOURCE)/redcode.l

$(TMP)/redcode.h: $(SOURCE)/redcode.h
	@mkdir -p $(TMP)
	cp $(SOURCE)/redcode.h $(TMP)

clean:
	rm -r $(OUTPUT)
	rm -r $(TMP)
