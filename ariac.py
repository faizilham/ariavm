#!/usr/bin/env python

"""
	a sample aria "assembly" code (*.ars) converter to aria executable code (*.arx)
"""

import sys, struct
from aria_inst import *

if (len(sys.argv) != 2):
	sys.exit(1)
	
fname = sys.argv[1]

try:
	fname = fname[:fname.index(".ars")]
except:
	sys.exit(1)
	
code = []
label = {}
labelpos = []

with open(sys.argv[1], 'r') as input:
	for line in input:
		token = line.strip().split()
		if (token and not token[0].startswith('#')):
			if token[0].startswith('.'):
				label[token[0]] = struct.pack("<i", len(code))
			else:
				code.append(struct.pack("<b", instruction_map[token[0].upper()]))
			
			if (len(token) > 1 and token[1]):
				if (token[1].startswith('.')):
					code.append(token[1])
					code.append('\0')
					code.append('\0')
					code.append('\0')
				else:
					for char in struct.pack("<i", int(token[1])):
						code.append(char)

with open("{0}.arx".format(fname), 'wb') as output:
	n = 0
	for c in code:
		if n > 0:
			n -= 1
			continue
		if (not c.startswith('.')) or (c not in label):
			output.write(c)
		else:
			for b in label[c]:
				output.write(b)
			n = 3