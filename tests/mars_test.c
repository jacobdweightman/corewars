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
#include "../src/mars.h"

#define TEST_ASSERT_EQUAL_OPCODE_ARRAY TEST_ASSERT_EQUAL_UINT32_ARRAY

void test_create_mars_1(void) {
    mars m = create_mars(256, 64, 100);
    TEST_ASSERT_EQUAL(256, m.core_size);
    TEST_ASSERT_EQUAL(64, m.block_size);
    TEST_ASSERT_EQUAL(100, m.duration);
    TEST_ASSERT_EQUAL(0, m.elapsed);
    TEST_ASSERT_EQUAL(0, m.alive_count);
    TEST_ASSERT_EQUAL(NULL, m.next_warrior);

    for(unsigned int i=0; i<256; i++) {
        TEST_ASSERT_EQUAL_UINT32(m.core[i], 0);
    }

    for(unsigned int i=0; i<4; i++) {
        TEST_ASSERT_FALSE(m.blocks[i]);
    }

    destroy_mars(&m);
}

void test_create_mars_2(void) {
    mars m = create_mars(21, 5, 50);
    TEST_ASSERT_EQUAL(21, m.core_size);
    TEST_ASSERT_EQUAL(5, m.block_size);
    TEST_ASSERT_EQUAL(50, m.duration);
    TEST_ASSERT_EQUAL(0, m.elapsed);
    TEST_ASSERT_EQUAL(0, m.alive_count);
    TEST_ASSERT_EQUAL(NULL, m.next_warrior);

    for(unsigned int i=0; i<21; i++) {
        TEST_ASSERT_EQUAL_UINT32(m.core[i], 0);
    }

    for(unsigned int i=0; i<4; i++) { // last, partial block doesn't count!
        TEST_ASSERT_FALSE(m.blocks[i]);
    }

    destroy_mars(&m);
}

void test_insert_warrior_empty(void) {
    mars m;
    warrior a;

    insert_warrior(&m, &a);

    // verify traversal order is a, a, ...
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
}

void test_insert_warrior(void) {
    mars m;
    warrior a, b, c, d;

    // insert, verify that newly inserted is always next to run
    insert_warrior(&m, &a);
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
    insert_warrior(&m, &b);
    TEST_ASSERT_EQUAL(&b, m.next_warrior);
    insert_warrior(&m, &c);
    TEST_ASSERT_EQUAL(&c, m.next_warrior);
    insert_warrior(&m, &d);
    TEST_ASSERT_EQUAL(&d, m.next_warrior);

    // move from last inserted back to first
    m.next_warrior = m.next_warrior->next;

    // verify traversal order is a, b, c, d, a, ...
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&b, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&c, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&d, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
}

void test_remove_warrior_middle(void) {
    mars m;
    warrior a, b, c, d;

    insert_warrior(&m, &a);
    insert_warrior(&m, &b);
    insert_warrior(&m, &c);
    insert_warrior(&m, &d);

    // remove warrior that is not next to run
    remove_warrior(&m, &b);

    // verify sequence changes from d, a, b, c --> d, a, c
    TEST_ASSERT_EQUAL(&d, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&c, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&d, m.next_warrior);
}

void test_remove_warrior_next(void) {
    mars m;
    warrior a, b, c, d;

    insert_warrior(&m, &a);
    insert_warrior(&m, &b);
    insert_warrior(&m, &c);
    insert_warrior(&m, &d);

    remove_warrior(&m, &d);

    // verify d, a, b, c --> a, b, c
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&b, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&c, m.next_warrior);
    m.next_warrior = m.next_warrior->next;
    TEST_ASSERT_EQUAL(&a, m.next_warrior);
}

void test_remove_warrior_only(void) {
    mars m;
    warrior a;

    insert_warrior(&m, &a);

    remove_warrior(&m, &a);

    TEST_ASSERT_EQUAL(NULL, m.next_warrior);
}

