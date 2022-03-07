#include "parser.h"

void readBytes(char * bytes, int len) {
    for (int i=0; i<len; i++){
        printf("%c", bytes[i]);
    }
}

void printImage(const int *bits, int width, int height, int type) {
    if (type == 0) {
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                if (bits[i * height + j] == 1) printf(" ");
                else printf("X");
            }
            printf("\n");
        }
    }else if (type == 1){
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                printf("%02X", bits[i * height + j]);
            }
            printf("\n");
        }
    }
}

void bytesToBits(const char * bytes, int blockSize, int * bits) {
    for(int i=0; i < blockSize; i++){
        for(int j=0; j<8; j++) {
            bits[(i*blockSize)+j] = ((bytes[i] << j) & 0x80) ? 1 : 0;
        }
    }
}

void verifyFile(FILE * file) {
    char buffer[8];

    fread(buffer, sizeof(char), 8, file);

    readBytes(buffer, 8);
    printf("\n");

    if (strncmp(buffer, "Mini-PNG", sizeof(char) * 8) != 0){
        printf("Error: file does not begin with Mini-PNG\n");
        fclose(file);
        exit(1);
    }

}

void readDBlock(FILE *file, int width, int height, int type, int previous, int *bitmap, char lastRead) {
    char headBuffer; // Just to verify that the first letter correspond to the block name
    int blockSize;
    char * bytes = (char *) malloc(sizeof(char)*((width*height)/8));

    if (lastRead != 'D') fread(&headBuffer, sizeof(char), 1, file);

    if (strncmp(&headBuffer, "D", sizeof(char)) != 0){
        if(strncmp(&headBuffer, "C", sizeof(char)) != 0 && lastRead != 'D'){
            printf("Error: Invalid block D structure\n");
            fclose(file);
            exit(1);
        }else if(lastRead != 'D'){
            readCBlock(file, width, height, type, true, 0, bitmap);
        }
    }

    fread(&blockSize, sizeof(int), 1, file);

    blockSize = htonl(blockSize); // Convert to bigendian

    printf("Block D size: %d\n", blockSize);

    fread(bytes, sizeof(char), blockSize, file);

    if (type == 0){
        bytesToBits(bytes, blockSize, bitmap);

        // Safety
        if((blockSize*8 + previous) != (width*height)) {
            readCBlock(file, width, height, type, true, blockSize * 8, bitmap);
        }

        printImage(bitmap, width, height, 0);
        free(bytes);

    }else if (type == 1){

        for(int i=0; i<blockSize;i++){
            bitmap[previous+i] = (int)bytes[i];
        }

        if((blockSize + previous) != (width*height)) {
            readCBlock(file, width, height, type, true, blockSize, bitmap);
        }

        printImage(bitmap, width, height, 1);
        free(bytes);
    }
}

void readCBlock(FILE *file, int width, int height, int type, bool invalid, int previous, int *bitmap) {
    char headBuffer; // Just to verify that the first letter correspond to the block name
    int blockSize;

    fread(&headBuffer, sizeof(char), 1, file);

    if (strncmp(&headBuffer, "C", sizeof(char)) != 0){
        // Block C is not mandatory
        if (previous != 0) {
            return;
        }

        if (!invalid && strncmp(&headBuffer, "D", sizeof(char)) == 0) {
            readDBlock(file, width, height, type, 0, bitmap, 'D');
            return;
        }
        else return;
    }

    fread(&blockSize, sizeof(int), 1, file);

    blockSize = htonl(blockSize); // Convert to bigendian

    printf("Block C size: %d\n", blockSize);

    char lineBuffer[blockSize];

    printf("Comments:\n");

    fread(lineBuffer, sizeof(char), blockSize, file);

    printf("\"");
    readBytes(lineBuffer, blockSize);
    printf("\"\n");

    readDBlock(file, width, height, type, 0, bitmap, 0);
}

void readBlocks(FILE * file) {
    char headBuffer; // Just to verify that the first letter correspond to the block name
    int blockSize;
    int widthBuffer; // Buffer that contains width (4 bytes)
    int heightBuffer; // Buffer that contains height (4 bytes)
    char typeBuffer;

    fread(&headBuffer, sizeof(char), 1, file);

    // Block H is mandatory! We don't want to blindly read user's file
    if (strncmp(&headBuffer, "H", sizeof(char)) != 0){
        printf("Error: Invalid block H structure\n");
        fclose(file);
        exit(1);
    }

    fread(&blockSize, sizeof(int), 1, file);

    blockSize = htonl(blockSize); // Convert to bigendian

    printf("Block H size: %d\n", blockSize);


    fread(&widthBuffer, sizeof(int), 1, file);

    widthBuffer = (int)htonl(widthBuffer); // Convert to bigendian

    printf("Width: %d\n", widthBuffer);


    fread(&heightBuffer, sizeof(int), 1, file);

    heightBuffer = (int)htonl(heightBuffer); // Convert to bigendian

    printf("Height: %d\n", heightBuffer);

    fread(&typeBuffer, sizeof(char), 1, file);

    printf("Pixel type: %x ", typeBuffer);

    if (typeBuffer == 0) {
        printf("(Black and white)\n");
    }else if (typeBuffer == 1) {
        printf("(50 Shade of grey)\n");
    }else if (typeBuffer == 2) {
        printf("(Palette)\n");
        printf("This type of pixel is not currently supported.\n");
        fclose(file);
        exit(1);
    }else if (typeBuffer == 3) {
        printf("(24 bits colors)\n");
        printf("This type of pixel is not currently supported.\n");
        fclose(file);
        exit(1);
    }else{
        printf("Error in pixel type\n");
        fclose(file);
        exit(1);
    }

    // *2 in size for safety measures, corrupted top size otherwise
    int * bitmap = (int *)malloc(sizeof(int) * (widthBuffer * heightBuffer) * 2 * 8);

    readCBlock(file, widthBuffer, heightBuffer, typeBuffer, false, 0, bitmap);
}

void parser(char * fileName) {
    FILE * file;

    file = fopen(fileName,"r");

    if (file == NULL){
        printf("Error opening the file %s\n", fileName);
        exit(1);
    }

    printf("File %s content:\n", fileName);

    verifyFile(file);

    readBlocks(file);

    printf("\n\n\n");

    fclose(file);
}

void testBlackAndWhite() {
    parser("../minipng-samples/bw/ok/A.mp");
    parser("../minipng-samples/bw/ok/Maxime.mp");
    parser("../minipng-samples/bw/ok/black.mp");
    parser("../minipng-samples/bw/ok/split-black.mp");
    parser("../minipng-samples/bw/ok/uneven-dimensions.mp");

    // Not supported, see doc to know why
    // parser("../minipng-samples/bw/ok/unordered_A.mp");

    // These files are not ok so it crashes
    // parser("../minipng-samples/bw/nok/missing-data.mp");
    // parser("../minipng-samples/bw/nok/missing-header.mp");
    // parser("../minipng-samples/bw/nok/wrong-magic.mp");
}

void testGrey() {
    parser("../minipng-samples/other/ok/gray.mp");
}

int main(int argc, char ** argv) {

    if (argc != 2) {
        printf("Error: no file path provided\n");
        exit(1);
    }

    // There is two choices here, either we just run test typing -t or we just
    // want to use the parser on a single file
    if (strncmp(argv[1], "-t", sizeof(char)*2) == 0){
        testBlackAndWhite();
        testGrey();
    }else{
        parser(argv[1]);
    }

    return 0;
}
