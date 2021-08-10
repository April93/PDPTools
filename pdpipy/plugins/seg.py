from hacklib import readValue
import os.path

class seg:
	"""seg file class"""
	filetype = "image"
	filename = ""
	offset = 0
	headerdata = []
	imgtype = ""
	height = 224
	width = 320
	imgbuf = []

	#Specific kinds of images in seg
	bg2dimg = []
	bg2dheight = 224
	bg2dwidth = 320
	bg2dlen = 143360
	portraitimg = []
	portraitwidth = 128
	portraitheight = 192

	def readBG2D(self, offset, data, height=224, width=320):
		self.bg2dimg = []
		imgin = offset
		while imgin <= height*width*2:
			if imgin+1 >= len(data):
				break
			val1 = data[imgin];
			val2 = data[imgin+1];
			r = val1>>3;
			g = ((val1&7)<<2)+((val2&0xC0)>>6);
			b = (val2&0x3E)>>1;
			r = (r/31)*255;
			g = (g/31)*255;
			b = (b/31)*255;
			self.bg2dimg.append([int(r),int(g),int(b),int((val2&1)*255)])
			imgin+=2;
		return self.bg2dimg


	#Initializes and reads binary data
	def __init__(self, data, **kwargs):
		filename = kwargs.get("filename", "")
		self.offset = 0
		path, filename = os.path.split(filename)
		print (filename)
		self.imgbuf = []

		if filename[0:4] == "lev_":
			imgtype = "lev"
			print ("lev")
		if filename[0:4] == "bg2d":
			imgtype = "bg2d"
			print ("bg2d")
		if filename[0:6] == "pon_2p":
			imgtype = "pon_2p"
			print ("pon_2p")
		if filename[0:9] == "pon_intro":
			imgtype = "pon_intro"
			print ("pon_intro")
		if filename[0:9] == "pon_2d_1p":
			imgtype = "pon_2d_1p"
			print ("pon_2d_1p")

		if imgtype == "pon_2d_1p":
			self.readBG2D(0x12400,data,350,320)
			self.imgbuf = self.bg2dimg
			self.height = 350
			self.width = 320
		if imgtype == "pon_intro":
			self.readBG2D(0,data)
			self.imgbuf = self.bg2dimg
		if imgtype == "pon_2p":
			data = data[0x6800:]
		if imgtype == "lev" or imgtype == "pon_2p":
			print ("reading lev")
			headerdata = data[0:16]
			sections = []

			#Fetch palette sizes
			for i in range(0,8):
				valarray = (headerdata[(i*2):(i*2)+2])
				val = "".join(['{0:0{1}X}'.format(item,2) for item in valarray])
				val = int(val,16)
				sections.append(val*2)
			print (sections)
			
			#Total size of header+palettes
			headerlen = 0x10+sections[0]+sections[1]+sections[2]+sections[3]+sections[4]+sections[5]+sections[6]+sections[7]
			imgin = headerlen
			portraitheaderlen = 264 #flower
			portraitheaderlen = 344 #cloud
			portraitheaderlen = 240 #devil
			imgin += portraitheaderlen

			#Fetch portrait
			while imgin <= len(data):
				if imgin+1 >= len(data):
					break
				val = data[imgin]
				val1=data[0x10+(val*2)]
				val2=data[0x10+(val*2)+1]
				r = val1>>3;
				g = ((val1&7)<<2)+((val2&0xC0)>>6);
				b = (val2&0x3E)>>1;
				r = (r/31)*255;
				g = (g/31)*255;
				b = (b/31)*255;
				self.imgbuf.append([int(r),int(g),int(b),int((val2&1)*255)])
				imgin+=1;

			#Get the width and height of the portrait
			valarray = (data[headerlen:headerlen+2])
			val = "".join(['{0:0{1}X}'.format(item,2) for item in valarray])
			self.width = int(val,16)
			valarray = (data[headerlen+2:headerlen+4])
			val = "".join(['{0:0{1}X}'.format(item,2) for item in valarray])
			self.height = int(val,16)
			print (self.height, self.width)

		if imgtype == "bg2d":
			print ("reading bg2d")
			self.readBG2D(0,data)
			self.imgbuf = self.bg2dimg
			
		#self.imgbuf = data[index:index+self.imagesize]

	def printvals(self):
		return

	#Converts imgbuf into array of rgba values, decompresses if needed
	#Returns an array of rgba/color arrays one per pixel
	def exportImage(self):
		return self.imgbuf