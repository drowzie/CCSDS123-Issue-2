# CCSDS Compression Software Guide
Guide is intended for usage guide of this software. 

## Usage

Compiling of this program is done by the make program in Software main folder. 
Requires GCC and make to be able to compile this software. Once compiled the program main.out can be run.


Running the command ./main.out -? will give out the guide and accepted parameters for the program.

```
./main.out -?
Usage: main.out [OPTION...] 
CCSDS 123 Issue 2

  -d, --Dynamic Range=DYNRANGE   Register D size, #2-16
      --DECOMPRESSION        DECOMPRESSION MODE. Input image is the compressed
                             image
  -f, --Full Prediction Mode Calculate in Full Prediction Mode
                             (DEFAULT=REDUCED)
  -i, --inputFILE=FILE       FILENAME
  -o, --imageorder=ORDER     0=BSQ, 1=BIP or 2=BIL
  -O, --imageorder=ENCODEORDER   0=BSQ, 1=BIP or 2=BIL
  -p, --Prediction Bands=PBANDS   How many preceding spectral bands to use.
                             #0-16
  -r, --Register size=REGSIZE   Register R size, max{32, D +  Ω +  2} ≤ R
                             ≤ 64
  -s, --Sample resolution=SRES   Sample Resolution(Θ), #0-4
      --SIGNED               SIGNED PIXELS
  -t, --Weight interval=Winterval
                             Chapter 4.8.2, CCSDS Issue 2
  -v, --Vmin=vMin            Chapter 4.8.2, CCSDS Issue 2
  -V, --Vmax=vMax            Chapter 4.8.2, CCSDS Issue 2
  -w, --Weight resolution=WRES   Weight Resolution(Omega), #4-19
  -x, --xSize=xSIZE          x size of image
  -y, --ySize=ySIZE          y size of image
  -z, --zSize=zSIZE          z size of image
  -?, --help                 Give this help list
      --usage                Give a short usage message
      --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to <chrisboltern3@gmail.com>.
```

Many of these parameters is not necesarry to change or use.
Fundementally the important parameter is `-i` for selecting input image,
`-x, '-y' and '-z'` for selecting cube size and `-f` for full compression mode, and
`--SIGNED` if the image is stored as signed where unsigned is used as default.
e.g `./main.out -i test.bsq -x 10 -y 10 -z 10 -f --SIGNED`
**NOTE: The compression algorithm will put all compressed images into Encoded.bin.**

### Decompression
This program can decompress images produced by the compression stage.
The command: `./main.out -i Encoded.bin --DECOMPRESSION` will do decompression on the input image Encoded.bin. 
**NOTE: Decompression will put decompressed images into a file called DecompressedFile**
