#!/usr/bin/python

import sys

isInUnion = 0
lastUnionLine = 0
isInStruct = 0
name = ''
structs = {}
function_mapping = {'intu8':'read_intu8(stream)', 'intu16':'read_intu16(stream)', 'intu32':'read_intu32(stream)'}

def print_name(value):
	isMalloc = 0
	newName = ''

	if (value[0] == 'UNION'):
		print '    // UNION'
		return
	
	x = value[1].find('[')
	if (x != -1):
		newName = value[1][0:x]
		ret = '    pointer->' + newName + ' = '
	else:
		ret = '    pointer->' + value[1] + ' = '
	
	if (value[1].find('[') != -1):
		ret += 'malloc(pointer->count*sizeof(' + value[0] + ')); // ' + value[0] + ' ' + value[1]
		isMalloc = 1	
	elif (value[0] in structs):
		ret = '    decode_' + value[0].lower() + '(stream, &pointer->' + value[1] + '); // ' + value[0] + ' ' + value[1]	
	else:
		if value[0] in function_mapping.keys():
			ret += function_mapping[value[0]] + '; // ' + value[0] + ' ' + value[1]
			
	if isMalloc == 1:
		isMalloc = 0
		ret += '\n'
		ret += '    int i;\n'
		ret += '    for (i = 0; i < pointer->count; i++) {\n'
		ret += '        decode_' + value[0].lower() + '(stream, pointer->' + newName + ' + i);' + '\n'
		ret += '     }\n'

	print ret;

def check_union(line):
	global isInUnion
	global isInStruct
	global lastUnionLine
	global name
	global structs


	if (lastUnionLine == 1):
		if isInStruct == 1 and isInUnion == 1:
			tmp = ('UNION', '')
			structs[name].append(tmp)
			
		isInUnion = 0
		lastUnionLine = 0
		return
	
	if (isInUnion == 1):
		if (line.find('}') != -1):
			lastUnionLine = 1

	elif (isInUnion == 0 and line.find('union') != -1):
		isInUnion = 1


def check_structs(line):
	global isInStruct
	global name

	if (isInStruct == 1):
		if (line.find('}') != -1):
			name = '';
			isInStruct = 0
		else:
			if (line.split()[0].find('#') == -1):
				tmp = (line.split()[0], line.split()[1][0:-1])
				structs[name].append(tmp)

	if (isInStruct == 0 and line.find('typedef struct') != -1):
		isInStruct = 1;
		name = line.split()[2]
		structs[name] = []	

def check_typedefs(line):
	if (line.find('typedef intu8') == 0):
		function_mapping[line.split()[2][0:-1]] = 'read_intu8(stream)'
	elif (line.find('typedef intu16') == 0):
		function_mapping[line.split()[2][0:-1]] = 'read_intu16(stream)'
	elif (line.find('typedef intu32') == 0):
		function_mapping[line.split()[2][0:-1]] = 'read_intu32(stream)'
	elif (line.find('typedef ') == 0):
		tmp = line.split()[1]
		if tmp in function_mapping:
			function_mapping[line.split()[2][0:-1]] = function_mapping[tmp];

def print_c_file():
	for key, value in structs.iteritems():
		print 'void decode_' + key.lower() + '(ByteStream *stream, ' + key + ' *pointer) {'
		for attr in value:
			print_name(attr)
		print '}'
		print ''	

def print_h_file():
	for key, value in structs.iteritems():
		print 'void decode_' + key.lower() + '(ByteStream *stream, ' + key + ' *pointer);'	

if __name__ == "__main__":
	fileName = sys.argv[1]
	f = open(fileName, 'r')

	global isInUnion

	for line in f:
		check_union(line)

		if isInUnion == 0:
			check_structs(line)

		# check the typedefs
		check_typedefs(line)
		
	print_c_file()

		


