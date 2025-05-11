
struct {
	int width, height, size;
	char bit_depth, color_type, compression, filter, interlace;
	unsigned char* data;
} typedef png_image;

int open_png(char* address, png_image* png);
void clean_png(png_image* png);



