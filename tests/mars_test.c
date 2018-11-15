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

void test_load_program(void) {

}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_create_mars);
    RUN_TEST(test_load_program);
    UNITY_END();

    return 0;
}
