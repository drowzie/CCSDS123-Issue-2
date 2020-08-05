# CCSDS123 Issue 2
An implementation of CCSDS 123 Issue 2. An hybrid encoder is implemented but the decoder is not implemented yet(soon(tm)).
If compiling with warnings enabled (eg Wall) there will be loads of errors because 6 months in this project i found out that it was not enabled during development.
Sample adaptive works perfectly.
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

