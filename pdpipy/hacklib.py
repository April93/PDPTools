#Read Bytes into array
def readBytes(filename):
	output = []
	with open(filename, "rb") as f:
		byte = f.read(1)
		while byte:
			output.append(ord(byte))
			byte = f.read(1)
	return output

#Save bytes to file
def saveBytes(filename, bytes):
	file = open(filename,"w")
	bytearrayfixed = bytearray(bytes)
	file.write(bytearrayfixed)
	file.close()

#Little endian value reading
def readValue(array, offset, be=0):
	if be == 1:
		valarray = (array[offset:offset+0x4])
	else:
		valarray = (array[offset:offset+0x4])[::-1]
	val = "".join(['{0:0{1}X}'.format(item,2) for item in valarray])
	return int(val,16)