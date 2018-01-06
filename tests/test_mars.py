import unittest
import cffi

class ParserTestCase(unittest.TestCase):
    def setUp(self):
        ffi = cffi.FFI()

        ffi.cdef("""
            /* BEGIN program.h */
            #define INSTRUCTION_TYPE_WIDTH 4
            #define ADDRESSING_MODE_WIDTH 2
            #define OPERAND_WIDTH 12

            #define IMMEDIATE_MODE 0x0
            #define RELATIVE_MODE 0x1
            #define INDIRECT_MODE 0x2

            #define DAT_TYPE 0
            #define MOV_TYPE 1
            #define ADD_TYPE 2
            #define SUB_TYPE 3
            #define JMP_TYPE 4
            #define JMZ_TYPE 5
            #define DJZ_TYPE 6
            #define CMP_TYPE 7

            #define MAX_PROGRAM_SIZE 64

            #define TYPE_OFFSET 28
            #define A_MODE_OFFSET 26
            #define B_MODE_OFFSET 24
            #define A_OFFSET 12
            #define B_OFFSET 0

            #define MODE_MASK 0x3
            #define OPERAND_MASK 0xFFF

            #define TYPE_MASK 0xF0000000
            #define A_MODE_MASK 0x0C000000
            #define B_MODE_MASK 0x03000000
            #define A_MASK 0x00FFF000
            #define B_MASK 0x00000FFF

            typedef struct instruction {
                unsigned int type: INSTRUCTION_TYPE_WIDTH;
                unsigned int a_mode: ADDRESSING_MODE_WIDTH;
                unsigned int b_mode: ADDRESSING_MODE_WIDTH;
                unsigned int a: OPERAND_WIDTH;
                unsigned int b: OPERAND_WIDTH;
            } instruction;

            typedef struct operand {
                unsigned int mode: ADDRESSING_MODE_WIDTH;
                unsigned int value: OPERAND_WIDTH;
            } operand;

            typedef uint32_t opcode;

            typedef struct program {
                int player_id;
                bool alive;
                int PC;
                int size;
                opcode code[MAX_PROGRAM_SIZE];
            } program;
            /* END program.h */

            /* FROM mars.h */
            #define DURATION 84
            #define CORE_SIZE 257  // 4096
            #define MEMORY_BLOCKS 4 // CORE_SIZE / MAX_PROGRAM_SIZE

            typedef struct mars {
                int program_count;
                int alive;
                int elapsed;
                program* programs[MEMORY_BLOCKS];
                opcode core[CORE_SIZE];
                bool blocks[MEMORY_BLOCKS];
            } mars;
            /* END mars.h */

            unsigned int randuint();
            void print_block(mars* m, int index);
            instruction get_instruction(opcode op);
            void load_program(mars* m, program* prog);
            program read_program(FILE* f);
            int get_operand_value(mars* m, int index, unsigned int mode,
                                  unsigned int raw_value);
            int get_operand_address(mars* m, int index, unsigned int mode,
                                    unsigned int raw_value);
            bool tick(mars* m, program* prog);
            mars create_mars();
        """)

        C = ffi.dlopen("build/mars")

        self.module = C

    def test_read_program(self):
        '''Just load a program, and make sure it's loaded correctly. Might
        consider adding example longer than MAX_PROGRAM_SIZE.'''
        prog = self.module.read_program(open('programs/gemini.hex'))
        self.assertEquals(prog.player_id, 0)
        self.assertTrue(prog.alive)
        self.assertEquals(prog.size, 10)
        self.assertSequenceEqual(
                prog.code[0:prog.size],
                [0x1100003B, 0x1103203C, 0x1A006007, 0x21001005, 0x21001005,
                 0x74003008, 0x4100002E, 0x41000FFB, 0x00000000, 0x00000032]
        )

    def test_get_instruction(self):
        self.assertTrue(True)

    def test_create_mars(self):
        self.assertTrue(True)

    def test_load_program(self):
        self.assertTrue(True)

    def test_get_operand_value(self):
        self.assertTrue(True)

    def test_get_operand_address(self):
        self.assertTrue(True)

    def test_tick(self):
        self.assertTrue(True)
