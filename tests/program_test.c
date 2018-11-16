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

#define TEST_BUILD

#include <string.h>

#include "../lib/unity/unity.h"
#include "../src/program.h"

#define TEST_ASSERT_EQUAL_OPCODE_ARRAY TEST_ASSERT_EQUAL_UINT32_ARRAY

void test_prog_from_buffer(void) {
    opcode* buf;
    program prog;

    buf = (opcode*) "\x0f\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b";
    prog = prog_from_buffer(5, buf, 3);

    TEST_ASSERT_EQUAL(prog.id, 5);
    TEST_ASSERT_EQUAL(prog.size, 3);
    TEST_ASSERT_EQUAL_OPCODE_ARRAY(buf, prog.code, 3);
}

void test_prog_from_buffer_with_null(void) {
    opcode* buf;
    program prog;

    buf = (opcode*) "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x08\x09\x0a\x0b";
    prog = prog_from_buffer(2, buf, 4);

    TEST_ASSERT_EQUAL(prog.id, 2);
    TEST_ASSERT_EQUAL(prog.size, 4);
    TEST_ASSERT_EQUAL_OPCODE_ARRAY(buf, prog.code, 4);
}

void test_prog_from_file_nop(void) {
    FILE* f;
    program prog;
    opcode expected[100];

    expected[0] = 0x41000000;

    f = fopen("programs/nop.hex", "r");
    if(f == NULL) {
      TEST_IGNORE_MESSAGE("nop program not found");
    }

    prog = prog_from_file(3, f);

    TEST_ASSERT_EQUAL(prog.id, 3);
    TEST_ASSERT_EQUAL(prog.size, 1);
    TEST_ASSERT_EQUAL_OPCODE_ARRAY(expected, prog.code, 1);
}

void test_prog_from_file_imp(void) {
    FILE* f;
    program prog;
    opcode expected[100];

    expected[0] = 0x15000001;

    f = fopen("programs/imp.hex", "r");
    if(f == NULL) {
      TEST_IGNORE_MESSAGE("nop program not found");
    }

    prog = prog_from_file(7, f);

    TEST_ASSERT_EQUAL(prog.id, 7);
    TEST_ASSERT_EQUAL(prog.size, 1);
    TEST_ASSERT_EQUAL_OPCODE_ARRAY(expected, prog.code, 1);
}

void test_prog_from_file_dwarf(void) {
    FILE* f;
    program prog;
    opcode expected[100];

    expected[0] = 0x21004003;
    expected[1] = 0x12001002;
    expected[2] = 0x41000FFE;
    expected[3] = 0x00000002;

    f = fopen("programs/dwarf.hex", "r");
    if(f == NULL) {
      TEST_IGNORE_MESSAGE("nop program not found");
    }

    prog = prog_from_file(10, f);

    TEST_ASSERT_EQUAL(prog.id, 10);
    TEST_ASSERT_EQUAL(prog.size, 4);
    TEST_ASSERT_EQUAL_OPCODE_ARRAY(expected, prog.code, 4);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_prog_from_buffer);
    RUN_TEST(test_prog_from_buffer_with_null);
    RUN_TEST(test_prog_from_file_nop);
    RUN_TEST(test_prog_from_file_imp);
    RUN_TEST(test_prog_from_file_dwarf);
    UNITY_END();

    return 0;
}
