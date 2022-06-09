#include <stdio.h>
#include <stdint.h>

// START DATA SECTION
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

uint16_t currVariable = 0;

typedef struct variable{
	uint32_t id;
	uint16_t value;
} colourVariable_t;

enum RESERVED_COLOURS{
	INC=0x00a249a4,
	CBRACKET=0,
	COMMA=0x00c3c3c3,
	PRINTC=0x00b7e61d
};

char ERRORCODES[][255] = {"Reserved Colour Found Outside Of Function Call"};

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// END DATA SECTION

// START BASIC TOOLS
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define _WARN(warning) printf("\e[0;33mWarning: %s\e[0m\n",warning);
#define _ERROR(error,exit) printf("\e[0;31mError: %s\e[0m\n",error); if(exit) return 1
#define _READTOVAR(var,where,size,file) fseek(file,where,SEEK_SET); fread(&var,size,1,file)
#define _READTOARRAY(var,where,size,length,file) fseek(file,where,SEEK_SET); fread(&var,size,length,file)
#define _GETRED(rgb) rgb>>16
#define _GETGREEN(rgb) (rgb&65280)>>8
#define _GETBLUE(rgb) (rgb&255)
#define _isReserved(rgb) ((rgb == INC || rgb == CBRACKET || rgb == COMMA || rgb == PRINTC) ? 1 : 0)
#define addVariable(rgb) (variables[currVariable++] = (colourVariable_t){rgb,1})

char alreadyVariable(uint32_t id, colourVariable_t vars[]){
	for(int i = 0; i<currVariable; i++){
		if(vars[i].id == id)
			return i;
	}
	return -1;
}

uint32_t getPixel(uint8_t *data, int position){
	uint32_t ret = 0;
	ret += data[position*3+2];
	ret <<= 8;
	ret += data[position*3+1];
	ret <<= 8;
	ret += data[position*3+0];
	return ret;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// END BASIC TOOLS

// START FUNCTION DECLERATIONS
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int iterateThrough(uint8_t data[], int length);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// END FUNCTION DECLERATIONS

int main(int argc, char **argv){
	if(argc < 2){
		_ERROR("Missing Argument", 1);
	}
	FILE *fp = fopen(argv[1],"rb");
	if(!fp){
		_ERROR("Failed to open file", 1);
	}
	uint32_t sX = 0,sY = 0,off = 0;
	_READTOVAR(sX,18,3,fp);
	_READTOVAR(sY,22,3,fp);
	_READTOVAR(off,10,3,fp);
	if(sY > 1){
		_WARN("Image contains y component\n\tthis will be ignored and ONLY the first row will be read");
	}
	uint32_t rowSize = ((4-((3*sX)%4))%4)+sX*3;
	uint8_t data[sX*3];
	_READTOARRAY(data,off+(rowSize*(sY-1)),1,sX*3,fp);
	int result = iterateThrough(data,sX);
	fclose(fp);
	if(result!=-1){ // There was a zero and an errorcode was returned from _ERRORCODES
		_ERROR(ERRORCODES[result],0);
	}
}

// START FUNCTION INITIALISATIONS
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int iterateThrough(uint8_t data[],int length){
	colourVariable_t variables[getAmountOfVariables(data)]; // getAmount should keep track of all the variables and increment, or just estimate idgaf
	int curr = 0;
	while(curr < length){
		uint32_t currPixel = getPixel(data,curr);
		switch(currPixel){
			default:{
				int exists = alreadyVariable(currPixel, variables);
				if(exists == -1){ // variable doesn't exist
					addVariable(currPixel);
					while(getPixel(data,curr++) == currPixel){
						variables[currVariable].value++;
					}
				} else {
					// redefine, so iterate until a different colour is reached
				}
			}
		}
	}
}
