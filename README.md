# gbdisasm

Hi this is a cheap gameboy disassembler designed to produce assembly code which can immediatebly be reassembled with wla-gb (my fav gb disassembler, yeahyeah).

_Currently doesn't necessarily reassemble ROM headers properly. Workaround: edit `gbdisasm` and set `HEADER_DIRECTIVES=false`_

_Tested with tetris.gb and harvestmoongb.gbc_

## Example usage

Do this to disassemble tetris:
`gbdisasm tetris.gb > tetris.asm`

And (if you have wla-gb) do this to reassemble it real quick for test:
`echo "[objects]\ntetris.o" > link && wla-gb -o tetris.o tetris.asm && wlalink link tetris_again.gb && rm link && rm tetris.o'

Check and see that it reassembled unaltered if you like (it should say nothing):
`diff tetris.gb tetris_again.gb`

## Compile

`python3` is needed to generate the instruction parsing code, and `gcc` is all I've tried compiling it with.
Just running `make` should build everything, yeah. (But I don't know how to write make files properly so if you're gonna use a different C compiler you gotta edit that.)

## Included programs

`gbdisasm` this is the program you wanna use. It's a shell script that takes a rom file and spits out a complete disassembled assembly file ready to reassembled with wla-gb.
`gbdis` is the actual disassembler. It takes in bytes from stdin and spits out mnemonics.
`gbinfo` is a utility to extract header information from rom files.
`gen_instruction_parser.py` generates instruction parsing code from mnemonics.txt (taken from wla-gb)
`gen_test.py` just spits out a bin file including every instruction, for testing purposes of course. :p