void test_load_program(void) {
    mars m = create_mars(10, 5, 100);
    opcode* buf = (opcode*) "\x0f\x0e\x0d\x0c\x0b\x0a\x09\x08\x07\x06\x05\x04";
    program prog = prog_from_buffer(5, buf, 3);

    for(unsigned int i=0; i<m.core_size; i++) {
        TEST_ASSERT_EQUAL_UINT32(0, m.core[i]);
    }

    load_program(&m, &prog, 0, 0);
    load_program(&m, &prog, 1, 2);

    warrior* w = m.next_warrior;
    TEST_ASSERT_EQUAL(5, w->id);
    TEST_ASSERT_EQUAL(7, w->PC);
    w = w->next;
    TEST_ASSERT_EQUAL(5, w->id);
    TEST_ASSERT_EQUAL(0, w->PC);


    TEST_ASSERT_EQUAL(0x0c0d0e0f, m.core[0]);
    TEST_ASSERT_EQUAL(0x08090a0b, m.core[1]);
    TEST_ASSERT_EQUAL(0x04050607, m.core[2]);
    TEST_ASSERT_EQUAL(0x00000000, m.core[3]);
    TEST_ASSERT_EQUAL(0x00000000, m.core[4]);
    TEST_ASSERT_EQUAL(0x00000000, m.core[5]);
    TEST_ASSERT_EQUAL(0x00000000, m.core[6]);
    TEST_ASSERT_EQUAL(0x0c0d0e0f, m.core[7]);
    TEST_ASSERT_EQUAL(0x08090a0b, m.core[8]);
    TEST_ASSERT_EQUAL(0x04050607, m.core[9]);

    destroy_program(&prog);
    destroy_mars(&m);
}

void test_get_operand_value(void) {
    mars m = create_mars(10, 5, 100);
    m.core[0] = (opcode) -5;
    m.core[1] = (opcode) -4;
    m.core[2] = (opcode) -3;
    m.core[3] = (opcode) -2;
    m.core[4] = (opcode) -1;
    m.core[5] = 0;
    m.core[6] = 1;
    m.core[7] = 2;
    m.core[8] = 3;
    m.core[9] = 4;

    TEST_ASSERT_EQUAL(5, get_operand_value(&m, 5, IMMEDIATE_MODE, 0x0005));
    TEST_ASSERT_EQUAL(260, get_operand_value(&m, 2, IMMEDIATE_MODE, 0x0104));
    TEST_ASSERT_EQUAL(-1, get_operand_value(&m, 9, IMMEDIATE_MODE, 0x0FFF));
    TEST_ASSERT_EQUAL(-256, get_operand_value(&m, 6, IMMEDIATE_MODE, 0x0F00));

    TEST_ASSERT_EQUAL(0, get_operand_value(&m, 3, RELATIVE_MODE, 0x0002));
    TEST_ASSERT_EQUAL(-1, get_operand_value(&m, 7, RELATIVE_MODE, 0x0FFD));
    TEST_ASSERT_EQUAL(-4, get_operand_value(&m, 8, RELATIVE_MODE, 0x0003)); // roll over top
    TEST_ASSERT_EQUAL(3, get_operand_value(&m, 2, RELATIVE_MODE, 0x0FFC)); // roll under bottom

    TEST_ASSERT_EQUAL(0, get_operand_value(&m, 4, INDIRECT_MODE, 0x0002));
    TEST_ASSERT_EQUAL(-5, get_operand_value(&m, 6, INDIRECT_MODE, 0x03));
    TEST_ASSERT_EQUAL(-4, get_operand_value(&m, 9, INDIRECT_MODE, 0x0FFE)); // roll over top
    TEST_ASSERT_EQUAL(1, get_operand_value(&m, 1, INDIRECT_MODE, 0x0FFF)); // roll under bottom

    destroy_mars(&m);
}

