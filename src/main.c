#include <stdio.h>
#include "mars.h"

int main() {
    mars m = create_mars();

    program p = read_program(stdin);
    load_program(&m, &p, 0, 0);

    printf("programs: %d\n", m.program_count);
    printf("alive: %d\n", m.alive);

    play(&m);

    print_block(&m, 0);

    return 0;
}
