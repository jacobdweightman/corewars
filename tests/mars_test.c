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

void test_create_mars(void) {
    mars m = create_mars(256, 64, 100);
    TEST_ASSERT_EQUAL(m.core_size, 256);
    TEST_ASSERT_EQUAL(m.block_size, 64);
    TEST_ASSERT_EQUAL(m.duration, 100);
    TEST_ASSERT_EQUAL(m.elapsed, 0);
    TEST_ASSERT_EQUAL(m.alive_count, 0);
    TEST_ASSERT_EQUAL(m.next_warrior, NULL);

    for(unsigned int i=0; i<m.core_size; i++) {
        TEST_ASSERT_EQUAL_UINT32(m.core[i], 0);
    }

    for(unsigned int i=0; i<(m.core_size / m.block_size); i++) {
        TEST_ASSERT_FALSE(m.blocks[i]);
    }
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

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_create_mars);
    RUN_TEST(test_insert_warrior_empty);
    RUN_TEST(test_insert_warrior);
    RUN_TEST(test_remove_warrior_middle);
    RUN_TEST(test_remove_warrior_next);
    RUN_TEST(test_remove_warrior_only);
    RUN_TEST(test_load_program);
    UNITY_END();

    return 0;
}
