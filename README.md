# MiniPNGParser
School project of a MiniPNG parser in C.

## How to use

Compile with gcc:\
<code>
gcc ./parser.c -o parser
</code>\
Compile with Cmake:\
<code>
cmake --install .
</code>\
<code>
cmake --build .
</code>\
<code>
make
</code>

If you want to parse a single file\
<code>
./parser file_path
</code>

If you want to run parser tests (BW and grey scale only)\
<code>
./parser -t
</code>
## Documentation

You can find all the code documentation inside the header file. There is additional comments inside class file.\
Here are the supported features from Mini-PNG:
<ol>
<li>Reads Header block H</li>
<li>Reads zero, one or more Comment block C</li>
<li>Reads one or more Data block D</li>
<li>Reads images with pixel type set to 0 (Black and white) or 1 (grey scale)</li>
<li>Reads grey scale image but only displaying bytes value (it gives a hint to the image shape)</li>
</ol>