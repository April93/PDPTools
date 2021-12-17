#Read Bytes into array
def readBytes(filename):
  output = []
  with open(filename, "rb") as f:
    byte = f.read(1)
    while byte != "" and len(byte) != 0:
      output.append(ord(byte))
      byte = f.read(1)
  return output

#Save bytes to file
def saveBytes(filename, bytes):
  file = open(filename,"wb")
  bytearrayfixed = bytearray(bytes)
  file.write(bytearrayfixed)
  file.close()

plf = readBytes("panepon.plf")

def ripsong(song):
  loc = song[1]+offset
  endloc = song[1]+offset+song[2]
  print (loc, endloc)
  songbytes = plf[loc:endloc]
  saveBytes(song[0]+".song", songbytes)

def ripprojfiles(song):
  loc = song[1]
  endloc = song[1]+song[2]
  print (loc, endloc)
  songbytes = plf[loc:endloc]
  saveBytes(song[0], songbytes)

s= ["panepon.proj", 0xdbef0, 11452]
ripprojfiles(s)
s= ["panepon.pool", 0xc4b70, 95104]
ripprojfiles(s)
s= ["panepon.sdir", 0xDEBB0, 0x6F64]#0x6F40]
ripprojfiles(s)

offset = 0xB8DA0
song = ["cloud_01_song", 0x2cd80,  9784]
ripsong(song)
song = ["cloud_02_song", 0x2f3c0,  5020]
ripsong(song)
song = ["dragon_01_song", 0x30760, 13728]
ripsong(song)
song = ["dragon_02_song", 0x33d00,  6748]
ripsong(song)
song = ["ending_song", 0x35760,  3224]
ripsong(song)
song = ["ending_easymode_song", 0x36400, 16152]
ripsong(song)
song = ["ending_frog_song", 0x3a320, 10448]
ripsong(song)
song = ["ending_staff_credits_song", 0x3cbf0, 21324]
ripsong(song)
song = ["ending_whale_eyes_song", 0x41f40, 15048]
ripsong(song)
song = ["ending_whale_gets_back_eyes_song", 0x45a10,  3124]
ripsong(song)
song = ["ending_which_photo_song", 0x46650, 56088]
ripsong(song)
song = ["fire_01_song", 0x54170, 16764]
ripsong(song)
song = ["fire_02_song", 0x582f0, 11872]
ripsong(song)
song = ["flower_01_song", 0x5b150,  7568]
ripsong(song)
song = ["flower_02_song", 0x5cee0,  2884]
ripsong(song)
song = ["goddess_01_song", 0x5da30,  6176]
ripsong(song)
song = ["goddess_02_song", 0x5f250,  2216]
ripsong(song)
song = ["goddess_appear_song", 0x5fb00,  3472]
ripsong(song)
song = ["green_01_song", 0x60890, 16972]
ripsong(song)
song = ["green_02_song", 0x64ae0,  1664]
ripsong(song)
song = ["ice_01_song", 0x65160, 13224]
ripsong(song)
song = ["ice_02_song", 0x68510,  2412]
ripsong(song)
song = ["intro_beginning_song", 0x68e80,  8160]
ripsong(song)
song = ["intro_storm_song", 0x6ae60, 21076]
ripsong(song)
song = ["jewel_01_song", 0x700c0, 10356]
ripsong(song)
song = ["jewel_02_song", 0x72940,  2364]
ripsong(song)
song = ["joker_01_song", 0x73280,  6548]
ripsong(song)
song = ["joker_02_song", 0x74c20,  6752]
ripsong(song)
song = ["joker_apper_song", 0x76680,  5608]
ripsong(song)
song = ["keyload_part1_song", 0x77c70,  5516]
ripsong(song)
song = ["keyroad_part2_song", 0x79200,   808]
ripsong(song)
song = ["keyroad_part3_song", 0x79530,  5780]
ripsong(song)
song = ["lion_01_song", 0x7abd0,  5756]
ripsong(song)
song = ["lion_02_song", 0x7c250,  2892]
ripsong(song)
song = ["moon_01_song", 0x7cda0,  4744]
ripsong(song)
song = ["moon_02_song", 0x7e030,  2432]
ripsong(song)
song = ["obaba_01_song", 0x7e9b0,  7284]
ripsong(song)
song = ["obaba_02_song", 0x80630,  4152]
ripsong(song)
song = ["opening_song", 0x81670,  8904]
ripsong(song)
song = ["opening_skip_song", 0x83940,  7776]
ripsong(song)
song = ["prince_01_song", 0x857a0,  7084]
ripsong(song)
song = ["prince_02_song", 0x87350,  4796]
ripsong(song)
song = ["puzzle_edit_song", 0x88610,  8576]
ripsong(song)
song = ["reserved1_song", 0x8a790, 15568]
ripsong(song)
song = ["reserved2_song", 0x8e460, 13092]
ripsong(song)
song = ["reslut_song", 0x91790,  9832]
ripsong(song)
song = ["sanatos_01_song", 0x93e00, 11308]
ripsong(song)
song = ["sanatos_02_song", 0x96a30,  2684]
ripsong(song)
song = ["sea_01_song", 0x974b0,  7260]
ripsong(song)
song = ["sea_02_song", 0x99110,  4856]
ripsong(song)
song = ["select_song", 0x9a410, 11284]
ripsong(song)
song = ["tutorial_song", 0x9d030, 16460]
ripsong(song)
song = ["water_01_song", 0xa1080,  7780]
ripsong(song)
song = ["water_02_song", 0xa2ef0,  7352]
ripsong(song)
song = ["whale_01_song", 0xa4bb0, 12180]
ripsong(song)
song = ["whale_02_song", 0xa7b50,  6332]
ripsong(song)
song = ["which_appear_song", 0xa9410,  1356]
ripsong(song)
song = ["you_lose_song", 0xa9960,  1484]
ripsong(song)
song = ["you_win_song", 0xa9f30,  5272]
ripsong(song)