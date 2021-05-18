//Panel De Pon (GC) Decryption Tool
//Code Written By Kafke
//Extracts/Creates __NAME.BIN files from Nintendo Puzzle Collection
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ios>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <strings.h>

#if defined(_WIN32)
#include <dirent.h>
#include <windows.h>
#elif defined __GNUC__
#include <sys/types.h>
#include <sys/stat.h>
#endif
using namespace std;

typedef unsigned char u8;
typedef unsigned char u16;
typedef unsigned int u32;

typedef struct
{
  int addr;
  int size;
  u8* buff;
} fileimport;

//Convert grabbed bytes to an integer
int bytesToInt(u8* bytes, int length=4)
{
  int Int32 = 0;

  for(int l = 0; l < length; l++)
    Int32 = (Int32 << 8) + bytes[l];

  return Int32;
}

//Flip an int, so we can write it correctly to the SBF file
int intToBytes(int dword)
 {
   return ((dword>>24)&0x000000FF) | ((dword>>8)&0x0000FF00) | ((dword<<8)&0x00FF0000) | ((dword<<24)&0xFF000000);
}

//FF -> 255, USEFUL
int hexStringToInt(string hex)
{
  return strtoul(hex.c_str(), NULL, 16);
}

std::string remove_char( std::string str, char ch )
{
    // remove all occurrences of char ch from str
    str.erase( std::remove( str.begin(), str.end(), ch ), str.end() ) ;
    return str ;
}

void outputBytes(u8* bytes, int size)
{
  cout << "0x";
  for(int i = 0; i < size; i++)
  {
    printf("%02x", bytes[i]);
    if(i%2 == 1){ cout << " ";}
  }
}

void outputBytesToFile(ofstream* fout, u8* bytes, int size)
{
  for(int i = 0; i < size; i++)
  {
    *fout << hex << setw(2) << setfill('0') << int(bytes[i]);
    if(i%2 == 1){ *fout << " ";}
  }
}

string encodeString(string text)
{
  stringstream encodestream;
  for(int i = 0; i < text.length(); i++)
  {
    bool write = false;
    if(text[i] == '\'' || text[i] == '"' || text[i] == ':')
    {
      if(text[i] == '"')
      {
        encodestream << "1008";
        write = true;
      }
      if(text[i] == ':')
      {
        cout << "Colon error!" << endl;
        write = true;
      }
      if(text[i] == '\'')
      {
        encodestream << "1009";
        write = true;
      }
    }
    else if(text[i] == ' ')
    {
      encodestream << "0001";
      write = true;
    }
    else if(text[i] >= '0' && text[i] <= '9')
    {
      encodestream << "14";
      encodestream << "0" << text[i];
      write = true;
    }
    else if(text[i] >= 'A' && text[i] <= 'Z')
    {
      encodestream << "18";
      encodestream << hex << setfill('0') << std::uppercase << std::setw(2) << ((int)text[i]-'A'+1);
      write = true;
    }
    else if(text[i] >= 'a' && text[i] <= 'z')
    {
      encodestream << "1C";
      encodestream << hex << setfill('0') << std::uppercase << std::setw(2) << ((int)text[i]-'a'+1);
      write = true;
    }
    else if(text[i] >= 0xfeff0000)//0xffffff00)
    {
      char* kana = (char*)text.substr(i,3).c_str();
      i+=2;

      if(strcmp(kana, "「") == 0) {encodestream << "1008"; write = true;}
      else if(strcmp(kana, "」") == 0) { encodestream << "1009"; write = true;}
      else if(strcmp(kana, "一") >= 0)
      {
        ifstream tablefile("kanji.txt");
        if (tablefile.is_open())
        {
          //Get the list of files
          int j = 0;
          string line;
          while ( getline (tablefile,line) )
          {
            char* val = const_cast<char *>(line.c_str());
            if(strcmp(val, kana) == 0)
            {
              int kmark = 4;
              if(j>=255)
              {
                j-=256;
                kmark++;
              }
              encodestream << "0" << kmark;
              encodestream << hex << setfill('0') << std::uppercase << std::setw(2) << j+1;
              write = true;
              break;
            }
            j++;
          }
          tablefile.close();
        }
      }

      else if(strcmp(kana, "ァ") >= 0 && strcmp(kana, "ヶ") <= 0)
      {
        encodestream << "08";
        ifstream tablefile("katakana.txt");
        if (tablefile.is_open())
        {
          //Get the list of files
          int j = 0;
          string line;
          while ( getline (tablefile,line) )
          {
            char* val = const_cast<char *>(line.c_str());
            if(strcmp(val, kana) == 0)
            {
              encodestream << hex << setfill('0') << std::uppercase << std::setw(2) << j+1;
              write = true;
              break;
            }
            j++;
          }
          tablefile.close();
        }
      }

      else if(strcmp(kana, "ぁ") >= 0 && strcmp(kana, "ん") <= 0)
      {
        encodestream << "0C";
        ifstream tablefile("hiragana.txt");
        if (tablefile.is_open())
        {
          //Get the list of files
          int j = 0;
          string line;
          while ( getline (tablefile,line) )
          {
            char* val = const_cast<char *>(line.c_str());
            if(strcmp(val, kana) == 0)
            {
              encodestream << hex << setfill('0') << std::uppercase << std::setw(2) << j+1;
              write = true;
              break;
            }
            j++;
          }
          tablefile.close();
        }
      }
      else
      {
        encodestream << "10";
        ifstream tablefile("symbols10.txt");
        if (tablefile.is_open())
        {
          //Get the list of files
          int j = 0;
          string line;
          while ( getline (tablefile,line) )
          {
            char* val = const_cast<char *>(line.c_str());
            if(strcmp(val, kana) == 0)
            {
              encodestream << hex << setfill('0') << std::uppercase << std::setw(2) << j+1;
              write = true;
              break;
            }
            j++;
          }
          tablefile.close();
        }
      }
    }
    else if(text[i] == '<')
    {
      //Shouldn't happen, but maybe there's some missing kanji
      if(text[i+1] == 'k')
      {
        i++;
        int kval = atoi(text.substr(i+1,3).c_str());

        if(kval > 255)
          encodestream << "05" << (kval-255);
        else
          encodestream << "04" << kval;

        write = true;
        i+=4;
        //cout << "KANJI ERROR!" << endl;
        //return "KANJI ERROR";
      }
      //<newline>
      else if(text[i+1] == 'n')
      {
        encodestream << "0002";
        write = true;
        i+=8;
      }
      //Un-listed tags
      else
      {
        i++;
        encodestream << text.substr(i,4);
        write = true;
        i+=4;
      }
    }
    else
    {
      encodestream << "10";
      ifstream tablefile("symbols10.txt");
      if (tablefile.is_open())
      {
        //Get the list of files
        int j = 0;
        string line;
        while ( getline (tablefile,line) )
        {
          char* val = const_cast<char *>(line.c_str());
          if(val[0] == text[i])
          {
            encodestream << hex << setfill('0') << std::uppercase << std::setw(2) << j+1;
            write = true;
            break;
          }
          j++;
        }
        tablefile.close();
      }
    }
    if(write == false)
      cout << text[i] << "DIDN'T WRITE" << endl;
  }
  return encodestream.str();
}

