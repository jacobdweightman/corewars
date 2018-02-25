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
#include <unistd.h>

#include "redcode.h"

int main(int argc, char* argv[]) {
    char* infile;
    char* outfile;
    int c;

    while((c = getopt(argc, argv, "o:")) != -1) {
        switch(c) {
            case 'o':
                outfile = optarg;
                break;
        }
    }

    for(int i=optind; i<argc; i++) {
        infile = argv[i];
    }


    FILE* input_stream;
    FILE* output_stream;

    if(infile != NULL) {
        input_stream = fopen(infile, "r");
    } else {
        input_stream = stdin;
    }

    if(outfile != NULL) {
        output_stream = fopen(outfile, "w");
    } else {
        output_stream = stdout;
    }

    int ret = assemble(input_stream, output_stream);

    fclose(input_stream);
    fclose(output_stream);

    return ret;
}