void test_get_operand_address(void) {
    mars m = create_mars(10, 5, 100);
    m.core[0] = (opcode) -5;
    m.core[1] = (opcode) -4;
    m.core[2] = (opcode) -3;
    m.core[3] = (opcode) -2;
    m.core[4] = (opcode) -1;
    m.core[5] = 0;
    m.core[6] = 1;
    m.core[7] = 2;
    m.core[8] = 3;
    m.core[9] = 4;

    TEST_ASSERT_EQUAL(8, get_operand_address(&m, 6, RELATIVE_MODE, 0x0002));
    TEST_ASSERT_EQUAL(1, get_operand_address(&m, 4, RELATIVE_MODE, 0x0FFD));
    TEST_ASSERT_EQUAL(0, get_operand_address(&m, 9, RELATIVE_MODE, 0x0001));
    TEST_ASSERT_EQUAL(9, get_operand_address(&m, 8, RELATIVE_MODE, 0x0001));
    TEST_ASSERT_EQUAL(8, get_operand_address(&m, 2, RELATIVE_MODE, 0x0FFC));

    TEST_ASSERT_EQUAL(9, get_operand_address(&m, 6, INDIRECT_MODE, 0x0002));
    TEST_ASSERT_EQUAL(0, get_operand_address(&m, 4, INDIRECT_MODE, 0x0FFD));
    TEST_ASSERT_EQUAL(4, get_operand_address(&m, 9, INDIRECT_MODE, 0x0001));
    TEST_ASSERT_EQUAL(2, get_operand_address(&m, 8, INDIRECT_MODE, 0x0001));
    TEST_ASSERT_EQUAL(7, get_operand_address(&m, 3, INDIRECT_MODE, 0x0FFC));

    destroy_mars(&m);
}

void test_mov_relative_relative(void) {
    mars m = create_mars(5, 5, 100);
    warrior w;
    insert_warrior(&m, &w);

    // no address wrapping
    m.core[0] = 0x15004002; // MOV 4 2
    m.core[1] = 0x00000001; // DAT 1
    m.core[2] = 0x00000002; // DAT 2
    m.core[3] = 0x00000003; // DAT 3
    m.core[4] = 0x00000004; // DAT 4

    w.PC = 0;
    tick(&m);

    TEST_ASSERT_EQUAL(0x15004002, m.core[0]);
    TEST_ASSERT_EQUAL(1, m.core[1]);
    TEST_ASSERT_EQUAL(4, m.core[2]);
    TEST_ASSERT_EQUAL(3, m.core[3]);
    TEST_ASSERT_EQUAL(4, m.core[4]);

    // with address wrapping
    m.core[0] = 0x00000000; // DAT 0
    m.core[1] = 0x1500AFFF; // MOV 10 -1
    m.core[2] = 0x00000002; // DAT 2
    m.core[3] = 0x00000003; // DAT 3
    m.core[4] = 0x00000004; // DAT 4

    w.PC = 1;
    tick(&m);

    TEST_ASSERT_EQUAL(0x1500AFFF, m.core[0]);
    TEST_ASSERT_EQUAL(0x1500AFFF, m.core[1]);
    TEST_ASSERT_EQUAL(2, m.core[2]);
    TEST_ASSERT_EQUAL(3, m.core[3]);
    TEST_ASSERT_EQUAL(4, m.core[4]);

    destroy_mars(&m);
}

void test_mov_indirect_relative(void) {
    mars m = create_mars(5, 5, 100);
    warrior w;
    insert_warrior(&m, &w);

    // no address wrapping
    m.core[0] = 0x00000000; // DAT 0
    m.core[1] = 0x00000001; // DAT 1
    m.core[2] = 0x19FFF002; // MOV @-1 2
    m.core[3] = 0x00000003; // DAT 3
    m.core[4] = 0x00000004; // DAT 4

    w.PC = 2;
    tick(&m);

    TEST_ASSERT_EQUAL(0, m.core[0]);
    TEST_ASSERT_EQUAL(1, m.core[1]);
    TEST_ASSERT_EQUAL(0x19FFF002, m.core[2]);
    TEST_ASSERT_EQUAL(3, m.core[3]);
    TEST_ASSERT_EQUAL(3, m.core[4]);

    // with address wrapping
    m.core[0] = 0x00000000; // DAT 0
    m.core[1] = 0x00000001; // DAT 1
    m.core[2] = 0x1900C006; // MOV @12 6
    m.core[3] = 0x00000003; // DAT 3
    m.core[4] = 0x00000004; // DAT 4

    w.PC = 2;
    tick(&m);

    TEST_ASSERT_EQUAL(0, m.core[0]);
    TEST_ASSERT_EQUAL(1, m.core[1]);
    TEST_ASSERT_EQUAL(0x1900C006, m.core[2]);
    TEST_ASSERT_EQUAL(1, m.core[3]);
    TEST_ASSERT_EQUAL(4, m.core[4]);

    destroy_mars(&m);
}

