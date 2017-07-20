// this is a lil program to extract raw header information from gameboy rom files :D

#include <stdio.h>
#include <string.h>

int ignore_null_bytes;
int format;

void print_help()
{
	printf("Usage: gbinfo [options] file\n\n");
	printf("Options:\n");
	printf("\t--format\t\tprint out hex formatted numbers instead of raw bytes\n");
	printf("\t--ignorenull\t\tdon't print null bytes\n");
	printf("\t--entry\t\t\textracts the 4 byte program entry code\n");
	printf("\t--logo\t\t\textracts the nintendo logo data\n");
	printf("\t--title\t\t\textracts the 11 byte title string\n");
	printf("\t--fulltitle\t\textracts the 16 byte title string\n");
	printf("\t--manufacturer\t\textracts the manufacturer code\n");
	printf("\t--cgb\t\t\textracts the color gameboy flag\n");
	printf("\t--newlicensee\t\textracts the new licensee code\n");
	printf("\t--sgb\t\t\textracts the super gameboy flag\n");
	printf("\t--type\t\t\textracts the cartridge type code\n");
	printf("\t--banks\t\t\textracts the number of rom banks\n");
	printf("\t--rom\t\t\textracts the rom size code\n");
	printf("\t--romsize\t\textracts the rom size value (in hex bytes)\n");
	printf("\t--ram\t\t\textracts the ram size code\n");
	printf("\t--ramsize\t\textracts the ram size value (in hex bytes)\n");
	printf("\t--dest\t\t\textracts the destination code\n");
	printf("\t--oldlicensee\t\textracts the old licensee code\n");
	printf("\t--version\t\textracts the mask rom version number\n");
	printf("\t--headerchecksum\textracts the header checksum\n");
	printf("\t--globalchecksum\textracts the global checksum\n");
}

unsigned char get_byte(FILE *file, int position)
{
	// go to the position for this byte
	fseek(file, position, SEEK_SET);

	// read and return the value there
	return fgetc(file);
}

void extract_bytes(FILE *file, int position, int length)
{
	// go to the starting place for these bytes
	fseek(file, position, SEEK_SET);
	int i;
	// and read out the specified amount of bytes
	for(i = 0; i < length; i++)
	{
		// read a byte and spit it out
		// grab the byte
		int c = fgetc(file);
		// only print null bytes if they aren't ignored
		if(!ignore_null_bytes || c != 0)
		{
			// print out the byte
			// format it as hex or print raw
			if(format)
			{
				printf("$%02x", c);
				if(i < length - 1) putchar(' ');
			}
			else
			{
				putchar(c);
			}
		}
	}
}

