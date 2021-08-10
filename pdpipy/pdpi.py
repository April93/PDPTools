import sys
import binascii
import math
import os.path
from hacklib import *
import plugins
import PIL
from PIL import Image, ImageQt
from PyQt5.QtWidgets import *
from PyQt5.QtGui import QIcon, QPixmap

#Initializes GUI
app = QApplication([])
window = QWidget()
layout = QVBoxLayout()
label = QLabel()
listwidget = QListWidget()
window.setWindowTitle("PDPI")
window.setGeometry(100,100,200,50)

loc = 0

#Draws a rawimage to an PIL image object
def drawImage(rawimage, width, height, xoff=5,yoff=5):
	y = 0
	x = 0
	img = PIL.Image.new('RGBA', (int(width), int(height)))
	for pixel in rawimage:
		if y >= height or x >= width:
			break
		img.putpixel((x, y), (pixel[0],pixel[1],pixel[2],pixel[3]))
		x += 1
		if x >= width:
			x = 0
			y += 1
	return img

#Generates a PIL image from a rawimage (using drawImage()) and then displays it on the gui
def displayImage(imagedata, w, h):
	pilImage = drawImage(imagedata, w, h)
	img = ImageQt.ImageQt(pilImage)
	pixmap = QPixmap.fromImage(img)
	label.setPixmap(pixmap)

#Exports rawimage data to a png
def exportImage(name, imagedata, w, h):
	pilImage = drawImage(imagedata,w,h)
	exportname = name.replace('\00', '')
	if exportname == "":
		exportname = "export"
	exportname = exportname+".png"
	print("Exporting "+exportname)
	pilImage.save(exportname)

BIF = None
def exportBFF2():
	global loc
	exportImage(BIF.images[loc].name,BIF.images[loc].exportImage(),BIF.images[loc].width,BIF.images[loc].height)


#Loads an animation file and draws the first BFF2 to the window
def loadAnim(loadedFile):
	global loc
	global BIF
	loc = 0
	BIF = loadedFile
	BIF.images[loc].printvals()
	displayImage(BIF.images[loc].exportImage(),BIF.images[loc].width,BIF.images[loc].height)
	imgwin.show()
def loadBIFData(data, name):
	global loc
	global BIF
	loc = 0
	extension = "bif"
	BIF = getattr(getattr(plugins, extension),extension)(data, filename=name)
	print (name)
	BIF.images[loc].printvals()
	displayImage(BIF.images[loc].exportImage(),BIF.images[loc].width,BIF.images[loc].height)
	imgwin.show()

img = None
def loadImage(loadedFile):
	global loc
	loc = 0
	global img
	img = None
	img = loadedFile
	img.printvals()
	displayImage(img.exportImage(),img.width,img.height)
	imgwin.show()

#Loads an archive file into the gui list
BIN = None
def loadArchive(loadedFile):
	global listwidget
	listwidget.clear()
	global BIN
	BIN = None
	BIN = loadedFile
	for f in range(0,len(BIN.files)):
		listwidget.addItem(BIN.filenames[f])
	#loadBIFData(BIN.filedata[1])
	listwidget.show()

#Loads a scene file
SBF = None
def loadScene(loadedFile):
	global SBF
	SBF = None
	SBF = loadedFile
def loadSceneData(data, name):
	global SBF
	SBF = None
	extension = "sbf"
	SBF = getattr(getattr(plugins, extension),extension)(data, filename=name)

