#include<stdio.h>
#include "../../cliparser/include/cli.h"

int encode(unsigned int * counter, unsigned int * accumulator, int x, int y, int z, struct arguments * parameters) {

    return 0;
}


void bitStream_store(unsigned char * compressed_stream, unsigned int * written_bytes,
                    unsigned int * written_bits, unsigned int num_bits_to_write, unsigned int bits_to_write, FILE * stream){
    int i = 0;
    for(i = num_bits_to_write - 1; i >= 0; i--){
        compressed_stream[*written_bytes] |= ((bits_to_write >> i) & 0x1) << (7 - *written_bits);
        (*written_bits)++;
        if(*written_bits >= 8){
            *written_bits = 0;
            fwrite(compressed_stream[*written_bytes], 1, sizeof(char), stream);
            (*written_bytes)++;
        }
    }
}