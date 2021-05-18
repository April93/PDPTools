from hacklib import readValue

class bin:
	"""BIN file class"""
	filetype = "archive"
	filename = ""
	filesnum = 0
	files = []
	filenames = []
	filedata = []

	#Initializes and reads binary data
	def __init__(self, data, **kwargs):
		filename = kwargs.get("filename", "")
		self.filename = filename
		self.filesnum = 0
		self.files = []
		self.filenames = []
		self.filedata = []

		#Check number of files
		self.filesnum = readValue(data,0x00,1)
		#print ("Files:",self.filesnum)

		#Files
		self.files = []
		for b in range(0, self.filesnum):
			f = []

			#Size
			f.append(readValue(data,(b*24)+4,1))
			#Address
			f.append(readValue(data,(b*24)+8,1))
			#Name
			fname = data[((b*24)+12):(((b*24)+12)+16)]
			fname = ''.join(chr(ch) for ch in fname)
			fname = fname.rstrip()
			f.append(fname)
			self.filenames.append(fname)
			self.files.append(f)
			self.filedata.append(data[f[1]:f[1]+f[0]])
			#print ("File: ", f, len(self.filedata[b]))

		#print (len(self.files))