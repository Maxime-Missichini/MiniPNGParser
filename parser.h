#ifndef TPSEC_PARSER_H
#define TPSEC_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>

void readBytes(char * bytes, int len);
void printImage(const int * bits, int width, int height);
void bytesToBits(const char * bytes, int blockSize, int * bits);
void verifyFile(FILE * file);
void readDBlock(FILE *file, int width, int height, int type, int previous, int *bitmap);
void readCBlock(FILE *file, int width, int height, int type, bool invalid, int previous, int *bitmap);
void readBlocks(FILE * file);
void parser(char * fileName);
void testBlackAndWhite();
void testGrey();

#endif //TPSEC_PARSER_H
