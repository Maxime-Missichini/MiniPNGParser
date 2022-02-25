#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

void readBytes(char * bytes, int len) {
    for (int i=0; i<len; i++){
        printf("%c", bytes[i]);
    }
}

void printImage(const int * bits, int width, int height) {
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            if (bits[i*height+j] == 1) printf(" ");
            else printf("X");
        }
        printf("\n");
    }
}

int * bytesToBits(const char * bytes, int blockSize) {
    int * bits = (int *)malloc(sizeof(int) * blockSize * 8);

    for(int i=0; i < blockSize; i++){
        for(int j=0; j<8; j++) {
            bits[(i*blockSize)+j] = ((bytes[i] << j) & 0x80) ? 1 : 0;
        }
    }

    return bits;
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

void readDHeader(FILE * file, int width, int height, int type) {
    char headBuffer; // Just to verify that the first letter correspond to the block name
    int blockSize;
    char * bytes = (char *) malloc(((width*height)/8)*sizeof(char));

    fread(&headBuffer, sizeof(char), 1, file);

    if (strncmp(&headBuffer, "D", sizeof(char)) != 0){
        printf("Error: Invalid block D structure\n");
        fclose(file);

        exit(1);
    }

    fread(&blockSize, sizeof(int), 1, file);

    blockSize = htonl(blockSize); // Convert to bigendian

    printf("Block size: %d\n", blockSize);
    if (type == 1){
        if(blockSize*8 != (width*height)) {
            printf("Error block D is not containing data corresponding to previous information from block H\n");
            fclose(file);
            exit(1);
        }
    }

    fread(bytes, sizeof(char), blockSize, file);

    int * bits = bytesToBits(bytes, blockSize);

    printImage(bits, width, height);

    free(bits);
    free(bytes);
}

void readCHeader(FILE * file, int width, int height, int type) {
    char headBuffer; // Just to verify that the first letter correspond to the block name
    int blockSize;

    fread(&headBuffer, sizeof(char), 1, file);

    if (strncmp(&headBuffer, "C", sizeof(char)) != 0){
        printf("Error: Invalid block C structure\n");
        fclose(file);
        exit(1);
    }

    fread(&blockSize, sizeof(int), 1, file);

    blockSize = htonl(blockSize); // Convert to bigendian

    printf("Block size: %d\n", blockSize);

    char lineBuffer[blockSize];

    printf("Commentaires:\n");

    fread(lineBuffer, sizeof(char), blockSize, file);

    printf("\"");
    readBytes(lineBuffer, blockSize);
    printf("\"\n");

    readDHeader(file, width, height, type);
}

void readHHeader(FILE * file) {
    char headBuffer; // Just to verify that the first letter correspond to the block name
    int blockSize;
    int widthBuffer; // Buffer that contains width (4 bytes)
    int heightBuffer; // Buffer that contains height (4 bytes)
    char typeBuffer;

    fread(&headBuffer, sizeof(char), 1, file);

    if (strncmp(&headBuffer, "H", sizeof(char)) != 0){
        printf("Error: Invalid block H structure\n");
        fclose(file);
        exit(1);
    }

    fread(&blockSize, sizeof(int), 1, file);

    blockSize = htonl(blockSize); // Convert to bigendian

    printf("Block size: %d\n", blockSize);


    fread(&widthBuffer, sizeof(int), 1, file);

    widthBuffer = htonl(widthBuffer); // Convert to bigendian

    printf("Width: %d\n", widthBuffer);


    fread(&heightBuffer, sizeof(int), 1, file);

    heightBuffer = htonl(heightBuffer); // Convert to bigendian

    printf("Height: %d\n", heightBuffer);

    fread(&typeBuffer, sizeof(char), 1, file);

    printf("Pixel type: %x ", typeBuffer);

    if (typeBuffer == 0) {
        printf("(Black and white)\n");
    }else if (typeBuffer == 1) {
        printf("(50 Shade of grey (lol))\n");
    }else if (typeBuffer == 2) {
        printf("(Palette)\n");
    }else if (typeBuffer == 3) {
        printf("(24 bits colors)\n");
    }else{
        printf("Error in pixel type\n");
        fclose(file);
        exit(1);
    }

    readCHeader(file, widthBuffer, heightBuffer, typeBuffer);

}

void parser(char * fileName) {
    FILE * file;

    file = fopen(fileName,"r");

    if (file == NULL){
        printf("Error opening the file\n");
        exit(1);
    }

    printf("File content:\n");

    verifyFile(file);

    readHHeader(file);

    fclose(file);
}

int main(int argc, char ** argv) {

    if (argc != 2) {
        printf("Error: no file path provided\n");
        exit(1);
    }

    parser(argv[1]);

    return 0;
}