loadedFile = None
def loadFile(self):
	options = QFileDialog.Options()
	options |= QFileDialog.DontUseNativeDialog
	filetypes = "All Panel de Pon Files (*.BIF *.SEG *.BIN *.SBF *.NPF *.NP2 *.NP3 *.NMF *.RDF)"
	filetypes += ";;Image Files (*.BIF *.SEG)"
	filetypes += ";;BIN Archives (*.BIN);;SBF Script Files (*.SBF)"
	fileName, _ = QFileDialog.getOpenFileName(window,"QFileDialog.getOpenFileName()", "",filetypes, options=options)
	if fileName:
		#Generic File Load
		print ("Loading file...")
		data = readBytes(fileName)
		print ("File loaded")
		extension = (os.path.splitext(fileName)[1]).lower()[1:]
		loadedFile = getattr(getattr(plugins, extension),extension)(data, filename=fileName)
		print (loadedFile.filetype)

		#Perform Action based on Filetype
		if loadedFile.filetype == "archive":
			loadArchive(loadedFile)
		if loadedFile.filetype == "anim":
			loadAnim(loadedFile)
		if loadedFile.filetype == "image":
			loadImage(loadedFile)
			#displayImage(loadedFile.exportImage(),loadedFile.width,loadedFile.height)
		if loadedFile.filetype == "scene":
			loadScene(loadedFile)

def importImage(self):
	options = QFileDialog.Options()
	options |= QFileDialog.DontUseNativeDialog
	filetypes = "Image Files (*.jpg *.png *.gif *.bmp *.jpeg)"
	fileName, _ = QFileDialog.getOpenFileName(window,"QFileDialog.getOpenFileName()", "",filetypes, options=options)
	if fileName:
		img = Image.open(fileName)
		pixels = list(img.getdata())

def btnLeft(self):
	global loc
	loc -= 1
	if loc < 0:
		loc = len(BIF.images)-1
	BIF.images[loc].printvals()
	displayImage(BIF.images[loc].exportImage(),BIF.images[loc].width,BIF.images[loc].height)
def btnRight(self):
	global loc
	loc += 1
	if loc >= len(BIF.images):
		loc = 0
	BIF.images[loc].printvals()
	displayImage(BIF.images[loc].exportImage(),BIF.images[loc].width,BIF.images[loc].height)


#Setup Image Layout
imgwin = QWidget()
imglayout = QVBoxLayout()
imglayout.addWidget(label)
imgwin.setLayout(imglayout)


#===========
#File button layout for image
filebtnlayout = QHBoxLayout()


#Add an export button to the layout
btne = QPushButton("Export Frame")
btne.clicked.connect(exportBFF2)
btne.resize(40,100)
btne.move(0,0)
filebtnlayout.addWidget(btne)

#Add an import button to the layout
btni = QPushButton("Import Frame Image")
btni.clicked.connect(importImage)
btni.resize(40,100)
btni.move(0,0)
filebtnlayout.addWidget(btni)


#layout.addLayout(filebtnlayout)
#==========

#Add < and > buttons to a horizontal layout, and that hlayout to the layout of the window
lrbtnlayout = QHBoxLayout()
btnl = QPushButton("<")
btnl.clicked.connect(btnLeft)
lrbtnlayout.addWidget(btnl)
btnr = QPushButton(">")
btnr.clicked.connect(btnRight)
lrbtnlayout.addWidget(btnr)
#layout.addLayout(lrbtnlayout)
#==========

#Add UI to image window
imglayout.addLayout(filebtnlayout)
imglayout.addLayout(lrbtnlayout)

#===============================


#Actions for clicking inside of list
def clicked(qmodelindex):
	item = listwidget.currentItem()
	if item is None:
		return
	if ".BIF" in item.text():
		fdata = BIN.filedata[BIN.filenames.index(item.text())]
		loadBIFData(fdata, item.text())
	if ".SBF" in item.text():
		fdata = BIN.filedata[BIN.filenames.index(item.text())]
		loadSceneData(fdata, item.text())

listwidget.currentItemChanged.connect(clicked)
#layout.addWidget(listwidget)

#Add Load File Button to layout
btn = QPushButton("Load File")
btn.clicked.connect(loadFile)
btn.resize(40,100)
btn.move(0,0)
layout.addWidget(btn)


#layout.addWidget(QPushButton('Top'))
#layout.addWidget(QPushButton('Bottom'))
window.setLayout(layout)
window.show()
app.exec_()