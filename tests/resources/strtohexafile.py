 #! /usr/bin/env python

import binascii
import sys

#text_file = open(sys.argv[0], "r")
str = sys.argv[2]
#for line in text_file:
#    str = str + line
#text_file.close()

#str = '0xE7 0x00 0x00 0xA8 0x00'
strHexa_values = str.split(' ')

hs = str.replace('0x', '').replace(' ', '')

hb=binascii.a2b_hex(hs)

file = open(sys.argv[1],"wb")
file.write(hb)
file.close()


sys.exit(0)
