Scratch Pad:
- Play with the debug for the adder commands, and look up the Add B and C commands to better understand what is happening

Compile:
`gcc -o disassembler disassembler.c`

Run:
`./disassembler ./rom/invaders`

Valgrind:
`valgrind -s --leak-check=full ./disassembler ./rom/invaders`