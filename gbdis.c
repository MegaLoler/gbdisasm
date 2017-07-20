#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

// lovin these messy macros, why did i even do this
// this seriously needs a makeover...........
// this is problematic because it forces the nexit byte after unexpected labels to be dbs
// do fix this!!!!
#define get_word() if((low = getchar()) == EOF || (length && byte_i >= end_pos) || (label = address_is_label(addr + 1)) != -1) { print_db(c); if(label != -1) { goto RETRY; } break; } byte_i++; instruction_bytes[instruction_byte_i++] = low; if((high = getchar()) == EOF || (length && byte_i >= end_pos) || (label = address_is_label(addr + 2)) != -1) { print_db(c); print_db(low); if(label != -1) { goto RETRY; } break; } arg = low + (high << 8); byte_i++; instruction_bytes[instruction_byte_i++] = high;
#define get_byte() if((arg = getchar()) == EOF || (length && byte_i >= end_pos) || (label = address_is_label(addr + 1)) != -1) { print_db(c); if(label != -1) { goto RETRY; } break; } byte_i++; instruction_bytes[instruction_byte_i++] = arg;
#define get_extended_byte() if((extended_byte = getchar()) == EOF || (length && byte_i >= end_pos) || (label = address_is_label(addr + 1)) != -1) { print_db(c); if(label != -1) { goto RETRY; } break; } byte_i++; instruction_bytes[instruction_byte_i++] = extended_byte;

// some command line options
int comments; // whether to print comments at the end of each line
int db; // whether to disassembly as .db directives rather than instructions
int skip; // how many bytes in the file to ignore at the start
int offset; // how many bytes after the skip to wait until dissassembling
int length; // how many bites to process before quitting
int find; // is this a jump finding pass?
int location_count; // how many locations there were given
short unsigned *locations; // this is the list of locations to label

// keep track of which byte we are on
int byte_i;
int addr;

// keep track of the bytes loaded since the first current instruction byte
int instruction_bytes[4];
int instruction_byte_i;

// and here , come up with a coooool name for the label, or u know
// just whatever fits
// given the location/label's index in the locations array
char *get_label_name(int location_i)
{
	char *name = (char *)malloc(32 * sizeof(char));
	sprintf(name, "l_%04x", location_i);
	return name;
}

// see if an address is inside of the locations array
// hmm, theres' probably a more efficient way to do this lol
// if it exists, it returns the labels index
int address_is_label(unsigned short address)
{
	int i;
	for(i = 0; i < location_count; i++)
	{
		unsigned short location = locations[i];
		if(location == address)
		{
			return i;
		}
	}
	return -1;
}

// get a ' lol formatted ' list of the instruction bytes
char *format_n_instruction_bytes(int n)
{
	char *string = (char *)malloc((4 * n + 1) * sizeof(char));
	int i;
	for(i = 0; i < n; i++)
	{
		sprintf((string + 4 * i), "$%02x ", instruction_bytes[i]);
	}
	return string;
}

// print some general information about the line
void print_comment(int n)
{
	if(!find && comments) printf("\t\t; $%04x: %s", addr, format_n_instruction_bytes(n));
}

// print an instruction that has no args!
void print_instruction_0_args(char *mnemonic)
{
	if(!find) printf("\t%s\t\t", mnemonic);
}

// print an instruction that has a sinlge arg!
void print_instruction_1_args(char *mnemonic, char *arg)
{
	if(!find) printf("\t%s\t\t%s", mnemonic, arg);
}

// print an instruction that has two args!!
void print_instruction_2_args(char *mnemonic, char *arg1, char *arg2)
{
	if(!find) printf("\t%s\t\t%s, %s", mnemonic, arg1, arg2);
}

// print an assembler directive, given the name, how many args, and some arg strings
void print_directive(char *directive, int argc, char *argv[])
{
	if(find) return;
	printf(".%s ", directive);
	int i;
	for(i = 0; i < argc; i++)
	{
		char *arg = argv[i];
		printf("%s", arg);
		if(i + 1 < argc)
		{
			printf(", ");
		}
	}
	putchar('\n');
}

