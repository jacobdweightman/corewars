/* Copyright 2018 Jacob Weightman
 *
 * This file is part of corewars-1984.
 *
 * corewars-1984 is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * corewars-1984 is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jacob Weightman <jacobdweightman@gmail.com>
 */

#include <string.h>

#include "../lib/unity/unity.h"
#include "../src/mars.h"

#define TEST_ASSERT_EQUAL_OPCODE_ARRAY TEST_ASSERT_EQUAL_UINT32_ARRAY

/*
unsigned int randuint(void);
void print_block(mars* m, int index);
instruction get_instruction(opcode op);
void load_program(mars* m, program* prog);
program read_program(FILE* f);                  <----
int get_operand_value(mars* m, int index, unsigned int mode,
                      unsigned int raw_value);
int get_operand_address(mars* m, int index, unsigned int mode,
                        unsigned int raw_value);
bool tick(mars* m, program* prog);
mars create_mars(void);                         <----
*/

void test_create_mars(void) {
    mars m = create_mars();
    TEST_ASSERT_EQUAL(m.program_count, 0);
    TEST_ASSERT_EQUAL(m.alive, 0);
    TEST_ASSERT_EQUAL(m.elapsed, 0);
    TEST_ASSERT_EQUAL(m.program_count, 0);

    for(int i=0; i<CORE_SIZE; i++) {
        TEST_ASSERT_EQUAL_UINT32(m.core[i], 0);
    }

    for(int i=0; i<MEMORY_BLOCKS; i++) {
        TEST_ASSERT_FALSE(m.blocks[i]);
    }
}

void test_read_program(void) {
    FILE* bin;
    program prog;
    opcode expected_code[MAX_PROGRAM_SIZE];

    // pointless loop program
    bin = fopen("programs/nop.hex", "r");
    if(bin == NULL) {
        TEST_IGNORE_MESSAGE("nop program not found");
    }

    prog = read_program(bin);

    memset(expected_code, 0, sizeof(expected_code));
    expected_code[0] = 0x41000000;

    TEST_ASSERT_EQUAL_OPCODE_ARRAY(expected_code, prog.code, MAX_PROGRAM_SIZE);


    // imp program (copy forward)
    bin = fopen("programs/imp.hex", "r");
    if(bin == NULL) {
        TEST_IGNORE_MESSAGE("imp program not found");
    }

    prog = read_program(bin);

    memset(expected_code, 0, sizeof(expected_code));
    expected_code[0] = 0x15000001;

    TEST_ASSERT_EQUAL_OPCODE_ARRAY(expected_code, prog.code, MAX_PROGRAM_SIZE);


    // dwarf program (copy bomb)
    bin = fopen("programs/dwarf.hex", "r");
    if(bin == NULL) {
        TEST_IGNORE_MESSAGE("dwarf program not found");
    }

    prog = read_program(bin);

    memset(expected_code, 0, sizeof(expected_code));
    expected_code[0] = 0x21004003;
    expected_code[1] = 0x12001002;
    expected_code[2] = 0x41000FFE;
    expected_code[3] = 0x00000002;

    TEST_ASSERT_EQUAL_OPCODE_ARRAY(expected_code, prog.code, MAX_PROGRAM_SIZE);

    // gemini program (copy self forward)
    bin = fopen("programs/gemini.hex", "r");
    if(bin == NULL) {
        TEST_IGNORE_MESSAGE("gemini program not found");
    }

    prog = read_program(bin);

    memset(expected_code, 0, sizeof(expected_code));
    expected_code[0] = 0x1100003B;
    expected_code[1] = 0x1103203C;
    expected_code[2] = 0x1A006007;
    expected_code[3] = 0x21001005;
    expected_code[4] = 0x21001005;
    expected_code[5] = 0x74003008;
    expected_code[6] = 0x4100002E;
    expected_code[7] = 0x41000FFB;
    expected_code[8] = 0x00000000;
    expected_code[9] = 0x00000032;

    TEST_ASSERT_EQUAL_OPCODE_ARRAY(expected_code, prog.code, MAX_PROGRAM_SIZE);
}

void test_load_program(void) {

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_create_mars);
    RUN_TEST(test_read_program);
    RUN_TEST(test_load_program);
    UNITY_END();

    return 0;
}
