#include <stdio.h>
#include "mars.h"

int main() {
    mars m = create_mars(256, 64, 100);

    program p = prog_from_file(1, stdin);
    load_program(&m, &p, 0, 0);

    printf("warriors: %d\n", m.alive_count);

    play(&m);

    print_block(&m, 0);

    return 0;
}