string decodeString(u8* bytes, int size)
{
  std::stringstream decodestream;

  for(int i = 0; i < size; i++)
  {
    if(bytes[i] == 28) //0x1C
    {
      i++;
      decodestream << (char)(bytes[i]+'a'-1);
    }
    else if(bytes[i] == 24) //0x18
    {
      i++;
      decodestream << (char)(bytes[i]+'A'-1);
    }
    else if(bytes[i] == 20 && bytes[i+1] <= 9) //0x14
    {
      i++;
      decodestream << (char)(bytes[i]+'0');
    }
    else if(bytes[i] == 0 && bytes[i+1] <= 2)
    {
      i++;
      if(bytes[i] == 1)
        decodestream << " ";
      else
        decodestream << "<newline>";
    }
    else if(bytes[i] == 16) //0x10
    {
      i++;
      //decodestream << "あ";

      ifstream tablefile("symbols10.txt");
      bool isfound = false;
      if (tablefile.is_open())
      {
        //Get the list of files
        int j = 0;
        string line;
        while ( getline (tablefile,line) )
        {
          string val = const_cast<char *>(line.c_str());
          if(bytes[i]-1 == j)
          {
            decodestream << val;
            isfound=true;
            break;
          }
          j++;
        }
        tablefile.close();
      }
      if(!isfound)
      {
        decodestream << "<"
        << std::hex << std::setfill('0') << std::uppercase
        << std::setw(2) << static_cast<int>(bytes[i-1])
        << std::setw(2) << static_cast<int>(bytes[i])
        << ">";
      }
    }
    else if(bytes[i] == 12) //0C
    {
      i++;
      //decodestream << "あ";

      ifstream tablefile("hiragana.txt");
      if (tablefile.is_open())
      {
        //Get the list of files
        int j = 0;
        string line;
        while ( getline (tablefile,line) )
        {
          string val = const_cast<char *>(line.c_str());
          if(bytes[i]-1 == j)
          {
            decodestream << val;
            break;
          }
          j++;
        }
        tablefile.close();
      }

    }
    else if(bytes[i] == 8) //08
    {
      i++;
      //decodestream << "ア";
      ifstream tablefile("katakana.txt");
      if (tablefile.is_open())
      {
        //Get the list of files
        int j = 0;
        string line;
        while ( getline (tablefile,line) )
        {
          string val = const_cast<char *>(line.c_str());
          if(bytes[i]-1 == j)
          {
            decodestream << val;
            break;
          }
          j++;
        }
        tablefile.close();
      }
    }
    else if(bytes[i] == 4 || bytes[i] == 5)
    {
      i++;
      //decodestream << "人";

      ifstream tablefile("kanji.txt");
      bool isfound = false;
      if (tablefile.is_open())
      {
        //Get the list of files
        int j = 0;
        string line;
        while ( getline (tablefile,line) )
        {
          string val = const_cast<char *>(line.c_str());
          if( ((bytes[i]-1 == j && bytes[i-1] == 4) || (255+bytes[i] == j && bytes[i-1] == 5)) && val != "")
          {
            decodestream << val;
            isfound=true;
            break;
          }
          j++;
        }
        tablefile.close();
      }
      if(!isfound)
      {
        decodestream << "<k"
        << std::hex << std::setfill('0') << std::uppercase
        << std::setw(2) << static_cast<int>(bytes[i]+(bytes[i-1]==5?255:0))
        << ">";
      }
    }
    else
    {
      decodestream << "<"
      << std::hex << std::setfill('0') << std::uppercase
      << std::setw(2) << static_cast<int>(bytes[i])
      << std::setw(2) << static_cast<int>(bytes[i+1])
      << ">";
      i++;
    }
    //decodestream << "みんなのにほんご";
    //decodestream << folder << "/index.txt";
  }


  std::string decodedstring = decodestream.str();
  return decodedstring;
}


char* substringCharArray(char* str, int start, int end)
{
  char* newstr = new char[end-start];
  for(int i = start; i < end; i++)
  {
    newstr[i-start] = str[i];
  }
  newstr[end] = 0;
  return newstr;
}