// we gotta convert relative addresses in jr instructions to absolute addresses
unsigned short relative_to_absolute_address(signed char addr)
{
	// it's relative to the position 1+ the relative address itself
	return byte_i + addr;
}

// take a raw byte (8 bits) and format it as a hex number
char *format_byte_hex(unsigned char byte)
{
	char *string = (char *)malloc(4 * sizeof(char));
	sprintf(string, "$%02x", byte);
	return string;
}

// add a minus sign if it's negative
char *format_signed_byte_hex(signed char byte)
{
	if(byte >= 0x00) return format_byte_hex(byte);
	char *string = (char *)malloc(5 * sizeof(char));
	sprintf(string, "-$%02x", 0x100 - (unsigned char)byte);
	return string;
}

// add a minus sign if it's negative, and a plus sign if positive
char *format_signed_byte_hex_plus(signed char byte)
{
	if(byte < 0x00) return format_signed_byte_hex(byte);
	char *string = (char *)malloc(5 * sizeof(char));
	sprintf(string, "+$%02x", byte);
	return string;
}

// take a raw word (16 bits) and format it as a hex number
char *format_word_hex(unsigned short word)
{
	char *string = (char *)malloc(6 * sizeof(char));
	sprintf(string, "$%04x", word);
	return string;
}

// take a raw word (16 bits) and format it as an indirect address in hex
char *format_indirect_address_hex(unsigned short word)
{
	char *string = (char *)malloc(8 * sizeof(char));
	sprintf(string, "($%04x)", word);
	return string;
}

// take a raw byte and format it as an indirect high address in hex
char *format_indirect_high_address_hex(unsigned short byte)
{
	char *string = (char *)malloc(8 * sizeof(char));
	sprintf(string, "($%02x)", byte);
	return string;
}

// take a raw byte and format it as an indirect high address in hex
char *format_high_address_hex(unsigned char byte)
{
	char *string = (char *)malloc(12 * sizeof(char));
	sprintf(string, "($ff00+$%02x)", byte);
	return string;
}

// take a raw byte and format it as an offset from the stack pointer
char *format_relative_sp_hex(unsigned char byte)
{
	char *string = (char *)malloc(7 * sizeof(char));
//	sprintf(string, "sp+$%02x", byte);
	sprintf(string, "sp%s", format_signed_byte_hex_plus(byte));
	return string;
}

// convert an absolute address into a label, if applicable, otherwise just format it
char *resolve_address(unsigned short word)
{
	int label_i;
	if((label_i = address_is_label(word)) != -1)
	{
		return get_label_name(label_i);
	}
	else
	{
		return format_word_hex(word);
	}
}

// print a .db line withe a number of bytes
void print_dbs(int count, unsigned char *bytes)
{
	char *argv[count];
	int i;
	for(i = 0; i < count; i++)
	{
		char byte = bytes[i];
		char *arg = format_byte_hex(byte);
		argv[i] = arg;
	}
	print_directive("db", count, argv);
}

// print a .db line with a single byte
void print_db(unsigned char c)
{
	unsigned char bytes[1];
	bytes[0] = c;
	print_dbs(1, bytes);
}

// print some help for the options nd stuff
void print_help()
{
	printf("Usage: gbdisasm [options] < infile > outfile\n\n");
	printf("Options:\n");
	printf("\t--help\t\t\tprint this list of options and usage\n");
	printf("\t--nocomment\t\tdisable printing comments at the end of lines\n");
	printf("\t--db\t\t\tdisasmble as data bytes\n");
	printf("\t--skip n\t\tcompletly ignore the first n (hex) input bytes\n");
	printf("\t--offset n\t\tdisassemble starting at n (hex) bytes\n");
	printf("\t--length n\t\tonly process n (hex) bytes\n");
	printf("\t--find\t\tonly find and print jump locations this pass\n");
	printf("\t--locations\t\tgive a series of locations to create labels for (must be last argument)\n");
}

// lol, yeah jst deal with em
void deal_with_labels()
{
	if(find) return;
	// if this is where a label goes, then put it
	int label_i;
	if((label_i = address_is_label(addr)) != -1)
	{
		printf("%s:\n", get_label_name(label_i));
	}
}

