from hacklib import readValue
import plugins.bff2

class bif:
	"""BIF file class"""
	filetype = "anim"
	filename = ""
	compressed = 0
	filesnum = 0
	namelength = 0
	name = ""
	offsets = []
	fixedOffsets = []
	images = []
	timings = []

	#Initializes and reads binary data
	def __init__(self, data, **kwargs):
		filename = kwargs.get("filename", "")
		#Check Header
		headermatch = (data[0x00:0x04] == [0x33,0x46,0x49,0x42]) #3FIB
		if headermatch == False:
			print ("Not a BIF file!")
			return
		print ("Proper header!")

		#Check if compressed
		self.compressed = data[0x05]
		print ("Compressed",self.compressed)

		#Check number of BFF2s in file
		self.filesnum = readValue(data,0x0C)
		print ("BFF2s:",self.filesnum)

		#Get length of internal name
		self.namelength = readValue(data,0x10)
		print ("Name Length:",self.namelength)

		#Get the internal name
		self.name = data[0x14:(0x14+self.namelength)]
		self.name = ''.join(chr(ch) for ch in self.name)
		print (self.name)

		#Get offsets of BFF2 magic bytes in files (where BFF2s are located)
		BFF2Magic = [0x42,0x46,0x46,0x32]
		self.offsets = [x for x in range(len(data)) if data[x:x+len(BFF2Magic)] == BFF2Magic]
		print ("BFF2 offsets:",self.offsets)
		self.fixedOffsets = [(x-12) for x in self.offsets]
		print ("Fixed BFF2 Offsets:",self.fixedOffsets)

		self.offsets.append(len(data))
		self.fixedOffsets.append(len(data))

		#BFF2s
		self.images = []
		self.timings = []
		for b in range(0, len(self.offsets)-1):
			self.timings.append(data[self.fixedOffsets[b]:self.fixedOffsets[b]+12])
			bff2data = data[self.offsets[b]:self.fixedOffsets[b+1]]
			mBFF2 = plugins.bff2.bff2(bff2data, compressed=self.compressed, offsets=self.offsets[b])
			self.images.append(mBFF2)

		print (self.timings)
		print (len(self.images))