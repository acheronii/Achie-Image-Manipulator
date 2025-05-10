
struct {
	unsigned char* data;
	int width, height, channels;
} typedef png_image;


int open_png(char* address, png_image* png);
void clean_png(png_image* png);



