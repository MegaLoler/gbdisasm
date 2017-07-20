#!/bin/python3

import sys

line_prefix = "\t\t"
line_prefix_2 = line_prefix + "\t"
nest_prefix = ""

opcode_prefix = None

last_opcode = None

for line in sys.stdin:
	parts = line.strip().split(" ", 1)
	opcode = parts[0][1:].lower()
	extended_opcode = None
	mnemonic = parts[1][1:-1].lower()

	if opcode == last_opcode:
		continue
	last_opcode = opcode

	if len(opcode) > 2:
		extended_opcode = opcode[2:]
		opcode = opcode[:2]

	if extended_opcode != None and opcode_prefix == None:
		print(nest_prefix + line_prefix + "// opcode prefix: 0x" + opcode)
		print(nest_prefix + line_prefix + "case 0x" + opcode + ":")
		nest_prefix = "\t"
		opcode_prefix = opcode
		print(nest_prefix + line_prefix + "get_extended_byte();")
		print(nest_prefix + line_prefix + "switch(extended_byte)")
		print(nest_prefix + line_prefix + "{")
	elif opcode_prefix != None and extended_opcode == None:
		print(nest_prefix + line_prefix + "// unknown opcode")
		print(nest_prefix + line_prefix + "default:")
		print(nest_prefix + line_prefix_2 + "print_db(extended_byte);")
		print(nest_prefix + line_prefix_2 + "break;")
		print(nest_prefix + line_prefix + "}")
		print(nest_prefix + line_prefix + "break;")
		print()
		nest_prefix = ""
		opcode_prefix = None

	if extended_opcode != None:
		opcode = extended_opcode

	comment_line = "// " + mnemonic
	case_line = "case 0x" + opcode + ":"
	break_line = "break;"

	print(nest_prefix + line_prefix + comment_line)
	print(nest_prefix + line_prefix + case_line)

	parts = mnemonic.split()
	mnemonic_name = parts[0]
	if len(parts) > 1:
		mnemonic_args = parts[1].split(",")
	else:
		mnemonic_args = []
	mnemonic_args_count = len(mnemonic_args)

	# this is for calculating how many tabs to put before comments
	mnemonic_arg = ", ".join(mnemonic_args)
	mnemonic_arg = mnemonic_arg.replace("(?)", "($0000)")
	mnemonic_arg = mnemonic_arg.replace("?", "$0000")
	mnemonic_arg = mnemonic_arg.replace("x", "$00")
	mnemonic_arg = mnemonic_arg.replace("sps", "__+$00")
	mnemonic_arg = mnemonic_arg.replace("sp", "__")
	#if mnemonic_name == "jr":
	#	mnemonic_arg = mnemonic_arg.replace("s", "$0000")
	#else:
	mnemonic_arg = mnemonic_arg.replace("s", "$00")

	instruction_size = 1
	for i in range(mnemonic_args_count):
		arg = mnemonic_args[i]
		if arg == "(?)":
			instruction_size = 3
			mnemonic_args[i] = "format_indirect_address_hex(arg)"
			print(nest_prefix + line_prefix_2 + "get_word();")
		elif arg == "?":
			instruction_size = 3
			mnemonic_args[i] = "resolve_address(arg)"
			print(nest_prefix + line_prefix_2 + "get_word();")
		elif arg == "x":
			instruction_size = 2
			mnemonic_args[i] = "format_byte_hex(arg)"
			print(nest_prefix + line_prefix_2 + "get_byte();")
		elif arg == "s":
			instruction_size = 2
#			if mnemonic_name == "jr":
#				mnemonic_args[i] = "resolve_address(relative_to_absolute_address(arg))"
#			else:
			mnemonic_args[i] = "format_signed_byte_hex(arg)"
			print(nest_prefix + line_prefix_2 + "get_byte();")
		elif arg == "($ff00+x)":
			instruction_size = 2
			mnemonic_args[i] = "format_high_address_hex(arg)"
			print(nest_prefix + line_prefix_2 + "get_byte();")
		elif arg == "(x)":
			instruction_size = 2
			mnemonic_args[i] = "format_indirect_high_address_hex(arg)"
			print(nest_prefix + line_prefix_2 + "get_byte();")
		elif arg == "sp+s":
			instruction_size = 2
			mnemonic_args[i] = "format_relative_sp_hex(arg)"
			print(nest_prefix + line_prefix_2 + "get_byte();")
		else:
			mnemonic_args[i] = "\"" + arg + "\""
	mnemonic_name = "\"" + mnemonic_name + "\""
	if extended_opcode != None: instruction_size += 1

	print_args = ", ".join([mnemonic_name] + mnemonic_args)
	print_function = "print_instruction_" + str(mnemonic_args_count) + "_args"
	print_line = print_function + "(" + print_args + ");"

	extra_tab = True
	if len(mnemonic_arg) > 7: extra_tab = False

	print_tab_line = "if(!find) printf(\"\\t\");"
	print_comment_line = "print_comment(" + str(instruction_size) + ");"
	print_end_line = "if(!find) printf(\"\\n\");"
	
	print(nest_prefix + line_prefix_2 + print_line)
	if extra_tab: print(nest_prefix + line_prefix_2 + print_tab_line)
	print(nest_prefix + line_prefix_2 + print_comment_line)
	print(nest_prefix + line_prefix_2 + print_end_line)
	print(nest_prefix + line_prefix_2 + break_line)
	print()
