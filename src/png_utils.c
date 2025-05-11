#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <zlib.h>

#include "png_utils.h"


// ----------helper functions------------------------
static int check_header(char* header) {
	
	char pngHeader[9] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
	
	return (memcmp(header, pngHeader, 8));

}

static int invert_endianness(int in) {
	int out = ((in>>24)&0xff) | // move byte 3 to byte 0
        ((in<<8)&0xff0000) | // move byte 1 to byte 2
        ((in>>8)&0xff00) | // move byte 2 to byte 1
        ((in<<24)&0xff000000); // byte 0 to byte 3
	return out;
}

// given the header and data for a chunk, and the chunk's crc, verifies the crc
static int verify_crc(char* buffer, char* crc, int length) {
	uint32_t actual_crc = crc32(0, Z_NULL, 0);
	actual_crc = crc32(actual_crc, buffer, length);
	// invert cuz machine is little endian but crc is big endian
	actual_crc = invert_endianness(actual_crc);
	return memcmp(&actual_crc, crc, 4);
}

static void init_png(png_image* png) {
	png->data = NULL;
	png->width = 0;
	png->height = 0;
	png->bit_depth = 0;
    png->color_type = 0;
    png->compression = 0;
    png->filter = 0;
    png->interlace = 0;
}

static int handle_next_chunk(FILE* file, png_image* png) {
	
	// get length
	int length;
	fread(&length, 4, 1, file);
	
	// read as the correct endianness
	length = invert_endianness(length);

	// get chunk type
	char chunkTypeStr[5];
	fread(chunkTypeStr, 1, 4, file);
	chunkTypeStr[4] = '\0';
	
	printf("Chunk type:%s\n", chunkTypeStr);
	printf("Length: %d\n", length);
	
	// for IHDR chunk
	if (strncmp(chunkTypeStr, "IHDR", 4) == 0) {
		if (length != 13){
			perror("File may be corrupted, wrong length IHDR chunk");
			return 0;
		}

		char crc[4];
		char data[13];
		// read data and crc
		fread(data, 1, 13, file);
		fread(crc, 1, 4, file);
		
		// verify crc
		unsigned char crc_buff[17];
		memcpy(crc_buff, chunkTypeStr, 4);
		memcpy(crc_buff+4, data, 13);
		
		if (verify_crc(crc_buff, crc, 17)) {
			perror("IHDR CRC not valid");
		}


		// store the data
		memcpy(&png->width, data, 4);
		png->width = invert_endianness(png->width);
		memcpy(&png->height, data+4, 4);
		png->height = invert_endianness(png->height);
		png->bit_depth = data[8];
		png->color_type = data[9];
		png->compression = data[10];
		png->filter = data[11];
		png->interlace = data[12];
	}

	// for PLTE chunk
	else if (strncmp(chunkTypeStr, "PLTE", 4) == 0) {

	}		
	// for IDAT chunk
	else if (strncmp(chunkTypeStr, "IDAT", 4) == 0) {
		if (png->data == NULL) {
			png->size = length;
			png->data = malloc(length*sizeof(char));	
			fread(png->data, 1, length, file);
		} else {
			png->data = realloc(png->data, (length + png->size)*sizeof(char));
			fread(png->data + png->size, 1, length, file);
			png->size += length;
		}
		char crc[4];
		fread(crc, 1, 4, file);
		

	}
	// for IEND chunk
	else if (strncmp(chunkTypeStr, "IEND", 4) == 0) {
		// just return
		return 0;
	}	
	else if (strncmp(chunkTypeStr, "\0\0\0\0", 4) == 0) {
		return 0;
	}	
	else {	
		char dump[length + 4];
		fread(dump, 1, length + 4, file);
		printf("unknown chunk: %s\n", chunkTypeStr);

	}
	
	return 1;
}

//----------------public functions---------------------------

// opens a png file, returns 0 if file is read correctly, 1 otherwise 
int open_png(char* address, png_image* png) {
	FILE* file = fopen(address, "rb");
	if (file == NULL) {

		char out[255];
		snprintf(out, 255, "file not found %s\n", address);
		perror(out);
		return 1;
	}
	
	// check header
	char header[9];
	fread(header, 1, 8, file);
	if (check_header(header)) {
		char out[255];
		snprintf(out, 255, "%s is not a png file\n", address);
		perror(out);
		fclose(file);
		return 1;
	}
	printf("opened png file: %s\n", address);
	

	// initiate data
	init_png(png);

	// handle all the chunks
	int resp = 1;
	int i = 0;
	while(resp == 1){
		resp = handle_next_chunk(file, png);
	}

	fclose(file);
	// print the information about the png
	printf("width: %d\n", png->width);
	printf("height: %d\n", png->height);
	printf("png->bit_depth: %d\n", png->bit_depth);
	printf("png->color_type: %d\n", png->color_type);
	printf("png->compression: %d\n", png->compression);
	printf("png->filter: %d\n", png->filter);
	printf("png->interlace: %d\n", png->interlace);
	return 0;
}

int save_png(char* address, png_image* png) {
	
}


// cleans a png file
void clean_png(png_image* png) {
	free(png->data);
	free(png);
}

