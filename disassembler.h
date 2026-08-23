#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

int disassemble8080code(unsigned char *codebuffer, int counter);
void test_disassembler(long file_size, unsigned char *buffer);

#endif