int main(int argc, char *argv[])
{
	// print null bytes unless otherwise specified
	ignore_null_bytes = 0;

	// print out raw bytes unless otherwise specified
	format = 0;

	// make sure there's some arguments, to begin with!
	if(argc < 2)
	{
		print_help();
		return 1;
	}

	// now let's grab the file name, which should be the last arg
	char *file_name = argv[argc - 1];

	// now let's try and open the file
	FILE *file;
	if((file = fopen(file_name, "rb")) == NULL)
	{
		fprintf(stderr, "Unable to open that file, \"%s\", yo\n", file_name);
		return 1;
	}

	// now loop through the options, if there are any, and for each
	// print out the relevant information
	int i;
	for(i = 1; i < argc - 1; i++)
	{
		// heres the arg
		char *arg = argv[i];

		// now let's see which one it is?
		if(strcmp(arg, "--ignorenull") == 0)
		{
			ignore_null_bytes = 1;
		}
		else if(strcmp(arg, "--format") == 0)
		{
			format = 1;
		}
		else if(strcmp(arg, "--entry") == 0)
		{
			extract_bytes(file, 0x100, 0x4);
		}
		else if(strcmp(arg, "--logo") == 0)
		{
			extract_bytes(file, 0x104, 0x30);
		}
		else if(strcmp(arg, "--title") == 0)
		{
			extract_bytes(file, 0x134, 0xb);
		}
		else if(strcmp(arg, "--fulltitle") == 0)
		{
			extract_bytes(file, 0x134, 0x10);
		}
		else if(strcmp(arg, "--manufacturer") == 0)
		{
			extract_bytes(file, 0x13f, 0x4);
		}
		else if(strcmp(arg, "--cgb") == 0)
		{
			extract_bytes(file, 0x143, 0x1);
		}
		else if(strcmp(arg, "--newlicensee") == 0)
		{
			extract_bytes(file, 0x144, 0x2);
		}
		else if(strcmp(arg, "--sgb") == 0)
		{
			extract_bytes(file, 0x146, 0x1);
		}
		else if(strcmp(arg, "--type") == 0)
		{
			extract_bytes(file, 0x147, 0x1);
		}
		else if(strcmp(arg, "--rom") == 0)
		{
			extract_bytes(file, 0x148, 0x1);
		}
		else if(strcmp(arg, "--ram") == 0)
		{
			extract_bytes(file, 0x149, 0x1);
		}
		else if(strcmp(arg, "--banks") == 0)
		{
			unsigned char rom_size_code = get_byte(file, 0x148);
			if(rom_size_code == 0x0)
			{
				printf("2");
			}
			else if(rom_size_code == 0x1)
			{
				printf("4");
			}
			else if(rom_size_code == 0x2)
			{
				printf("8");
			}
			else if(rom_size_code == 0x3)
			{
				printf("16");
			}
			else if(rom_size_code == 0x4)
			{
				printf("32");
			}
			else if(rom_size_code == 0x5)
			{
				printf("64");
			}
			else if(rom_size_code == 0x6)
			{
				printf("128");
			}
			else if(rom_size_code == 0x7)
			{
				printf("256");
			}
			else if(rom_size_code == 0x52)
			{
				printf("72");
			}
			else if(rom_size_code == 0x53)
			{
				printf("80");
			}
			else if(rom_size_code == 0x54)
			{
				printf("96");
			}
			else
			{
				printf("?");
			}
		}
		else if(strcmp(arg, "--romsize") == 0)
		{
			unsigned char rom_size_code = get_byte(file, 0x148);
			if(rom_size_code == 0x0)
			{
				printf("$8000");
			}
			else if(rom_size_code == 0x1)
			{
				printf("$10000");
			}
			else if(rom_size_code == 0x2)
			{
				printf("$20000");
			}
			else if(rom_size_code == 0x3)
			{
				printf("$40000");
			}
			else if(rom_size_code == 0x4)
			{
				printf("$80000");
			}
			else if(rom_size_code == 0x5)
			{
				printf("$100000");
			}
			else if(rom_size_code == 0x6)
			{
				printf("$200000");
			}
			else if(rom_size_code == 0x7)
			{
				printf("$400000");
			}
			else if(rom_size_code == 0x52)
			{
				printf("$120000");
			}
			else if(rom_size_code == 0x53)
			{
				printf("$140000");
			}
			else if(rom_size_code == 0x54)
			{
				printf("$180000");
			}
			else
			{
				printf("?");
			}
		}
		else if(strcmp(arg, "--ramsize") == 0)
		{
			unsigned char ram_size_code = get_byte(file, 0x149);
			if(ram_size_code == 0x0)
			{
				printf("$00");
			}
			else if(ram_size_code == 0x1)
			{
				printf("$800");
			}
			else if(ram_size_code == 0x2)
			{
				printf("$2000");
			}
			else if(ram_size_code == 0x3)
			{
				printf("$80000");
			}
			else
			{
				printf("?");
			}
		}
		else if(strcmp(arg, "--dest") == 0)
		{
			extract_bytes(file, 0x14a, 0x1);
		}
		else if(strcmp(arg, "--oldlicensee") == 0)
		{
			extract_bytes(file, 0x14b, 0x1);
		}
		else if(strcmp(arg, "--version") == 0)
		{
			extract_bytes(file, 0x14c, 0x1);
		}
		else if(strcmp(arg, "--headerchecksum") == 0)
		{
			extract_bytes(file, 0x14d, 0x1);
		}
		else if(strcmp(arg, "--globalchecksum") == 0)
		{
			extract_bytes(file, 0x14e, 0x2);
		}
		else
		{
			fprintf(stderr, "Invalid option, \"%s\", yo\n", arg);
			return 1;
		}
	}

	// done, so close the file and exit
	fclose(file);

	return 0;
}
