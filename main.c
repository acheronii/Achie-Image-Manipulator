#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]){
	for(int i = 0; i < argc - 1; i++){
		printf("%s\n", argv[i + 1]); 
	}
	return 1;
}