void createBIN(char* binfilename)
{
  //Get the directory
  char* folder = substringCharArray(binfilename, 0, strlen(binfilename)-4);

  //Read Index File
  std::stringstream indexloc;
  indexloc << folder << "/index.txt";
  std::string indexname = indexloc.str();


  //Initialize fileindex values
  string* fileindex;
  int filenum;
  string line;

  //Open file index
  ifstream indexfile(indexname);
  if (indexfile.is_open())
  {
    //Get the number of files and initialize variables
    getline(indexfile, line);
    filenum = atoi(line.c_str());
    fileindex = new string[filenum];
    int j = 0;

    //Get the list of files
    while ( getline (indexfile,line) )
    {
      fileindex[j] = const_cast<char *>(line.c_str());
      j++;
    }
    indexfile.close();
  }else cout << "Unable to open file";


  //For each file, read in data and store in fileimport
  //cout << "Files: " << filenum << endl;
  fileimport files[filenum];
  for(int i = 0; i < filenum; i++)
  {
    std::stringstream fileloc;
    fileloc << folder << "/" << fileindex[i];
    std::string filename = fileloc.str();

    //Open file
    FILE* inFile = fopen(filename.c_str(), "rb");
    fseek(inFile, 0, SEEK_END);
    int size = ftell(inFile);
    fseek(inFile, 0, SEEK_SET);

    //Variable to hold data
    fileimport tempfile;
    tempfile.size = size;
    tempfile.buff = new u8[size];

    //Load the data
    fread(tempfile.buff, 1, tempfile.size, inFile);

    //Done with the file, so close
    fclose(inFile);

    files[i] = tempfile;
  }

  //Initialize index stuff
  int indexSize = 4+(24*filenum);
  u8* indexbuff = new u8[indexSize];
  int currentLoc = indexSize;

  //Calculate Addresses of files
  for(int i = 0; i < filenum; i++)
  {
    files[i].addr = currentLoc;

    int indexlocstart = 4+(24*i);
    //Write size for index
    for (int l = 0; l < 4; l++)
      indexbuff[indexlocstart+(3 - l)] = (files[i].size >> (l * 8));
    //Write address for index
    for (int l = 0; l < 4; l++)
      indexbuff[indexlocstart+4+(3 - l)] = (files[i].addr >> (l * 8));
    for (int l = 0; l < 16; l++)
    {
      if(l < fileindex[i].length())
        indexbuff[indexlocstart+8+l] = fileindex[i][l];
      else
        indexbuff[indexlocstart+8+l] = 0;
    }

    currentLoc += files[i].size;
    printf("Size: %X, Location: %X\n", files[i].size, files[i].addr);
  }

  //Create index

  //Write number of files
  for (int i = 0; i < 4; i++)
    indexbuff[3 - i] = (filenum >> (i * 8));


  int finalSize = currentLoc;
  //cout << finalSize << endl;

  u8* filebuff = new u8[finalSize];

  //Write index to buff
  for(int j = 0; j < indexSize; j++)
  {
    filebuff[j] = indexbuff[j];
  }

  //Write files to buff
  for(int i = 0; i < filenum; i++)
  {
    for(int j = 0; j < files[i].size; j++)
    {
      filebuff[j+files[i].addr] = files[i].buff[j];
    }
  }

  //Write final binary
  FILE* output = fopen( binfilename, "wb" );
  fwrite( filebuff, 1, finalSize, output );
  fclose(output);

}

void extractBIN(char* binfilename)
{
  char* param = binfilename;
  FILE* inFile = fopen(param, "rb");
  if(inFile == NULL)
  {
    printf("inFile == NULL\n");
    return;
  }
  printf("File OK\n");


  //Get file size
  fseek(inFile, 0, SEEK_END);
  int size = ftell(inFile);
  fseek(inFile, 0, SEEK_SET);
  printf("input file size: 0x%X\n", size);

  //Variable to hold data
  u8* buff = new u8[size];

  //Load the data
  fread(buff, 1, size, inFile);

  //Done with the file, so close
  fclose(inFile);

  //Set our index
  int index = 0;

  //Store the header for later
  u8* header = new u8[4];
  for(int i = 0; i < 4; i++) { header[index] = buff[index]; index++; }
  int filenum = bytesToInt(header);

  //Create a file index (we'll be saving this later)
  char* fileindex[filenum];

  //Output the number of files
  //cout << "Files: " << filenum << endl;


  //Make the directory if it doesn't exist
  char* folder = substringCharArray(param, 0, strlen(param)-4);
  #if defined(_WIN32)
  mkdir(folder);
  #else
  mkdir(folder, 0777);
  #endif


  //Loop through the file list
  for(int i = 0; i < filenum; i++)
  {
    //cout << i << ". ";

    //File Size
    //cout << "Size: ";
    u8* size = new u8[4];
    for(int j = 0; j < 4; j++)
    {
      size[j] = buff[index];
      index++;
    }
    //cout << bytesToInt(size);

    //Address
    //cout << ", Address: ";
    u8* address = new u8[4];
    for(int j = 0; j < 4; j++)
    {
      address[j] = buff[index];
      index++;
    }
    //cout << bytesToInt(address);

    //Name
    //cout << ", Name: ";
    u8* name = new u8[16];
    for(int j = 0; j < 16; j++)
    {
      name[j] = buff[index];
      index++;
    }
    //cout << name;
    fileindex[i] = (char*)name;


    //Create var storage for the file data and grab relevant data
    u8* fileBuf = new u8[bytesToInt(size)];
    for(int l = 0; l < bytesToInt(size); l++)
    {
      fileBuf[l] = buff[l+bytesToInt(address)];
    }

    //Figure out folder/file names
    std::stringstream fileloc;
    fileloc << folder << "/" << (char*)name;
    std::string filename = fileloc.str();


    //Write File
    FILE* output = fopen( filename.c_str(), "wb" );
    fwrite( fileBuf, 1, bytesToInt(size), output );
    fclose(output);

    //printf("\n");
  }


  //Create index file
  std::stringstream indexloc;
  indexloc << folder << "/index.txt";
  std::string indexname = indexloc.str();

  ofstream indexfile;
  indexfile.open(indexname);
  indexfile << filenum << "\n";
  for(int i = 0; i < filenum; i++)
  {
    indexfile << fileindex[i] << "\n";
  }
  indexfile.close();

  delete [] buff;
}

