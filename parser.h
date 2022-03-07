#ifndef TPSEC_PARSER_H
#define TPSEC_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <stdbool.h>

/**
 * Displays the array of bytes.
 * @param bytes array to be displayed
 * @param len length of this array
 */
void readBytes(char * bytes, int len);

/**
 *
 * @param bits bitmap array to be displayed.
 * @param width of the image
 * @param height of the image
 * @param type pixel type for the image
 */
void printImage(const int *bits, int width, int height, int type);

/**
 * Convert an array of bytes to an array of bits (bitmap).
 * @param bytes to be converted to bits
 * @param blockSize length of the bytes array
 * @param bits bitmap to store bytes content in bits
 */
void bytesToBits(const char * bytes, int blockSize, int * bits);

/**
 * Verifies that this is a mini-png file (starting with MINI-PNG).
 * @param file file pointer
 */
void verifyFile(FILE * file);

/**
 * Reads D block and stores the data inside bitmap. Prints image when done. This function is calling readCBlock when D
 * blocks are slitted.
 * @param file file pointer
 * @param width of the image
 * @param height of the image
 * @param type of pixels
 * @param previous number of bytes (or bits if it's BW) red previous D block
 * @param bitmap of the image
 * @param lastRead last letter red (either NULL or 'D')
 */
void readDBlock(FILE *file, int width, int height, int type, int previous, int *bitmap, char lastRead);

/**
 * Reads C block and display its content. This function is calling readDblock if there is comments between D blocks/
 * @param file file pointer
 * @param width of the image
 * @param height of the image
 * @param type of pixels
 * @param invalid boolean to know if there was a D between this call and previous call of C
 * @param previous number of bytes (or bits if it's BW) red previous D block
 * @param bitmap of the image
 */
void readCBlock(FILE *file, int width, int height, int type, bool invalid, int previous, int *bitmap);

/**
 * Reads header of the file.
 * @param file file pointer
 */
void readBlocks(FILE * file);

/**
 * Main function to parse files
 * @param fileName path to the file
 */
void parser(char * fileName);

/**
 * Tests all supported BW images
 */
void testBlackAndWhite();

/**
 * Tests all supported grey scale images
 */
void testGrey();

#endif //TPSEC_PARSER_H
