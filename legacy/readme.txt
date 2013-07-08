  APNG Assembler 2.7
 
  Creates APNG animation from PNG/TGA image sequence.
 
  http://apngasm.sourceforge.net/
 
  Copyright (c) 2010-2012 Max Stepin
  maxst@users.sourceforge.net

  License: zlib license

--------------------------------

  Changes in version 2.7:

- Better optimization after loading paletted PNG files

  Changes in version 2.6:

- Fixed a bug with loading 1,2,4-bit paletted PNG files

  Changes in version 2.5:

- Fixed a bug with loading mode2 or mode0 input files with tRNS

  Changes in version 2.4:

- Reads delay information from .txt files
- Accepts input files of different color types
- Better optimization
- zlib license

--------------------------------

  Usage: 

Usage: 
apngasm output.png frame001.png [options]
apngasm output.png frame*.png   [options]

Options :
1 10    : frame delay is 1/10 sec. (default)
/l2     : 2 loops (default is 0, forever)
/f      : skip the first frame
/kp     : keep palette
/kc     : keep color type

----------------------------------------------------------------
Example 1:

Let's say you have following frame sequence:
frame01.png
frame02.png
frame03.png

And you want to have 3/4 seconds delay between frames.
The correct command will be

apngasm output.png frame01.png 3 4


If frame02.txt is found with the following one-line content, 
it will override delay information for frame 2:

delay=25/100

----------------------------------------------------------------
Example 2: 

The same as above, but you added "invisible" frame00.png :
frame00.png - invisible
frame01.png
frame02.png
frame03.png

The correct command will be

apngasm output.png frame00.png 3 4 /f

That way APNG supported browsers and image viewers 
will show frame01-frame02-frame03 animation, 
while IE and Chrome will display static frame00.png image.

Some people like to put "your browser sucks" message in that 
frame00.png but keep in mind that you will see that message 
in thumbnails when you are browsing image folders, 
or using google image search. So be careful.

----------------------------------------------------------------
Example 3: 

apngasm output.png frame01.png

That way you'll get 1/10 sec delay.

----------------------------------------------------------------
Some optimizations used in APNG Assembler might re-sort the
palette, or change the color type from RGBA and RGB modes
to RGB and indexed modes. Those optimizations are only performed
when they are lossless, but if you want to avoid changing the
palette or colortype, use those switches to turn them off:

/kp     : keep palette
/kc     : keep color type

