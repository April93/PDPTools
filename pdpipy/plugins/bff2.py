from hacklib import readValue

class bff2:
	"""BFF2 file class"""
	filetype = "image"
	filename = ""
	offset = 0
	headerdata = []
	imgtype = 0
	height = 0
	rawwidth = 0
	width = 0
	colorSize = 0
	namelength = 0
	name = ""
	colornum = 0
	colors = []
	charnum24 = 0
	charidx24 = []
	charwidths24 = []
	charheights24 = []
	imagesize = 0
	imgbuf = []
	decompressedimgbuf = []
	compressed = 0

	rawimage = []

	types = {
		0x55: "RGBA8888",
		0x54: "RGBA5551",
		0x33: "8bpp",
		0x32: "L8/Shadow",
		0x24: "LA8",
		0x23: "LA4/Font/1bpp",
		0x22: "LA4? 1bpp? Unknown"
	}

	#Initializes and reads binary data
	def __init__(self, data, **kwargs):
		filename = kwargs.get("filename", "")
		compressed = kwargs.get("compressed", 0)
		offset = kwargs.get("offset", 0)
		#print ("Created BFF2!")
		headermatch = (data[0x00:0x04] == [0x42,0x46,0x46,0x32]) #BFF2
		if headermatch == False:
			print ("Not a BFF2 file!")
			return
		index = 4 #Skipping magic bytes since we already know
		#self.compressed = compressed
		self.offset = offset
		#print "BFF2 Offset:",self.offset

		#Get type of BFF2
		self.headerdata = data[index:index+8] #Not sure what this data does. not important?
		self.imgtype = data[index+3]
		print (self.headerdata, data[index+2])
		print (format(data[index+2], '#010b'))
		self.compressed = int(format(data[index+2], '#010b')[6])
		index += 8
		#print "BFF2 Type:",hex(self.imgtype), self.headerdata

		#Height/Width
		self.height = readValue(data,index,1)
		index += 4
		self.rawwidth = readValue(data,index,1)
		index += 4
		# print "Raw Height/Width:", height, rawwidth

		#Adjust Width/ColorSize based on BFF2 type
		self.width = self.rawwidth
		self.colorSize = 1;
		if self.imgtype == 0x55: #RGBA8888
			self.width = int(self.rawwidth/4)
			self.colorSize = 4
		if self.imgtype == 0x54: #RGBA5551
			self.width = int(self.rawwidth/2)
			self.colorSize = 2
		if self.imgtype == 0x24: #LA8
			self.width = int(self.rawwidth/2)
			self.colorSize = 2
		#print "Height/Width/ColorSize:", self.height, self.width, self.colorSize

		#Get BFF2 name length + name
		self.namelength = readValue(data,index,1)
		index += 4
		#print "BFF2 Name Length:",self.namelength

		self.name = data[index:(index+self.namelength)]
		self.name = ''.join(chr(ch) for ch in self.name)
		index += self.namelength
		#print self.name

		#Image data starts here I think?

		#If BFF2 is type 8bpp (0x33), Get Palette
		if self.imgtype == 0x33 or self.imgtype == 0x32:
			self.colornum = readValue(data,index,1)
			index += 4
			#print "Colors:", self.colornum

			self.colors = []
			for i in range(0,self.colornum):
				color = data[index:index+4]
				self.colors.append(color)
				index += 4
			#print "Colors fetched: ", len(self.colors)

		if self.imgtype == 0x24:
			#self.compressed = 0
			self.charnum24 = readValue(data, index, 1)
			index += 4
			print ("charnum24:", self.charnum24)
			#Something went wrong and the image doens't have a char table
			#despite being 0x24
			if self.charnum24 >= 2048:
				self.charnum24 = 0
				index -= 4
			else:
				self.charidx24 = []
				self.charwidths24 = []
				self.charheights24 = []
				for i in range(0,self.charnum24):
					#Read Character Metadata
					charmeta = readValue(data, index, 1)
					metadata = '{:032b}'.format(charmeta)

					#Index of Character image data is First 20 bits
					#Height is the next 6 bits
					#Width is the last 6 bits
					#As nibbles it looks like idx(#####), H/W(###)
					charidx = int(metadata[:20],2)
					charwidth = int(metadata[-6:],2)
					charheight = int(metadata[-12:-6],2)
					# print ("charidx:",charidx, int(charidx,2))
					# print ("charwidth:",charwidth, int(charwidth,2))
					# print ("charheight:",charheight, int(charheight,2))
					self.charidx24.append(charidx)
					self.charwidths24.append(charwidth)
					self.charheights24.append(charheight)
					index += 4
				#print ("charwidths24:", self.charwidths24)

		self.imagesize = len(data)-index
		#print "Image Size: ", self.imagesize

		self.imgbuf = data[index:index+self.imagesize]

	def printvals(self):
		print ("Offset:", self.offset)
		print ("Name:", self.name)
		print ("Name Length:", self.namelength)
		print ("Header Data:", self.headerdata)
		print ("Image Type:", hex(self.imgtype), self.types.get(self.imgtype))
		print ("Height:", self.height)
		print ("Raw Width:", self.rawwidth)
		print ("Width:", self.width)
		print ("Color Size:", self.colorSize)
		print ("Number of Colors:", self.colornum)
		print ("Charnum24:", self.charnum24)
		# print ("charwidths24:", self.charwidths24)
		#print( self.colors)
		print ("Image Size:", self.imagesize)
		print ("Compressed:", self.compressed)
		#print( self.imgbuf)
		return

	#Decompresses the imgbuf
	def decompress(self):
		imgindex = 0
		compcmd = 0
		bytecnt = 0
		bytemax = -1
		loop = -1
		tmpcmd = 0

		newbsize = self.width * self.height * self.colorSize
		newb = []
		j = 0

		#If bff2 isn't compressed, then just return the imgbuf
		if self.compressed != 1:
			self.decompressedimgbuf = self.imgbuf
			return self.decompressedimgbuf

		while imgindex < self.imagesize:
			#If reached end of command, get a new one.
			if bytecnt >= bytemax:
				if loop <= 0:
					loop = 0
					tmpcmd = self.imgbuf[imgindex]
					bytecnt = 0
					bytemax = (tmpcmd&0x0F)
					compcmd = (tmpcmd&0xF0)>>4
					#If first nibble is above 8, loop.
					if compcmd >= 8:
						#If first nibble is between 8 and 0xB, loop a single pixel (tmpcmd-0x80)+1 times.
						if compcmd <= 0xB:
							#Set the amount of times to loop the pixel
							loop = (tmpcmd-0x80)+1
							#How many pixels looped? 1, obviously.
							bytemax = 1

						#If the first nibble is above 0xB, loop multiple pixels a certain amount of times.
						else:
							#Set the amount of times to loop the set of pixels
							#The amount of times to loop can be between 1 and 8 (represented by values 0-7).
							#This is due to when the value is 9-F, the amount of pixels is increased by 1 (and then loops between 1-8 times)
							loop = (bytemax%8)+1
							#The amount of pixels to loop. C=2, D=4, E=5, F=6. +1 if cntmax (2nd nibble) is 8 or above.
							addone = 0
							if bytemax >= 8:
								addone += 1
							bytemax = ((compcmd-0xB)*2)+addone
					else:
						#No loop, just draw pixels as normal.
						#Set the amount of pixels to draw normally
						bytemax=tmpcmd+1
						loop = 0
					#int cbyte = encodeCommandByte(bytemax, loop);
					#cout << tmpcmd << ", " << cbyte << endl;
					imgindex+=1;
				else:
					loop-=1
					bytecnt = 0
					imgindex-= bytemax
			if imgindex > (self.imagesize - 1):
				break;
			if j > (newbsize - 1):
				break;
			pixelbyte = self.imgbuf[imgindex]
			imgindex+=1
			newb.append(pixelbyte)
			j+=1
			bytecnt+=1
		self.decompressedimgbuf = newb
		#print(self.decompressedimgbuf)
		return newb;

	#Converts imgbuf into array of rgba values, decompresses if needed
	#Returns an array of rgba/color arrays one per pixel
	def exportImage(self):
		if self.decompressedimgbuf == []:
			self.decompress()
		imgin = 0
		image = []
		print (len(self.decompressedimgbuf), self.width*self.height, self.width, self.height)

		#Handle 0x24 images differently
		if self.imgtype == 0x24 and self.charnum24 > 0:
			#Fix width/height for display
			totalwidth = 0
			for c in range(0, self.charnum24):
				totalwidth += self.charwidths24[c]
				if self.height < self.charheights24[c]:
					self.height = self.charheights24[c]
			if self.width < totalwidth:
				self.width = totalwidth
			image = [[0,0,0,0]] * (self.width*self.height)
			print (len(image))

			#Go through each letter
			cw = 0
			for c in range(0, self.charnum24):
				imgin = self.charidx24[c]
				for y in range (0, self.charheights24[c]):
					for x in range (0, self.charwidths24[c]):
						if imgin+1 >= len(self.decompressedimgbuf):
							imgin = len(self.decompressedimgbuf)-2
						shade = self.decompressedimgbuf[imgin];
						alpha = self.decompressedimgbuf[imgin+1];
						#print (x, y, c, self.charwidths24[c], self.charheights24[c], (y*self.width)+x+cw)
						image[(y*self.width)+x+cw] = [shade,shade,shade,alpha];
						imgin+=2
				cw += self.charwidths24[c]
			self.rawimage = image
			return image

		for y in range (0, self.height):
			for x in range(0, self.width):
			#while imgin < len(self.decompressedimgbuf):
				if self.imgtype == 0x55: #RGBA8888
					if imgin+3 >= len(self.decompressedimgbuf):
						break
					r = self.decompressedimgbuf[imgin];
					g = self.decompressedimgbuf[imgin+1];
					b = self.decompressedimgbuf[imgin+2];
					a = self.decompressedimgbuf[imgin+3];
					image.append([r,g,b,a]);
					imgin+=4;

				if self.imgtype == 0x54: #RGBA5551
					if imgin+1 >= len(self.decompressedimgbuf):
						break
					val1 = self.decompressedimgbuf[imgin];
					val2 = self.decompressedimgbuf[imgin+1];
					r = val1>>3;
					g = ((val1&7)<<2)+((val2&0xC0)>>6);
					b = (val2&0x3E)>>1;
					r = (r/31)*255;
					g = (g/31)*255;
					b = (b/31)*255;
					image.append([int(r),int(g),int(b),int((val2&1)*255)])
					imgin+=2;

				if self.imgtype == 0x33 or self.imgtype == 0x32: #8bpp
					if imgin >= len(self.decompressedimgbuf):
						imgin = len(self.decompressedimgbuf)-1
					if self.decompressedimgbuf[imgin] < len(self.colors):
						image.append(self.colors[self.decompressedimgbuf[imgin]]);
					else:
						image.append(self.colors[self.decompressedimgbuf[imgin]%len(self.colors)]);
						#print (self.decompressedimgbuf[imgin])
					imgin+=1;

				#For 0x24 images without a char table
				if self.imgtype == 0x24: #LA8
					if imgin+1 >= len(self.decompressedimgbuf):
						imgin = len(self.decompressedimgbuf)-2
					shade = self.decompressedimgbuf[imgin];
					alpha = self.decompressedimgbuf[imgin+1];
					image.append([shade,shade,shade,alpha]);
					imgin+=2;

				if self.imgtype == 0x23 or self.imgtype == 0x22: #Font/1bpp/LA4
					if imgin >= len(self.decompressedimgbuf):
						imgin = len(self.decompressedimgbuf)-1
					pixel = self.decompressedimgbuf[imgin];
					alpha = (pixel&0x0F)*17;
					whiteblack = ((pixel&0xF0)>>4)*17;
					if(imgin >= self.width * self.height):
						whiteblack = 0;
						alpha = 0;
					image.append([whiteblack,whiteblack,whiteblack,alpha]);
					imgin+=1;
		self.rawimage = image
		return image