all: gbdis gbinfo gen_test

gbinfo:
	gcc -o gbinfo gbinfo.c

gbdis: gen_instruction_parser
	gcc -o gbdis gbdis.c

gen_instruction_parser:
	./gen_instruction_parser.py < mnemonics.txt > instruction_parse.inc

gen_test:
	./gen_test.py > test.bin

test:
	./gbdis < test.bin
