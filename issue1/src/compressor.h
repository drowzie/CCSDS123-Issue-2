#include <inttypes.h>
#include <cliparser/cli.h>
int compressImage(uint16_t * image, uint8_t * compressedImage, unsigned int * totalWrittenBytes, struct arguments * parameters);
int decompressImage(FILE * compressedImage, FILE * decompressedImageFile, struct arguments * parameters);