void createSBF(char* sbffilename, bool externaltxt)
{
  char* rootname = substringCharArray(sbffilename, 0, strlen(sbffilename)-4);

  std::stringstream txtloc;
  txtloc << rootname << ".txt";
  std::string txtname = txtloc.str();

  std::stringstream etxtloc;
  etxtloc << rootname << "tr.txt";
  std::string etxtname = etxtloc.str();

  std::stringstream sbfloc;
  sbfloc << rootname << ".SBF";
  std::string sbfname = sbfloc.str();


  //Input txt file
  string line;
  ifstream datafile(txtname);

  if(!datafile.is_open())
  {
    cout << "Problem opening TXT file." << endl;
    cout << strerror(errno) << endl;
    cout << txtname << endl;
    return;
  }

  string etext;
  ifstream etextfile;
  if(externaltxt)
  {
    etextfile.open(etxtname);
    if(!etextfile.is_open())
    {
      cout << "Problem opening External TXT file." << endl;
      cout << strerror(errno) << endl;
      cout << etxtname << endl;
      return;
    }
  }

  //SBF output file
  ofstream sbffile;
  sbffile.open (sbfname, ios::out | ios::binary);

  sbffile.write((char*)"SBF1",4);

  if (sbffile.is_open())
  {

    //Get new line
    getline (datafile,line);
    int colon = line.find(':');
    line = line.substr(colon+1);

    //Write an Int string from TXT to SBF

    //Get the number of used/imported files and write to SBF
    int files = stoi(line);
    int fileshex = intToBytes(files);
    sbffile.write((char*)&fileshex, sizeof(fileshex));

    string* filenames = new string[files];

    //Write each filename to the SBF
    for(int i = 0; i < files; i++)
    {
      getline (datafile,line);
      colon = line.find(':');
      line = line.substr(colon+1);
      char* filename = new char[17];
      for(int f = 0; f < 17; f++)
        filename[f] = 0x00;
      strcpy(filename, (line.substr(0,16)).c_str());
      filenames[i] = filename;
      sbffile.write(filename, 16);
    }


    //Get the number of pages and write to SBF
    getline (datafile,line);
    colon = line.find(':');
    line = line.substr(colon+1);
    int pages = stoi(line);
    int pageshex = intToBytes(pages);
    sbffile.write((char*)&pageshex, sizeof(pageshex));

    //Loop through pages and write them to SBF
    for(int p = 0; p < pages; p++)
    {
      //Get Page Header and write to SBF
      getline (datafile,line);
      colon = line.find(':');
      line = line.substr(colon+1);
      line = remove_char(line, ' ');
      int pageheader = hexStringToInt(line);
      pageheader = intToBytes(pageheader);
      sbffile.write((char*)&pageheader, sizeof(pageheader));

      //Get Page Name and write to SBF
      getline (datafile,line);
      colon = line.find(':');
      line = line.substr(colon+2);
      int namelengthhex = intToBytes(line.length());
      sbffile.write((char*)&namelengthhex, sizeof(namelengthhex));
      sbffile.write(line.c_str(), line.length());

      //Get the menu num and write to SBF
      getline (datafile,line);
      colon = line.find(':');
      line = line.substr(colon+1);
      int menunum = stoi(line);
      int menunumhex = intToBytes(menunum);
      sbffile.write((char*)&menunumhex, sizeof(menunumhex));

      //Get Menus and write to SBF
      for(int m = 0; m < menunum; m++)
      {
        //Get Menu Header and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int menuheader = hexStringToInt(line);
        menuheader = intToBytes(menuheader);
        sbffile.write((char*)&menuheader, sizeof(menuheader));

        //Get Menu Data and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int menudatasize = line.length()/8;
        string menudata = "";
        for(int md = 0; md < menudatasize; md++)
        {
          menudata = line.substr(md*8, 8);
          int mdval = hexStringToInt(menudata);
          mdval = intToBytes(mdval);
          sbffile.write((char*)&mdval, sizeof(mdval));
        }
      }

      //Get Number of Text Items and write to SBF
      getline (datafile,line);
      colon = line.find(':');
      line = line.substr(colon+1);
      line = remove_char(line, ' ');
      int textitems = hexStringToInt(line);
      int textitemshex = intToBytes(textitems);
      sbffile.write((char*)&textitemshex, sizeof(textitemshex));
      //Get Text Items and write to SBF
      for(int ti = 0; ti < textitems; ti++)
      {
        //Get Item Type and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int itemtypenum = hexStringToInt(line);
        itemtypenum = intToBytes(itemtypenum);
        sbffile.write((char*)&itemtypenum, sizeof(itemtypenum));

        unsigned char* itemtype = (unsigned char*)&itemtypenum;

        //Get X/Y and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int xy = hexStringToInt(line);
        xy = intToBytes(xy);
        sbffile.write((char*)&xy, sizeof(xy));
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        xy = hexStringToInt(line);
        xy = intToBytes(xy);
        sbffile.write((char*)&xy, sizeof(xy));

        //Check if item has menuF and write to SBF if it does
        if(itemtype[2] != 0x14)
        {
          //Get MenuF and write to SBF
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int menufsize = line.length()/8;
          string menuf= "";
          for(int md = 0; md < menufsize; md++)
          {
            menuf = line.substr(md*8, 8);
            int mdval = hexStringToInt(menuf);
            mdval = intToBytes(mdval);
            sbffile.write((char*)&mdval, sizeof(mdval));
          }
        }

        //Check if item has Marker and write to SBF if it does
        if(itemtype[2] > 5)
        {
          //Get Marker and write to SBF
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int markersize = line.length()/8;
          string marker= "";
          for(int md = 0; md < markersize; md++)
          {
            marker = line.substr(md*8, 8);
            int mdval = hexStringToInt(marker);
            mdval = intToBytes(mdval);
            sbffile.write((char*)&mdval, sizeof(mdval));
          }
        }

        //Check if item has Marker and write to SBF if it does
        if(bytesToInt((u8*)itemtype) == 0xc847)
        {
          //Get Marker and write to SBF
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int c847size = line.length()/8;
          string c847= "";
          for(int md = 0; md < c847size; md++)
          {
            c847 = line.substr(md*8, 8);
            int mdval = hexStringToInt(c847);
            mdval = intToBytes(mdval);
            sbffile.write((char*)&mdval, sizeof(mdval));
          }
        }

        //Get Number of Colors and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int colors = hexStringToInt(line);
        int colorshex = intToBytes(colors);
        sbffile.write((char*)&colorshex, sizeof(colorshex));

        //Get colors and write to SBF
        for(int c = 0; c < colors; c++)
        {
          //Get Color Fill and write to SBF
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int colorfill = hexStringToInt(line);
          colorfill = intToBytes(colorfill);
          sbffile.write((char*)&colorfill, sizeof(colorfill));
          //Get Color Border and write to SBF
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int colorborder = hexStringToInt(line);
          colorborder = intToBytes(colorborder);
          sbffile.write((char*)&colorborder, sizeof(colorborder));
        }

        //Get Number of Lines (in the text) and write to SBF
        //Note: this value doesn't actually need to be changed.
        //I think it's just for vertical alignment.
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int textlines = hexStringToInt(line);
        textlines = intToBytes(textlines);
        sbffile.write((char*)&textlines, sizeof(textlines));

        //Get Mystery Data 1/2 and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int md1 = hexStringToInt(line);
        md1 = intToBytes(md1);
        sbffile.write((char*)&md1, sizeof(md1));
        //MD2
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int md2 = hexStringToInt(line);
        int md2hex = intToBytes(md2);
        sbffile.write((char*)&md2hex, sizeof(md2hex));
        //MD-2 extra (4bytes each)
        for(int md = 0; md < md2; md++)
        {
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int md2ext = hexStringToInt(line);
          md2ext = intToBytes(md2ext);
          sbffile.write((char*)&md2ext, sizeof(md2ext));
        }

        //Text length (4bytes)
        getline (datafile,line);

        //Get Text and write to SBF
        getline (datafile,line);

        if(externaltxt)
        {
          getline (etextfile, etext);
          while(etext[0] != 'T')
          {
            getline (etextfile, etext);
          }
          line = etext;
        }
        if((int)line[line.length()-1] == 13)
        {
          line = line.substr(0,line.length()-1);
        }

        colon = line.find(':');
        line = line.substr(colon+2);
        line = encodeString(line);
        int linesize = line.length()/4;
        int linesizehex = intToBytes(linesize);
        sbffile.write((char*)&linesizehex, sizeof(linesizehex));

        for(int ls = 0; ls < line.length(); ls+=4)
        {
          char* encChar = (char*)(line.substr(ls, 4)).c_str();
          int hexChar = hexStringToInt(encChar);
          hexChar = intToBytes(hexChar);
          hexChar = hexChar>>16;
          sbffile.write((char*)&hexChar, 2);
        }
      }

      //Images: d
      //Get Number of Images and write to SBF
      getline (datafile,line);
      colon = line.find(':');
      line = line.substr(colon+1);
      line = remove_char(line, ' ');
      int images = hexStringToInt(line);
      int imageshex = intToBytes(images);
      sbffile.write((char*)&imageshex, sizeof(imageshex));
      for(int im = 0; im < images; im++)
      {
        //Get Header
        char* imageheader = new char[4];
        for(int ih = 0; ih < 4; ih++){ imageheader[ih] = 0; }
        getline (datafile,line);

        //FThing, gotta write this to SBF first
        if(line[0] == 'F')
        {
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int fthingsize = line.length()/8;
          string fthingstring= "";
          for(int ft = 0; ft < fthingsize; ft++)
          {
            fthingstring = line.substr(ft*8, 8);
            int ftval = hexStringToInt(fthingstring);
            ftval = intToBytes(ftval);
            sbffile.write((char*)&ftval, sizeof(ftval));
          }
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
        }

        //Get and write image header to SBF
        //Val1
        int val1 = hexStringToInt(line);
        val1 = intToBytes(val1);
        val1 = val1>>24;
        imageheader[0] = val1;
        //Val2
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int val2 = hexStringToInt(line);
        val2 = intToBytes(val2);
        val2 = val2>>24;
        imageheader[1] = val2;
        //Tag
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int tag = hexStringToInt(line);
        tag = intToBytes(tag);
        tag = tag>>24;
        imageheader[2] = tag;
        //Z-Layer
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int zlayer = hexStringToInt(line);
        zlayer = intToBytes(zlayer);
        zlayer = zlayer>>24;
        imageheader[3] = zlayer;
        //Write to SBF
        sbffile.write(imageheader, 4);

        //Get and write X/Y to the SBF (And frame too)
        for(int xy = 0; xy < 3; xy++)
        {
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int imagexy = hexStringToInt(line);
          imagexy = intToBytes(imagexy);
          sbffile.write((char*)&imagexy, sizeof(imagexy));
        }

        //Get ImageID (ImageF) and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int imageidsize = line.length()/8;
        string imageid= "";
        for(int md = 0; md < imageidsize; md++)
        {
          imageid = line.substr(md*8, 8);
          int mdval = hexStringToInt(imageid);
          mdval = intToBytes(mdval);
          sbffile.write((char*)&mdval, sizeof(mdval));
        }

        //Get Image File and write to SBF
        getline (datafile,line);
        colon = line.find(':');
        line = line.substr(colon+2);
        int fnum = 0;
        for(int imf = 0; imf < files; imf++)
        {
          if(strcasecmp(filenames[imf].c_str(), line.c_str()) == 0)
          {
            fnum = imf;
            break;
          }
        }
        fnum = intToBytes(fnum);
        sbffile.write((char*)&fnum, sizeof(fnum));
        //Get image extra data and write to SBF
        if(tag == 4 || tag==5 || tag==6 || tag==7)
        {
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int tag47 = hexStringToInt(line);
          tag47 = intToBytes(tag47);
          sbffile.write((char*)&tag47, sizeof(tag47));
        }
        if(tag==12)
        {
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int tag12 = hexStringToInt(line);
          int feval = (tag12&0xFF);
          tag12 = intToBytes(tag12);
          sbffile.write((char*)&tag12, sizeof(tag12));


          if(feval == 0xFE)
          {
            getline (datafile,line);
            colon = line.find(':');
            line = line.substr(colon+1);
            line = remove_char(line, ' ');
            int tag12size = line.length()/8;
            string tag12ext= "";
            for(int md = 0; md < tag12size; md++)
            {
              tag12ext = line.substr(md*8, 8);
              int mdval = hexStringToInt(tag12ext);
              mdval = intToBytes(mdval);
              sbffile.write((char*)&mdval, sizeof(mdval));
            }
          }
        }
      }

      //Get Extra Data, but possibly endFThing as well.
      getline (datafile,line);

      //Get endFThing and write to file
      if(strcasecmp(line.substr(0,9).c_str(), "endFThing") == 0)
      {
        colon = line.find(':');
        line = line.substr(colon+1);
        line = remove_char(line, ' ');
        int fthingsize = line.length()/8;
        string fthingstring= "";
        for(int ft = 0; ft < fthingsize; ft++)
        {
          fthingstring = line.substr(ft*8, 8);
          int ftval = hexStringToInt(fthingstring);
          ftval = intToBytes(ftval);
          sbffile.write((char*)&ftval, sizeof(ftval));
        }
        getline (datafile,line);
      }

      colon = line.find(':');
      line = line.substr(colon+1);
      line = remove_char(line, ' ');
      int extradata = hexStringToInt(line);
      int edhex = intToBytes(extradata);
      sbffile.write((char*)&edhex, sizeof(edhex));


      //If Extra data, then display
      if(extradata >= 1)
      {
        int code = 0;
        int endval = 4294901888; //0xFFFF0080
        while(code != endval && extradata >= 1)
        {
          //Extra Data
          getline (datafile,line);
          colon = line.find(':');
          line = line.substr(colon+1);
          line = remove_char(line, ' ');
          int eddata = hexStringToInt(line);
          edhex = intToBytes(eddata);
          sbffile.write((char*)&edhex, sizeof(edhex));
          code = eddata;
          if(code == 0x00ff0080)
           break;
          if(code == 0xFF000080)
           extradata -= 1;
        }
      }
      //break;
    }


    //Write a Hex string from TXT to SBF
    /*line = "08181407";
    int intval = hexStringToInt(line);
    intval = intToBytes(intval);
    sbffile.write((char*)&intval, sizeof(intval));*/


    //Close when we are done reading
    datafile.close();
    etextfile.close();
  }
  else cout << "Problem opening SBF file!\n";
  sbffile.close();
}

