# corewars-1984

This project is still in progress.

My end goal is to create a nice implementation of this old programming game
that's playable on the web, with convenient matchmaking and code editing.
First things first, though.

## What's Done

The lexer and parser for the Redcode assembly language is mostly complete and
partially tested. The source for the lexer can be found in `src/redcode.l`.
The parser source can be found in `src/redcode.y`. A wrapper of the assembler
for reading source from a file can be found in `src/assembler.c`.

## How to Run it

A redcode program can be built from a file by running

```
make asm
./build/assembler -o path/to/output.hex path/to/input.asm
```

The interface for running programs is still a work in progress. In the meantime,
programs can be run in a sort of "debug" mode by passing the path to a file
containing the program as the first argument of the mars executable:

```
make mars
./build/mars path/to/program.hex
```

This will output a summary of the program to be loaded, the initial state of
the MARS, a summary of the instructions executed (stops at 5), and the
final state of the MARS.

All tests can be run by using `make test`. The tests for a particular component
can be run with `make {component}_test`, i.e.
```
make test
make asm_test
make program_test
make mars_test
```

## What's Next

I've started to implement the Memory Array Redcode Simulator (MARS), which can
be found in `src/mars.c`. For now the program is run through the main script
`src/main.c`, but eventually I hope to create python bindings for interfacing
with a web app.
