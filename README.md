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

## What's Next

I've started to implement the Memory Array Redcode Simulator (MARS), which can
be found in `src/mars.c`. For now the program is run through the main script
`src/main.c`, but eventually I hope to create python bindings for interfacing
with a web app.
