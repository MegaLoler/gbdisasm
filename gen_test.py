#!/bin/python3

import sys

for x in range(256):
	sys.stdout.buffer.write(bytes([x, 0, 0]))
