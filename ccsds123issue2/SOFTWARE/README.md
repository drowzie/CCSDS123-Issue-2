# CCSDS123 Issue 2
An implementation of CCSDS 123 Issue 2. The hybrid encoder is implemented but the decoder is not implemented as there was little to no time by the time i had to write the thesis. Sample adaptive works but not crossing from BSQ to BIP/Decoding BSQ to BIP...etc. The same problem is also in Issue 1 in this repository. This project should work as a reference to build upon where i have done some testing with the reference images/compressed images provided by CCSDS. There is support for reading 16bit/32bit big endian/low endian but i did not insert it into the available arguments for when to use 16/32 in the code. One would have to manually change the conversion function in the code if used. 
## Building the program using CMAKE
Requirements:
* CMAKE
* GCC
* LINUX (Windows can be used with WSL)

Optional:
* ccache

To build the program perform the following steps:
1. Clone/Download the repository
2. CD to the ccsds123issue2/SOFTWARE directory
3. Create the build directory with the command ```mkdir build```.
4. Set the build and source directory for cmake by running the command ```cmake -S . -B ./build/```
* Some configuration can be set by running ```ccmake . in /build/ directory``` but is not required.
5. Compile by running ```cmake --build ./build```. This will create a folder called binaries where the program ```ccsds``` can be found. This is the main proram compiled from main.c and the included libraries.