void padFile(char* filename, int bytes)
{
  //SBF output file
  ofstream file;
  file.open (filename, ios::out | ios::app | ios::binary);
  //strcat(rootname,".sbf")
  //sbffile = fstream("out.bin", ios::out | ios::binary);


  if (file.is_open())
  {
    for(int i = 0; i < bytes; i++)
    {
      file.write((char*)"00",1);
    }
  }
  else cout << "Problem opening file!\n";
  file.close();
}

void extractSBF(char* sbffilename, bool fullextract)
{
  //Extract just text, or extra data as well?
  bool edata = fullextract;

  char* param = sbffilename;
  FILE* inFile = fopen(param, "rb");
  if(inFile == NULL)
  {
    printf("inFile == NULL\n");
    return;
  }
  printf("File OK\n");


  //Get file size
  fseek(inFile, 0, SEEK_END);
  int size = ftell(inFile);
  fseek(inFile, 0, SEEK_SET);
  //printf("input file size: 0x%X\n", size);

  //Variable to hold data
  u8* buff = new u8[size];

  //Load the data
  fread(buff, 1, size, inFile);

  //Done with the file, so close
  fclose(inFile);

  //Set our index
  int index = 4;

  if(buff[0] != 'S' || buff[1] != 'B' || buff[2] != 'F' || buff[3] != '1')
  {
    printf("Not an SBF\n");
    return;
  }


  char* folder = substringCharArray(param, 0, strlen(param)-4);
  std::stringstream sbfloc;
  sbfloc << folder << ".txt";
  std::string sbfname = sbfloc.str();
  ofstream sbffile;
  sbffile.open(sbfname);

  //Store the header for later
  u8* header = new u8[4];
  for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
  int filenum = bytesToInt(header);

  //Create a file index (we'll be saving this later)
  char* fileindex[filenum];

  //Output the number of files
  //cout << "Included Files: " << filenum << endl;

  if(edata)
  sbffile << filenum << endl;

  //Loop through the file list
  for(int i = 0; i < filenum; i++)
  {
    ////cout << i << ". ";

    //Name
    u8* name = new u8[16];
    for(int j = 0; j < 16; j++)
    {
      name[j] = buff[index];
      index++;
    }
    ////cout << name;
    if(edata)
    sbffile << name << endl;
    fileindex[i] = (char*)name;

    //printf("\n");
  }

  //Number of pages
  for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
  int pagenum = bytesToInt(header);
  ////cout << "Pages: " << pagenum << endl;
  if(edata)
  sbffile << "Pages: " << pagenum << endl;

  for(int p = 0; p < pagenum; p++)
  {
    //Page Info
    u8* pageheaderinfo = new u8[4];
    for(int i = 0; i < 4; i++) { pageheaderinfo[i] = buff[index]; index++; }
    //cout << "Page Header: ";
    //outputBytes(pageheaderinfo,4);
    //cout << endl;
    if(edata)
    {
    sbffile << "Page Header: ";
    outputBytesToFile(&sbffile, pageheaderinfo, 4);
    sbffile << endl;
    }

    //Page name length
    for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
    int pagenamelength = bytesToInt(header);

    //Get page name
    u8* pagename = new u8[pagenamelength];
    for(int i = 0; i < pagenamelength; i++)
    {
      pagename[i] = buff[index];
      index++;
    }
    string spagename((char*)pagename);
    spagename = spagename.substr(0,pagenamelength);
    //cout << "Page Name: " << pname << endl;
    sbffile << "Page Name: " << spagename << endl;


    //Is there a menu? And how many?
    for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
    int menu = bytesToInt(header);
    //cout << "Has Menu: " << menu << endl;
    if(edata)
    sbffile << "Has Menu: " << menu << endl;


    int menuitemnum = -1;
    for(int m = 0; m < menu; m++)
    {
      //Menu Header. Important for determining menu data length
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      //cout << "Menu Header: "; outputBytes(header, 4); //cout << endl;
      int menuheader = bytesToInt(header);

      if(edata)
      {
      sbffile << "Menu Header: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
      }

      //Simplified Menu Extraction/Import
      //header[3]
      //>F0 = 13 extra (1 exception)
      //>E0 <F0 = 12 extra (1exception)
      //<70 = 11extra

      //A lot of my founding menu info is probably wrong. This simplified method works.
      //Might have thrown the baby out with the bathwater though, as I assumed I found
      //the amount of menu items at one point. But there doesn't seem to be consistency.
      //So I tossed it. Not important anyway for translation goals.

      int moremenusize = 13*4;

      if(header[3] < 0xF0)
        moremenusize = 12*4;
      if(header[3] < 0x70)
        moremenusize = 11*4;

      //Exceptions - Not sure why these are special.
      //if(menuheader == 0x0240d1)
        //moremenusize = 12*4;
      if(menuheader == 0x260d1)
        moremenusize = 13*4;
      if(menuheader == 0x002107F1)
        moremenusize = 17*4;
      if(menuheader == 0x000900f1)
        moremenusize = 17*4;

      u8* moremenudata = new u8[moremenusize];
      for(int i = 0; i < moremenusize; i++) { moremenudata[i] = buff[index]; index++; }
      //cout << "Menu Data: "; outputBytes(moremenudata, moremenusize); //cout << endl;

      if(edata)
      {
        sbffile << "Menu Data: "; outputBytesToFile(&sbffile, moremenudata, moremenusize); sbffile << endl;
      }
    }

    //Get number of script items (not counting images)
    for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
    int textitemnum = bytesToInt(header);
    //cout << "Text Items: " << textitemnum << endl;
    if(edata)
    sbffile << "Text Items: " << textitemnum << endl;

    //Run through menu items
    for(int mi = 0; mi < textitemnum; mi++)
    {

      //Item Type. Keeping this value is super important.
      u8* itemtype = new u8[4];
      for(int i = 0; i < 4; i++) { itemtype[i] = buff[index]; index++; }

      //cout << "Item Type: ";
      //outputBytes(itemtype,4);
      //cout << endl;
      if(edata)
      {
      sbffile << "Item Type: ";
      outputBytesToFile(&sbffile, itemtype, 4);
      sbffile << endl;
      }


      //X/Y Location
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      int xloc = bytesToInt(header);
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      int yloc = bytesToInt(header);
      //cout << "X/Y: " << xloc << ", " << yloc << endl;
      if(edata)
      sbffile << "X-Loc: " << xloc << endl << "Y-Loc: " << yloc << endl;


      //Old checks
      //if(itemtype[2] != 0x14)//(itemtype[1] != 0x18)//(bytesToInt(itemtype,4) == 293879815)//(mi < menuitemnum)
      //if(itemtype[2] != 0x14 && bytesToInt(itemtype) != 05 && itemtype[3] != 0x88
      //  && bytesToInt(itemtype) != 0x02000407 && !(itemtype[0] == 0x00 && (itemtype[1] == 0x20 || itemtype[1] == 0x40)) )

      //The infamous MenuF. Everything besides 0x14 type[3] items can have something be placed here.
      //Some data put here might be more fitting under marker. But it's okay. Everything is filtered correctly.
      if(itemtype[2] != 0x14)
      {
        //Just a bunch of FF's. Might be important though
        u8* menuf = new u8[8];
        for(int i = 0; i < 8; i++) { menuf[i] = buff[index]; index++; }
        //cout << "MenuF: "; outputBytes(menuf, 8); //cout << endl;
        if(edata)
        {
        sbffile << "MenuF: "; outputBytesToFile(&sbffile, menuf, 8); sbffile << endl;
        }
      }

      //Some sort of marker/header. Seems to determine triggered events/linked pictures
      //Everything with the third byte above 5 has values here. Might not all be used the same
      u8* marker = new u8[8];
      if(itemtype[2] > 5)
      {
        for(int i = 0; i < 8; i++) { marker[i] = buff[index]; index++; }
        //cout << "Marker: "; outputBytes(marker, 8); //cout << endl;
        if(edata)
        {
        sbffile << "Marker: "; outputBytesToFile(&sbffile, marker, 8); sbffile << endl;
        }
      }

      //0000 c847 - Has a mystery value (Perhaps a second marker?)
      if(bytesToInt(itemtype) == 0xc847)
      {
        for(int i = 0; i < 8; i++) { marker[i] = buff[index]; index++; }
        //cout << "c847-val: "; outputBytes(marker, 8); //cout << endl;
        if(edata)
        {
        sbffile << "c847-val: "; outputBytesToFile(&sbffile, marker, 8); sbffile << endl;
        }
      }


      //How many colors for text
      u8* colors = new u8[4];
      for(int i = 0; i < 4; i++) { colors[i] = buff[index]; index++; }
      int icolors = bytesToInt(colors, 4);
      //cout << "Colors: "; outputBytes(colors, 4); //cout << endl;
      if(edata)
      {
      sbffile << "Colors: "; outputBytesToFile(&sbffile, colors, 4); sbffile << endl;
      }

      //The amount of text colors is determined above. Here we have fill/border for each color (if any)
      for(int c = 0; c < icolors; c++)
      {
        //Text Fill Color
        u8* textfillcolor = new u8[4];
        for(int i = 0; i < 4; i++) { textfillcolor[i] = buff[index]; index++; }
        //cout << "Text Fill Color: "; outputBytes(textfillcolor, 4); //cout << endl;
        if(edata)
        {
        sbffile << "Text Fill Color: "; outputBytesToFile(&sbffile, textfillcolor, 4); sbffile << endl;
        }
        //Text Border Color
        u8* textbordercolor = new u8[4];
        for(int i = 0; i < 4; i++) { textbordercolor[i] = buff[index]; index++; }
        //cout << "Text Border Color: "; outputBytes(textbordercolor, 4); //cout << endl;
        if(edata)
        {
        sbffile << "Text Border Color: "; outputBytesToFile(&sbffile, textbordercolor, 4); sbffile << endl;
        }
      }

      //Number of Lines
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      if(edata)
      {
      sbffile << "Lines: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
      }


      //Everything needs mystery data!
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      if(edata)
      {
      sbffile << "Mystery Data 1: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
      }

      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      if(edata)
      {
      sbffile << "Mystery Data 2: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
      }
      int mnum2 = bytesToInt(header, 4);

      //Grabbing extra Mystery Data 2 info
      for(int m = 0; m < mnum2; m++)
      {
        for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
        if(edata)
        {
        sbffile << "MD2-ext: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
        }
      }



      //Text length - note: may not actually be 12 length.
      //But that's the amount of space, so we'll go with it.
      //Update: Text Length appears to be 4. There's still 2 mystery values that are 0 for most things.
      //But they seem to be indicators. S_CAST has the second value indicate some more mystery data.
      u8* textlength = new u8[4];
      for(int i = 0; i < 4; i++) { textlength[i] = buff[index]; index++; }
      int textlengthint = bytesToInt(textlength, 4);
      ////cout << "Text Length: " << textlengthint << endl;
      if(edata)
      {
      sbffile << "Text Length: ";
      outputBytesToFile(&sbffile, textlength, 4);
      sbffile <<  endl;
      }

      u8* text = new u8[textlengthint*2];
      for(int t = 0; t < textlengthint*2; t++)
      {
        text[t] = buff[index];
        index++;
      }
      //cout << "Text: " << decodeString(text, textlengthint*2) << endl;
      sbffile << "Text: " << decodeString(text, textlengthint*2) << endl;
      ////cout << "Text: "; outputBytes(text, textlengthint*2); //cout << endl;
    }

    //Number of images
    for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
    int imagenum = bytesToInt(header);
    //cout << "Images: " << imagenum << endl;
    if(edata)
    sbffile << "Images: " << imagenum << endl;

    //Run through Images
    for(int im = 0; im < imagenum; im++)
    {
      ////cout << "-New Image-" << endl;
      int val1 = 0;
      val1 = (val1 << 8) + buff[index];
      if(val1 >= 0xF0)
      {
        u8* fthing = new u8[8];
        for(int i = 0; i < 8; i++) { fthing[i] = buff[index]; index++; }
        //index+=8;
        if(edata)
        {
        sbffile << "FThing: "; outputBytesToFile(&sbffile, fthing, 8); sbffile << endl;
        }

        val1 = (val1 << 8) + buff[index];
      }
      ////cout << "Val1: " << val1 << endl;
      if(edata)
      sbffile << "Val1: " << val1 << endl;
      int val2 = 0;
      val2 = (val2 << 8) + buff[index+1];
      ////cout << "Val2: " << val2 << endl;
      if(edata)
      sbffile << "Val2: " << val2 << endl;

      int tag = 0;
      tag = (tag << 8) + buff[index+2];
      ////cout << "Tag: " << tag << endl;
      if(edata)
      sbffile << "Tag: " << tag << endl;

      int zlayer = 0;
      zlayer = (zlayer << 8) + buff[index+3];
      ////cout << "Z-Layer: " << zlayer << endl;
      if(edata)
      sbffile << "Z-Layer: " << zlayer << endl;

      index+=4;

      //X/Y Location
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      int xloc = bytesToInt(header);
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      int yloc = bytesToInt(header);
      ////cout << "X/Y: " << xloc << ", " << yloc << endl;
      if(edata)
      sbffile << "X-Loc: " << xloc << endl << "Y-Loc: " << yloc << endl;

      //Just some Zeros
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      if(edata)
      {
      sbffile << "Frame: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
      }

      u8* imageid = new u8[8];
      for(int i = 0; i < 8; i++) { imageid[i] = buff[index]; index++; }
      ////cout << "Image ID: "; outputBytes(imageid, 8); //cout << endl;
      if(edata)
      {
      sbffile << "Image ID: "; outputBytesToFile(&sbffile, imageid, 8); sbffile << endl;
      }


      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
      int imagefile = bytesToInt(header);
      ////cout << "Bytes: "; outputBytes(header, 4); //cout << endl;
      if(imagefile > filenum)
      {
        //cout << "Problem ImageFile: " << imagefile << " / " << filenum << endl;
        sbffile << "Problem Image File: ";
        outputBytesToFile(&sbffile, header, 4);
        sbffile << endl;
        return;
      }
      //cout << "Image File: " << fileindex[imagefile] << endl;
      if(edata)
      sbffile << "Image File: " << fileindex[imagefile] << endl;

      if(tag == 4 || tag==5 || tag==6 || tag==7)
      {
        for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
        if(edata)
        {
        sbffile << "Tag 4/5 6/7: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
        }
      }

      //Not sure why tags 6/7 have extra data. Only figured it out when debugging
      //haven't tested anything yet
      if(tag==12)
      {
        for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
        if(edata)
        {
        sbffile << "Tag 12: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
        }

        if(header[3] == 0xFE)
        {
        u8* tag12 = new u8[8];
        for(int i = 0; i < 8; i++) { tag12[i] = buff[index]; index++; }
        if(edata)
        {
        sbffile << "Tag 12-ext: "; outputBytesToFile(&sbffile, tag12, 8); sbffile << endl;
        }
        }
      }

      /*tag: 00 00 00
      z-layer: 00
      x\y: 00 00 00 20 \ 00 00 00 12
      00 00 00 00
      elementid?: FF FF FF FF (00 00 02 58)
      FF FF FF FF
      img file: 00 00 00 05
      extra: 00 00 00 AF/E0 (if tag is 4/5)*/
    }
    for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
    if(header[0] == 255)
    {
      index-=4;
      u8* endfthing = new u8[12];
      for(int i = 0; i < 12; i++) { endfthing[i] = buff[index]; index++; }
      if(edata)
      {
      sbffile << "endFThing: "; outputBytesToFile(&sbffile, endfthing, 12); sbffile << endl;
      }
      for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }
    }

    if(edata)
    {
    sbffile << "Extra Data?: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
    }

    int extradata = bytesToInt(header);
    if(extradata >= 1)
    {
      //index+=32;
      int code = 0;
      int endval = 4294901888;
      while(code != endval && extradata >= 1)
      {
        for(int i = 0; i < 4; i++) { header[i] = buff[index]; index++; }

        if(edata)
        {
        sbffile << "Misc Extra Data: "; outputBytesToFile(&sbffile, header, 4); sbffile << endl;
        }

        code = bytesToInt(header);
        if(code == 0x00ff0080)
          break;
        if(code == 0xFF000080)
          extradata -= 1;
      }
    }

    //cout << "-End of Page-" << endl;
    //return;
  }

  sbffile.close();

  delete [] buff;
}

