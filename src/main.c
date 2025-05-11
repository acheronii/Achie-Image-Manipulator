#include <stdio.h>
#include <stdlib.h>

#include "png_utils.h"

int main(int argc, char* argv[]){
	
	if (argc != 3) {
		printf("Wrong inputs. Expects \"aim input.png output.png\"\n");
		return 1;
	}
	
	png_image* png = malloc(sizeof(png_image));
	if (open_png(argv[1], png)) {
		clean_png(png);
		fflush(stdout);
		return 1;
	}
	clean_png(png);

	fflush(stdout);
	return 0;
}
