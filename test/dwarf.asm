DAT 0
ADD #4 -1
MOV #0 @-2 // place the DAT bomb
JMP -2

// `$ ./build/assembler < test/dwarf.asm` should output:
// 1
// 10
// 3
// 9
// 10
// 10
// 2
// 9
// 10
// 11
// 10
// 5
// 10