int main(int argc, char* argv[])
{
  if(argc < 3)
  {
    cout << "Usage: ./pdpdec file mode [extra]" << endl;
    cout << "extractbin: Extracts a .BIN file into a folder of the same name." << endl;
    cout << "createbin: Creates a .BIN file using a folder of the same name." << endl;
    cout << "padfile: Pads a given file with [extra] amount of bytes." << endl;
    cout << "extractsbf: Extracts an SBF file to a txt file of the same name. Set [extra] to false to only extract text." << endl;
    cout << "createsbf: Creates an SBF from a txt file of the same name. Set [extra] to true to use an external txt file of name [file]tr.txt\n";
    return EXIT_FAILURE;
  }

  char* param = argv[1];

  char* mode = argv[2];

  if(strcasecmp(mode,"createbin") == 0)
    createBIN(param);
  if(strcasecmp(mode,"extractbin") == 0)
    extractBIN(param);

  if(strcasecmp(mode,"padfile") == 0)
    padFile(param, atoi(argv[3]));

  if(strcasecmp(mode, "generatetext") == 0)
  {
    cout << encodeString(param) << endl;
  }

  if(strcasecmp(mode, "extractsbf") == 0)
  {
    bool fullextract = true;
    if(argc > 3)
    {
      char* fe = argv[3];
      fullextract = (strcasecmp("true",fe) == 0);
    }

    extractSBF(param, fullextract);
  }
  if(strcasecmp(mode, "createsbf") == 0)
  {
    bool externaltxt = false;
    if(argc > 3)
    {
      char* et = argv[3];
      externaltxt = (strcasecmp("true",et) == 0);
    }
    createSBF(param, externaltxt);
  }


  return EXIT_SUCCESS;
}