void test_mov_relative_indirect(void) {
    mars m = create_mars(5, 5, 100);
    warrior w;
    insert_warrior(&m, &w);

    // no address wrapping
    m.core[0] = 0x00000002; // DAT 2
    m.core[1] = 0x00000001; // DAT 1
    m.core[2] = 0x16001FFE; // MOV 1 @-2 (3, #-1 --> @-2, 2, #4)
    m.core[3] = 0xFFFFFFFF; // DAT -1
    m.core[4] = 0x00000004; // DAT 4

    w.PC = 2;
    tick(&m);

    TEST_ASSERT_EQUAL(0x00000002, m.core[0]);
    TEST_ASSERT_EQUAL(0x00000001, m.core[1]);
    TEST_ASSERT_EQUAL(0x16001FFE, m.core[2]);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, m.core[3]);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, m.core[4]); // should be -1

    // with address wrapping
    m.core[0] = 0;          // DAT 0
    m.core[1] = 1;          // DAT 1
    m.core[2] = 3;          // DAT -1
    m.core[3] = 0x16008FFC; // MOV 8 @-4
    m.core[4] = 4;          // DAT 4

    w.PC = 3;
    tick(&m);

    TEST_ASSERT_EQUAL(0x00000000, m.core[0]);
    TEST_ASSERT_EQUAL(0x00000001, m.core[1]);
    TEST_ASSERT_EQUAL(0x00000001, m.core[2]);
    TEST_ASSERT_EQUAL(0x16008FFC, m.core[3]);
    TEST_ASSERT_EQUAL(0x00000004, m.core[4]);

    destroy_mars(&m);
}

void test_mov_indirect_indirect(void) {
    mars m = create_mars(5, 5, 100);
    warrior w;
    insert_warrior(&m, &w);

    // no address wrapping
    m.core[0] = 0x00000002; // DAT 2
    m.core[1] = 0x00000001; // DAT 1
    m.core[2] = 0x1A001FFE; // MOV @1 @-2
    m.core[3] = 0xFFFFFFFF; // DAT -1
    m.core[4] = 0x00000004; // DAT 4

    w.PC = 2;
    tick(&m);

    TEST_ASSERT_EQUAL(0x00000002, m.core[0]);
    TEST_ASSERT_EQUAL(0x00000001, m.core[1]);
    TEST_ASSERT_EQUAL(0x1A001FFE, m.core[2]);
    TEST_ASSERT_EQUAL(0xFFFFFFFF, m.core[3]);
    TEST_ASSERT_EQUAL(0x00000001, m.core[4]);

    // with address wrapping
    m.core[0] = 0x00000002; // DAT 2
    m.core[1] = 0xFFFFFFFF; // DAT -1
    m.core[2] = 0x1AFFFFFE; // MOV @1 @-2
    m.core[3] = 0x1A003FF7; // DAT @3 @-9
    m.core[4] = 0x00000003; // DAT 3

    w.PC = 3;
    tick(&m);

    TEST_ASSERT_EQUAL(0x00000002, m.core[0]);
    TEST_ASSERT_EQUAL(0x1AFFFFFE, m.core[1]);
    TEST_ASSERT_EQUAL(0x1AFFFFFE, m.core[2]);
    TEST_ASSERT_EQUAL(0x1A003FF7, m.core[3]);
    TEST_ASSERT_EQUAL(0x00000003, m.core[4]);

    destroy_mars(&m);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_create_mars_1);
    RUN_TEST(test_create_mars_2);
    RUN_TEST(test_insert_warrior_empty);
    RUN_TEST(test_insert_warrior);
    RUN_TEST(test_remove_warrior_middle);
    RUN_TEST(test_remove_warrior_next);
    RUN_TEST(test_remove_warrior_only);
    RUN_TEST(test_load_program);
    RUN_TEST(test_get_operand_value);
    RUN_TEST(test_get_operand_address);
    RUN_TEST(test_mov_relative_relative);
    RUN_TEST(test_mov_indirect_relative);
    RUN_TEST(test_mov_relative_indirect);
    RUN_TEST(test_mov_indirect_indirect);
    UNITY_END();

    return 0;
}
