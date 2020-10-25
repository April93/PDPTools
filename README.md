# PDPTools

These are tools used to edit various file formats for the Panel de Pon games, with a primary focus on Panel de Pon 64, which is included in Nintendo Puzzle Collection. These tools should also work on Pokemon Puzzle League given the shared codebase.

## pdpdec

pdpdec is able to extract and compile the .bin archives, including the ones embedded in panepon.rel, and the pokemon puzzle league rom. It can also convert sbf files into a more readable format.

Usage: ./pdpdec file mode [extra] 

Here are the various modes/functions: 

extractbin: Extracts a .BIN file into a folder of the same name.  
createbin: Creates a .BIN file using a folder of the same name.  
padfile: Pads a given file with [extra] amount of bytes.  
extractsbf: Extracts an SBF file to a txt file of the same name. Set [extra] to false to only extract text.  
createsbf: Creates an SBF from a txt file of the same name. Set [extra] to true to use an external txt file of name [file]tr.txt 

compstory.sh and comptuts.sh are example scripts that compile sbf files into the proper bin, and rebuilds the bin archive.

## PDPI

PDPI allows for basic editing of .BIF files (with magic byte header 3FIB).

When prompted, you can use the following commands:

exportall: Exports all of the frames of the bif file into pngs.  
import: Imports a png file into a frame of the bif.  
save: Saves the bif file.  

## extractsongs.py

extractsongs.py is a simple script that extracts all of the musyx files from panepon.plf.

## Proprietary Tools

Inside of the "proprietary" folder are some other tools that are able to edit panel de pon files. Namely there is PanelDeTool.jar and szp.exe. PanelDeTool is a gui tool that is able to open .bin files and view the related .bif images and edit them. szp.exe is a tool used to compress panepon.rel into panepon.szp, which is needed to properly import the edited panepon.rel file.