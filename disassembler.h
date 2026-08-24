#ifndef DISASSEMBLER_H
#define DISASSEMBLER_H

int Disassemble8080Code(unsigned char *codebuffer, int counter);
void TestDisassembler(long file_size, unsigned char *buffer);

#endif
