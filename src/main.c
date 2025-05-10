#include <stdio.h>
#include <stdlib.h>

#include "png_utils.h"


// 
int main(int argc, char* argv[]){
	
	if (argc != 3) {
		printf("Wrong imputs. Expects \"aim input.png output.png\"\n");
		return 1;
	}
	
	png_image* png = malloc(sizeof(png_image));
	open_png(argv[1], png);
	
	clean_png(png);

	return 0;
}

