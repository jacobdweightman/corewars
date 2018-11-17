#define TEST_BUILD // include test functions

#include <stdio.h>

#include "mars.h"

int main(int argc, char* argv[]) {
    if(argc < 2) {
      printf("No input file supplied. Try:\n    ./build/mars path/to/file.hex\n");
      return 1;
    }

    mars m = create_mars(10, 5, 5);
    FILE* f = fopen(argv[1], "r");
    program p = prog_from_file(1, f);

    printf("id: %u\n", p.id);
    printf("size: %lu\n", p.size);
    printf("code:\n");
    for(unsigned int i=0; i<p.size; i++) {
      printf("%08x ", p.code[i]);
    }

    load_program(&m, &p, 0, 0);
    printf("PC: %d\n", m.next_warrior->PC);

    printf("\n MARS initial state:\n");
    print_block(&m, 0);
    print_block(&m, 1);

    play(&m);

    printf("\n MARS final state:\n");
    print_block(&m, 0);
    print_block(&m, 1);

    return 0;
}
