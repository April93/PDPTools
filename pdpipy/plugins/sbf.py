from hacklib import readValue

class sbf:
	"""SBF file class"""
	filetype = "scene"
	filename = ""
	importedfiles = []

	katakana = ['', 'ア', 'イ', 'ウ', 'エ', 'オ', 'カ', 'キ', 'ク', 'ケ', 'コ', 'サ', 'シ', 'ス', 'セ', 'ソ', 'タ', 'チ', 'ツ', 'テ', 'ト', 'ナ', 'ニ', 'ヌ', 'ネ', 'ノ', 
			'ハ', 'ヒ', 'フ', 'ヘ', 'ホ', 'マ', 'ミ', 'ム', 'メ', 'モ', 'ヤ', 'ユ', 'ヨ', 'ラ', 'リ', 'ル', 'レ', 'ロ', 'ワ', 'ヲ', 'ン', 'ガ', 'ギ', 'グ', 'ゲ', 'ゴ', 
			'ザ', 'ジ', 'ズ', 'ゼ', 'ゾ', 'ダ', 'ヂ', 'ズ', 'デ', 'ド', 'バ', 'ビ', 'ブ', 'ベ', 'ボ', '?', 'パ', 'ピ', 'プ', 'ペ', 'ポ', 'ァ', 'ィ', 'ゥ', 'ェ', 'ォ', 'ャ', 'ュ', 'ョ', 'ッ']

	hiragana = ['', 'あ', 'い', 'う', 'え', 'お', 'か', 'き', 'く', 'け', 'こ', 'さ', 'し', 'す', 'せ', 'そ', 'た', 'ち', 'つ', 'て', 'と', 'な', 'に', 'ぬ', 'ね', 'の',
				'は', 'ひ', 'ふ', 'へ', 'ほ', 'ま', 'み', 'む', 'め', 'も', 'や', 'ゆ', 'よ', 'ら', 'り', 'る', 'れ', 'ろ', 'わ', 'を', 'ん', 'が', 'ぎ', 'ぐ', 'げ', 'ご', 
				'ざ', 'じ', 'ず', 'ぜ', 'ぞ', 'だ', 'ぢ', 'づ', 'で', 'ど', 'ば', 'び', 'ぶ', 'べ', 'ぼ', 'ぱ', 'ぴ', 'ぷ', 'ぺ', 'ぽ', 'ぁ', 'ぃ', 'ぅ', 'ぇ', 'ぉ', 'ゃ', 'ゅ', 'ょ', 'っ']

	symbols = ['', '-', '<1002>', '・', '!', '?', '.', ',', '「', '」', '(', ')', '<100C>', '<100D>', '<100E>', '~', '&']

	kanji = [ '', '人', '遊', '説', '明', '上', '達', '道', '名', '前', '登', '録', '操', '作', '基', '本', '同', '時', '連', '鎖', '消',
			 '記', '自', '分', '設', '定', '画', '面', '級', '仕', '方', '見', '新', '下', '左', '右', '数', '最', '大', '妖', '精',
			 '選', '択', '手', '終', '了', '使', '問', '題', '愛', '悪', '一', '運', '雲', '炎', '旺', '王', '温', '音', '何', '歌', 
			 '火', '花', '会', '海', '界', '覚', '感', '奇', '気', '起', '義', '強', '狂', '驚', '空', '月', '元', '言', '個', '光', 
			 '好', '幸', '広', '荒', '高', '歳', '在', '三', '姉', '子', '事', '持', '者', '主', '守', '宿', '出', '緒', '女', '傷', 
			 '少', '情', '色', '心', '真', '神', '水', '世', '性', '正', '盛', '声', '右', '責', '双', '存', '他', '太', '体', '短', 
			 '知', '中', '仲', '帝', '敵', '的', '天', '怒', '倒', '頭', '動', '任', '番', '秘', '氷', '品', '不', '負', '物', '聞', 
			 '慕', '母', '宝', '豊', '魔', '妹', '味', '魅', '命', '鳴', '目', '役', '優', '勇', '由', '様', '羊', '葉', '陽', '欲', 
			 '竜', '良', '力', '緑', '霊', '話', '安', '栄', '加', '嘉', '雅', '介', '外', '学', '丸', '久', '宮', '居', '橋', '啓', 
			 '賢', '玄', '宏', '康', '弘', '晃', '浩', '豪', '佐', '崎', '山', '史', '志', '治', '実', '仁', '生', '西', '仙', '川', 
			 '浅', '村', '代', '滝', '沢', '智', '池', '鳥', '田', '嶋', '湯', '藤', '奈', '内', '曰', '八', '妃', '敏', '夫', '武', 
			 '福', '文', '茂', '木', '野', '弥', '矢', '悠', '裕', '洋', '利', '隆', '鈴', '泳', '回', '間', '希', '帰', '玉', '思', 
			 '借', '助', '勝', '城', '聖', '切', '全', '伝', '当', '平', '返', '法', '望', '夢', '無', '和', '伊', '井', '葛', '季', 
			 '紀', '吉', '勲', '軍', '健', '呼', '孝', '鋼', '剛', '根', '示', '殊', '樹', '将', '松', '晋', '森', '瀬', '清', '善', 
			 '泰', '谷', '津', '島', '特', '二', '之', '美', '百', '表', '富', '扶', '輔', '北', '也', '亮', '呂', '郎', '修', '正', 
			 '戻', '峻', '潘', '薙', '九', '後', '角', '四', '路', '亜', '岸', '有', '寺', '永', '圭', '渚', '今', '足', '立', '綾', 
			 '舞', '鳴', '佳', '英', '細', '俊', '宗', '章', '小', '寺', '明', '黒', '育', '辻', '臣', '江', '口', '勉', '差', '準', 
			 '備', '練', '習', '建', '林', '絵', '里', '伯', '直', '敬', '春', '竹', '男', '理', '風']

	def decodeString(self, s):
		dstring = ""
		#print ("String: ",s)
		c = 0
		while c < len(s):
			#print (c, len(s))
			#print (s[c])

			#Lowercase roman letters
			if s[c] == 28: #0x1C
				c += 1
				dstring += chr(s[c]+ord('a')-1)

			#Uppercase roman letters
			elif s[c] == 24: #0x18
				c += 1
				dstring += chr(s[c]+ord('A')-1)

			#Numbers
			elif s[c] == 20 and s[c+1] <= 9: #0x14
				c += 1
				dstring += chr(s[c]+ord('0'))

			#spaces/newline
			elif s[c] == 0 and s[c+1] <= 2:
				c += 1
				if s[c] == 1:
					dstring += " "
				else:
					dstring += "<newline>"

			#Symbols File
			elif s[c] == 16: #0x10
				c += 1
				if s[c] < len(self.symbols):
					dstring += self.symbols[s[c]]
				else:
					dstring += "<"+("%02X"%s[c-1])+("%02X"%s[c])+">"


			#Hiragana
			elif s[c] == 12: #0x0C
				c += 1
				if s[c] < len(self.hiragana):
					dstring += self.hiragana[s[c]]
				else:
					dstring += "<"+("%02X"%s[c-1])+("%02X"%s[c])+">"
			#Katakana
			elif s[c] == 8: #0x08
				c += 1
				if s[c] < len(self.katakana):
					dstring += self.katakana[s[c]]
				else:
					dstring += "<"+("%02X"%s[c-1])+("%02X"%s[c])+">"
			#Kanji
			elif s[c] == 4 or s[c] == 5: #0x04 0x05
				kindex = s[c+1]+(255*(s[c]-4))
				c += 1
				if kindex < len(self.kanji):
					dstring += self.kanji[kindex]
				else:
					dstring += "<"+("%02X"%s[c-1])+("%02X"%s[c])+">"

			#Other
			else:
				dstring += "<"+("%02X"%s[c])+("%02X"%s[c+1])+">"
				c += 1

			c += 1
		return dstring

	#Initializes and reads binary data
	def __init__(self, data, **kwargs):
		filename = kwargs.get("filename", "")
		self.filename = filename
		readindex = 0
		printfull = True

		#Check magic bytes
		if chr(data[0]) != "S" or chr(data[1]) != "B" or chr(data[2]) != "F" or chr(data[3]) != "1":
			print ("Not an SBF!")
			return
		readindex += 4

		#Check number of Imported/Included Files
		importfilenum = readValue(data, readindex, 1)
		readindex += 4
		if printfull:
			print (importfilenum,"imported files")

		#Read Included/Imported File Names
		for i in range(0, importfilenum):
			name = data[readindex:(readindex+16)]
			name = ''.join(chr(ch) for ch in name)
			readindex += 16
			self.importedfiles.append(name)

		#Get number of scenes
		scenenum = readValue(data, readindex, 1)
		readindex += 4
		if printfull:
			print (scenenum)

		#Read each scene
		for i in range(0, scenenum):
			#Unknown Header/Type info
			scenetype = data[readindex:readindex+4]
			readindex += 4

			#Read Scene name length
			scenenamelength = readValue(data, readindex, 1)
			readindex += 4

			#Read Scene name
			scenename = data[readindex:(readindex+scenenamelength)]
			scenename = ''.join(chr(ch) for ch in scenename)
			readindex += scenenamelength
			if printfull:
				print (scenename)

			#Check Menu
			scenemenu = readValue(data, readindex, 1)
			readindex += 4
			if printfull:
				print ("scenemenu",scenemenu)

			#Loop through menu entries
			for m in range(0, scenemenu):
				if printfull:
					print("Menu Entry:",m)
				menuheader = readValue(data, readindex, 1)

				# Simplified Menu Extraction/Import
				# header[3]
				# >F0 = 13 extra (1 exception)
				# >E0 <F0 = 12 extra (1exception)
				# <70 = 11extra
				menusize = 13*4
				if data[readindex+3] < 0xF0:
					menusize = 12*4
				if data[readindex+3] < 0x70:
					menusize = 11*4

				#Exceptions
				if menuheader == 0x260d1:
					menusize = 13*4
				if menuheader == 0x002107F1:
					menusize = 17*4
				if menuheader == 0x000900f1:
					menusize = 17*4
				readindex += 4
				menudata = data[readindex:readindex+menusize]
				readindex += menusize
				if printfull:
					print("menu:", menusize, menudata)

			#Get number of Text Items
			textitemnum = readValue(data, readindex, 1)
			readindex += 4
			if printfull:
				print ("Text Items:", textitemnum)

			for t in range(0, textitemnum):
				itemtype = data[readindex:readindex+4]
				itemtypeint = readValue(data, readindex, 1)
				if printfull:
					print ("Item type:", itemtypeint)
				readindex += 4
				xloc = readValue(data,readindex,1)
				readindex += 4
				yloc = readValue(data,readindex,1)
				readindex += 4
				if printfull:
					print ("xloc/yloc:", xloc, yloc)

				#Menu FF's. No idea if they're ever different
				if itemtype[2] != 0x14:
					menuF = data[readindex:readindex+8]
					readindex += 8

				# Some sort of marker/header. Seems to determine triggered events/linked pictures
				# Everything with the third byte above 5 has values here. Might not all be used the same
				if itemtype[2] > 5:
					marker = data[readindex:readindex+8]
					readindex += 8

				#0000 c847 - Has a mystery value (Perhaps a second marker?)
				if itemtypeint == 0xc847:
					c847val = data[readindex:readindex+8]
					readindex += 8

				#Text Colors
				colornum = readValue(data, readindex, 1)
				if printfull:
					print ("Colornum: ", colornum)
				readindex += 4
				for c in range(0, colornum):
					textfillcolor = data[readindex:readindex+4]
					readindex += 4
					textbordercolor = data[readindex:readindex+4]
					readindex += 4
					if printfull:
						print (c, textfillcolor, textbordercolor)
					if c > 100:
						break

				#Number of Lines
				linenum = readValue(data, readindex, 1)
				readindex += 4

				if printfull:
					print ("Lines:",linenum)

				#Mystery Data
				m1 = readValue(data, readindex, 1)
				readindex += 4
				m2 = readValue(data, readindex, 1)
				readindex += 4
				for m in range(0, m2):
					md2ext = readValue(data, readindex, 1)
					readindex += 4
				if printfull:
					print ("md", m1, m2)

				textlength = readValue(data, readindex, 1)
				textlength2 = textlength*2
				readindex += 4
				if printfull:
					print ("text length: ", textlength, textlength2)
				text = data[readindex:(readindex+textlength2)]
				#text = ''.join(chr(ch) for ch in text)
				text = self.decodeString(text) #Need to write the decode string function
				readindex += textlength2
				print (text)
			
			imageitemnum = readValue(data, readindex, 1)
			readindex += 4
			if printfull:
				print ("Image Items:", imageitemnum)

			for im in range(0, imageitemnum):
				val1 = data[readindex]
				readindex += 1

				fthing = 0
				if val1 >= 0xF0:
					fthing = data[readindex:readindex+8]
					readindex += 8
					val1 = data[read]
					readindex += 1
					if printfull:
						print ("fthing:", fthing)

				if printfull:
					print ("val1:", val1)

				val2 = data[readindex]
				readindex += 1
				if printfull:
					print ("val2:", val2)

				tag = data[readindex]
				readindex += 1
				if printfull:
					print ("tag:", tag)

				#image z layer/location
				zlayer = data[readindex]
				readindex += 1

				#image x and y location
				xloc = readValue(data, readindex, 1)
				readindex += 4
				yloc = readValue(data, readindex, 1)
				readindex += 4

				if printfull:
					print ("Z-Layer:", zlayer)
					print ("X-Loc:", xloc)
					print ("Y-Loc:", yloc)

				frame = readValue(data, readindex, 1)
				readindex += 4
				if printfull:
					print ("frame: ", frame)

				imageid = data[readindex:readindex+8]
				readindex += 8
				if printfull:
					print ("Image ID: ", imageid)

				imagefile = readValue(data, readindex, 1)
				readindex += 4
				imagename = self.importedfiles[imagefile]
				if printfull:
					print ("Image File:", imagename)

				tag4567 = 0
				tag12 = 0
				tag12ext = 0
				if tag >= 4 and tag <= 7:
					tag4567 = data[readindex:readindex+4]
					readindex += 4
					if printfull:
						print ("tag4567:", tag4567)
				if tag == 12:
					tag12 = data[readindex:readindex+4]
					readindex += 4
					if printfull:
						print ("tag12:", tag12)
					if tag12[3] == 0xFE:
						tag12ext = data[readindex:readindex+8]
						readindex += 8
						if printfull:
							print ("tag12ext: ", tag12ext)

			if data[readindex] == 255:
				endf = data[readindex:readindex+12]
				readindex += 12
				if printfull:
					print ("endFThing:", endf)

			extradata = readValue(data, readindex, 1)
			readindex += 4
			if printfull:
				print ("extradata:", extradata)
			if extradata >= 1:
				code = 0
				endval = 4294901888
				while code != endval and extradata >= 1:
					miscextradata = data[readindex:readindex+4]
					code = readValue(data, readindex, 1)
					readindex += 4
					if printfull:
						print ("Misc Extra Data:", miscextradata)
					if code == 0x00ff0080 or code == 0xFF000080:
						code = endval
						extradata = -1



			#break

