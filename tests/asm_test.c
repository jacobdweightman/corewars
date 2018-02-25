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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "../lib/unity/unity.h"
#include "../src/redcode.h"

void test_prog(char* path, uint8_t expected[]) {
    FILE* prog = fopen(path, "r");
    TEST_ASSERT_NOT_NULL(prog);

    uint8_t buf[1024];

    memset(buf, 0, sizeof(buf));
    FILE* code = fmemopen(buf, sizeof(buf), "w");

    assemble(prog, code);

    fclose(prog);
    fclose(code);

    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, buf, 1024);
}

void test_nop(void) {
    uint8_t expected[1024] = {
        '\x00', '\x00', '\x00', '\x41'
    };

    test_prog("programs/nop.asm", expected);
}

void test_imp(void) {
    uint8_t expected[1024] = {
        '\x01', '\x00', '\x00', '\x15'
    };

    test_prog("programs/imp.asm", expected);
}

void test_dwarf(void) {
    uint8_t expected[1024] = {
        '\x03', '\x40', '\x00', '\x21',
        '\x02', '\x10', '\x00', '\x12',
        '\xFE', '\x0F', '\x00', '\x41',
        '\x02', '\x00', '\x00', '\x00'
    };

    test_prog("programs/dwarf.asm", expected);
}

void test_gemini(void) {
    uint8_t expected[1024] = {
        '\x3B', '\x00', '\x00', '\x11',
        '\x3C', '\x20', '\x03', '\x11',
        '\x07', '\x60', '\x00', '\x1A',
        '\x05', '\x10', '\x00', '\x21',
        '\x05', '\x10', '\x00', '\x21',
        '\x08', '\x30', '\x00', '\x74',
        '\x2E', '\x00', '\x00', '\x41',
        '\xFB', '\x0F', '\x00', '\x41',
        '\x00', '\x00', '\x00', '\x00',
        '\x32', '\x00', '\x00', '\x00'
    };

    test_prog("programs/gemini.asm", expected);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_nop);
    RUN_TEST(test_imp);
    RUN_TEST(test_dwarf);
    RUN_TEST(test_gemini);
    UNITY_END();

    return 0;
}