// okay, so like, first
// lets just
// read in some bytes
// and translate them to assembly code
// and spit them back out
int main(int argc, char *argv[])
{
	// init the options
	comments = 1; // yes use comments by default
	db = 0; // dont disassembly as .db directives by default
	skip = 0; // don't ignore any input bytes by default
	offset  = 0; // start at the beginning by default
	length = 0; // no limit by default
	find = 0; // not finding jumps unless specified, sorry!
	location_count = 0; // no locations unless specified!

	// loop through args
	int i;
	for(i = 1; i < argc; i++)
	{
		char *arg = argv[i];
		if(strcmp(arg, "--help") == 0)
		{
			print_help();
			return 0;
		}
		else if(strcmp(arg, "--nocomment") == 0)
		{
			comments = 0;
		}
		else if(strcmp(arg, "--db") == 0)
		{
			db = 1;
		}
		else if(strcmp(arg, "--find") == 0)
		{
			find = 1;
		}
		else if(strcmp(arg, "--skip") == 0)
		{
			// grab the next 'word' as the arg to this arg, haha
			char *value = argv[++i];
			char *end;
			skip = strtoumax(value, &end, 16);
		}
		else if(strcmp(arg, "--offset") == 0)
		{
			// grab the next 'word' as the arg to this arg, haha
			char *value = argv[++i];
			char *end;
			offset = strtoumax(value, &end, 16);
		}
		else if(strcmp(arg, "--length") == 0)
		{
			// grab the next 'word' as the arg to this arg, haha
			char *value = argv[++i];
			char *end;
			length = strtoumax(value, &end, 16);
		}
		else if(strcmp(arg, "--locations") == 0)
		{
			// this is how many locations were given
			location_count = argc - i - 1;

			// allocate the memory for the locations
			locations = (unsigned short *)malloc(location_count * sizeof(unsigned short));

			// loop through the locations and store them
			int j;
			for(j = 0; j < location_count; j++)
			{
				char *value = argv[++i];
				char *end;
				unsigned short location = strtoumax(value, &end, 16);
				locations[j] = location;
			}
		}
		else
		{
			fprintf(stderr, "Invalid option, \"%s\", yo\n", arg);
			return 1;
		}
	}

	// ignore the first n skipped bytes
	for(i = 0; i < skip; i++)
	{
		getchar();
	}

	// this is where the input will end being processed
	int end_pos = offset + length;

	// loop through all the bytes given by stdin
	byte_i = 0;
	int c;
	while((c = getchar()) != EOF)
	{
		// some temporary vars
		int high; // for holding the high bytes of words
		int low; // for holding the low bytes of words
		int arg; // for holding bytes or words
		int extended_byte; // for holding the extended byte of some opcode
		int label = -1; // for holding whether and which label this addr is

// spaghetti code :)))
RETRY:
		// the address of the current byte
		addr = byte_i++;

		// start and stop where specified
		if(byte_i <= offset) continue;
		if(length && byte_i >= end_pos + 1) break;

		// just print a .db line if thats what was asked for
		if(!find && db)
		{
			print_db(c);
			continue;
		}

		// store the first instruction byte
		instruction_byte_i = 0;
		instruction_bytes[instruction_byte_i++] = c;

		// here we go
		deal_with_labels();

		// here figure out which opcode this is
		switch(c)
		{

#include "instruction_parse.inc"

		// unknown opcode
		default:
			print_db(c);
			break;
		}

		// now find jumps if that's a thing
		if(find)
		{
			// lookin for jps, jrs, and calls
			switch(c)
			{
			// jp
			case 0xc2:
			case 0xc3:
			case 0xca:
			case 0xd2:
			case 0xda:
				//get_word();
				printf("%04x ", arg);
				break;

			// call
			case 0xc4:
			case 0xcc:
			case 0xcd:
			case 0xd4:
			case 0xdc:
				//get_word();
				printf("%04x ", arg);
				break;

			// jr
			case 0x18:
			case 0x20:
			case 0x28:
			case 0x30:
			case 0x38:
				//get_byte();
				printf("%04x ", relative_to_absolute_address(arg));
				break;
			}
		}
	}

	// done yo
	return EXIT_SUCCESS;
}
