#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
// START DATA SECTION
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

uint16_t currVariable = 0;

typedef struct variable{
	uint32_t id;
	int32_t value;
} colourVariable_t;

enum RESERVED_COLOURS{
	INC=0x00a249a4,
	CBRACKET=0,
	COMMA=0x00c3c3c3,
	PRINTC=0x00b7e61d,
	PRINTI=0x007cd6d6,
	DEC=0x004a90e2,
	MUL=0x00ec277c,
	DIV=0x00fffa4f,
	JMP=0x006e120e,
	IFE=0x0000ff12,
	IFL=0x009aa6ad,
	IFG=0x00397463,
	RAN=0x00194d33,
	USRI=0x00ad456e,
	USRC=0x00887d3e
};

char ERRORCODES[][255] = {"Reserved Colour Found Outside Of Function Call","Function Expects 1 Or 2 Arguments","Function Arguments Takes Up More Than 1 Pixel","Too Many Arguments","Variable Referenced As Argument Not Found","Function Expects 1 Argument","Function Expects 1 Or 0 Arguments","Jumped Out Of Bounds","If Functions Expect 4 Arguments","Function Expects 3 Arguments"};

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
#define _isReserved(rgb) ((rgb == INC || rgb == CBRACKET || rgb == COMMA || rgb == PRINTC || rgb == PRINTI || rgb == DEC || rgb == MUL || rgb == DIV || rgb == JMP || rgb == IFE || rgb == IFL || rgb == IFG || rgb == RAN || rgb == USRI || rgb == USRC) ? 1 : 0)
#define addVariable(rgb) (variables[currVariable++] = (colourVariable_t){rgb,1})
#define getPixel(data,pos) (data[pos*3+2]<<16)+(data[pos*3+1]<<8)+(data[pos*3])

char alreadyVariable(uint32_t id, colourVariable_t vars[]){
	for(int i = 0; i<currVariable; i++){
		if(vars[i].id == id)
			return i;
	}
	return -1;
}

int getRandNum(int start, int end){
	int out = rand();
	return (out % (end-start) + start);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// END BASIC TOOLS

// START FUNCTION DECLERATIONS
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int iterateThrough(uint8_t data[], int length);
int getAmountOfVariables(uint8_t data[], int length);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// END FUNCTION DECLERATIONS

int main(int argc, char **argv){
	srand((int)time(0));
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

int32_t getVariableValue(colourVariable_t variables[], uint32_t colour){
	for(int i = 0; i < currVariable; i++){
		if(variables[i].id == colour)
			return variables[i].value;
	}
	return 0;
}

void setVariableValue(colourVariable_t *(variables[]), uint32_t colour, int32_t val){
        for(int i = 0; i < currVariable; i++){
                if(variables[i]->id == colour)
                        variables[i]->value = val;
        }
}

int _in(colourVariable_t variables[], uint32_t colour){
	for(int i = 0; i < currVariable; i++){
                if(variables[i].id==colour)
                        return 1;
        }
        return 0;
}

int iterateThrough(uint8_t data[],int length){
	colourVariable_t variables[getAmountOfVariables(data, length)]; // getAmount should keep track of all the variables and increment, or just estimate idgaf
	int curr = 0;
	char c = 0;
	while(curr < length-1 && getPixel(data,curr)!=0xffffff){
		uint32_t currPixel = getPixel(data,curr);
		switch(currPixel){
			case CBRACKET:{
				curr++;
				uint32_t args[4];
				int argsCount = 0;
				for(;!_isReserved(getPixel(data,curr));curr+=2){
					if(argsCount+1 > 4)
						return 3;
					args[argsCount++] = getPixel(data,curr);
				}
				int funcLength = 1; // <-- for functions that use the length of the function as an argument
				uint32_t function = getPixel(data,curr);
				if(curr+1 < length-1){
					for(++curr;getPixel(data,curr) == function;++curr){
						funcLength++;
					}
				}
				switch(function){
					case USRC:{
						if(argsCount == 1){
							if(!_in(variables,args[0]))
								return 4;
							for(int i = 0; i < currVariable; i++){
							        if(variables[i].id == args[0]){
									scanf("%lc",&(variables[i].value));
									while ( (c = getchar()) != '\n' && c != EOF);
								}
							}
						} else 
							return 5;
						break;
					}
					case USRI:{
						if(argsCount == 1){
							if(!_in(variables,args[0]))
								return 4;
							for(int i = 0; i < currVariable; i++){
							        if(variables[i].id == args[0]){
									scanf("%d",&(variables[i].value));
									while ( (c = getchar()) != '\n' && c != EOF);
								}
							}
						} else 
							return 5;
						break;
					}
					case RAN:{
						if(argsCount == 3){
							if(!_in(variables,args[0]) || !_in(variables,args[1]) || !_in(variables,args[2]))
								return 4;
							if(getVariableValue(variables,args[1])<getVariableValue(variables,args[2])){
								for(int i = 0; i < currVariable; i++){
							        	if(variables[i].id == args[0])
						        		        variables[i].value = getRandNum(getVariableValue(variables,args[1]),getVariableValue(variables,args[2]));
								}
							} else {
								for(int i = 0; i < currVariable; i++){
							        	if(variables[i].id == args[0])
						        		        variables[i].value = getRandNum(getVariableValue(variables,args[2]),getVariableValue(variables,args[1]));
								}
							}
						} else
							return 9;
						break;
					}
					case IFL:{
                                                if(argsCount == 4){
                                                        if(!_in(variables,args[0]) || !_in(variables,args[1]) || !_in(variables,args[2]) || !_in(variables,args[3]))
                                                                return 4;
                                                        if(getVariableValue(variables,args[0]) < getVariableValue(variables,args[1])){
                                                                if(-1 < (getVariableValue(variables,args[2])-1) && length > (getVariableValue(variables,args[2])-1))
                                                                        curr = (getVariableValue(variables,args[2])-2);
                                                                else
                                                                        return 7;
                                                        } else {
                                                                if(-1 < (getVariableValue(variables,args[3])-1) && length > (getVariableValue(variables,args[3])-1))
                                                                        curr = (getVariableValue(variables,args[3])-2);
                                                                else
                                                                        return 7;
                                                        }
                                                } else
                                                        return 8;
                                                break;
                                        }
                                        case IFG:{
                                                if(argsCount == 4){
                                                        if(!_in(variables,args[0]) || !_in(variables,args[1]) || !_in(variables,args[2]) || !_in(variables,args[3]))
                                                                return 4;
                                                        if(getVariableValue(variables,args[0]) > getVariableValue(variables,args[1])){
                                                                if(-1 < (getVariableValue(variables,args[2])-1) && length > (getVariableValue(variables,args[2])-1))
                                                                        curr = (getVariableValue(variables,args[2])-2);
                                                                else
                                                                        return 7;
                                                        } else {
                                                                if(-1 < (getVariableValue(variables,args[3])-1) && length > (getVariableValue(variables,args[3])-1))
                                                                        curr = (getVariableValue(variables,args[3])-2);
                                                                else
                                                                        return 7;
                                                        }
                                                } else
                                                        return 8;
                                                break;
                                        }
					case IFE:{
						if(argsCount == 4){
							if(!_in(variables,args[0]) || !_in(variables,args[1]) || !_in(variables,args[2]) || !_in(variables,args[3]))
								return 4;
							if(getVariableValue(variables,args[0]) == getVariableValue(variables,args[1])){
								if(-1 < (getVariableValue(variables,args[2])-1) && length > (getVariableValue(variables,args[2])-1))
								        curr = (getVariableValue(variables,args[2])-2);
								else
								        return 7;
							} else {
                                                                if(-1 < (getVariableValue(variables,args[3])-1) && length > (getVariableValue(variables,args[3])-1))
                                                                        curr = (getVariableValue(variables,args[3])-2);
                                                                else
                                                                        return 7;
							}
						} else
							return 8;
						break;
					}
					case JMP:{
						if(argsCount == 1){
							if(!_in(variables,args[0]))
							        return 4;
							for(int i = 0; i < currVariable; i++){
                                                        	if(variables[i].id == args[0])
									if(-1 < (getVariableValue(variables,args[0])-1) && length > (getVariableValue(variables,args[0])-1))
										curr = (getVariableValue(variables,args[0])-2); // why is there 2 you may ask and not 1, well...
									else							// later in this switch there is a curr++ to advance
										return 7;					// to the next pixel, with counteracts a jump
	                                                }
						} else if(argsCount == 0){
							if(-1 < funcLength-1 && length > funcLength-1)
								curr = funcLength-2;
                                                        else
                                                                return 7;
						} else {
							return 6;
						}
						break;
					}
					case PRINTI:{
                                                if(argsCount != 1)
                                                        return 5;
                                                if(!_in(variables,args[0]))
                                                        return 4;
                                                for(int i = 0; i < currVariable; i++){
                                                        if(variables[i].id == args[0])
                                                                printf("%d",getVariableValue(variables,args[0]));
                                                }
						break;
					}
					case PRINTC:{
						if(argsCount != 1)
						        return 5;
						if(!_in(variables,args[0]))
						        return 4;
                                                for(int i = 0; i < currVariable; i++){
                                                        if(variables[i].id == args[0])
								printf("%c",(char)getVariableValue(variables,args[0]));
                                                }
						break;
					}
					case INC:{
						if(argsCount == 1){
							if(!_in(variables,args[0]))
								return 4;
							for(int i = 0; i < currVariable; i++){
							        if(variables[i].id == args[0])
						        	        variables[i].value = getVariableValue(variables,args[0])+funcLength;
							}
						} else if(argsCount == 2){
							if(!_in(variables,args[0]) || !_in(variables,args[1]))
							        return 4;
							for(int i = 0; i < currVariable; i++){
							        if(variables[i].id == args[0])
						        	        variables[i].value = getVariableValue(variables,args[0])+getVariableValue(variables,args[1]);
							}
						} else {
							return 1;
						}
						break;
					}
					case DEC:{
						if(argsCount == 1){
							if(!_in(variables,args[0]))
								return 4;
							for(int i = 0; i < currVariable; i++){
							        if(variables[i].id == args[0])
						        	        variables[i].value = getVariableValue(variables,args[0])-funcLength;
							}
						} else if(argsCount == 2){
							if(!_in(variables,args[0]) || !_in(variables,args[1]))
							        return 4;
							for(int i = 0; i < currVariable; i++){
							        if(variables[i].id == args[0])
						        	        variables[i].value = getVariableValue(variables,args[0])-getVariableValue(variables,args[1]);
							}
						} else {
							return 1;
						}
						break;
					}
					case MUL:{
					        if(argsCount == 1){
					                if(!_in(variables,args[0]))
					                        return 4;
					                for(int i = 0; i < currVariable; i++){
					                        if(variables[i].id == args[0])
					                                variables[i].value = getVariableValue(variables,args[0])*funcLength;
					                }
					        } else if(argsCount == 2){
					                if(!_in(variables,args[0]) || !_in(variables,args[1]))
					                        return 4;
					                for(int i = 0; i < currVariable; i++){
					                        if(variables[i].id == args[0])
					                                variables[i].value = getVariableValue(variables,args[0])*getVariableValue(variables,args[1]);
					                }
					        } else {
					                return 1;
					        }
					        break;
					}
                                        case DIV:{
                                                if(argsCount == 1){
                                                        if(!_in(variables,args[0]))
                                                                return 4;
                                                        for(int i = 0; i < currVariable; i++){
                                                                if(variables[i].id == args[0])
                                                                        variables[i].value = getVariableValue(variables,args[0])/funcLength;
                                                        }
                                                } else if(argsCount == 2){
                                                        if(!_in(variables,args[0]) || !_in(variables,args[1]))
                                                                return 4;
                                                        for(int i = 0; i < currVariable; i++){
                                                                if(variables[i].id == args[0])
                                                                        variables[i].value = getVariableValue(variables,args[0])/getVariableValue(variables,args[1]);
                                                        }
                                                } else {
                                                        return 1;
                                                }
                                                break;
                                        }
					case COMMA:
						return 2; // surely a variable is longer than 1 pixel so end the program
						break;
				}
				curr++;
				break;
			}
			default:{
				if(_isReserved(currPixel)){ // a function is used outside brackets
					return 0;
				}
				int exists = alreadyVariable(currPixel, variables);
				if(exists == -1){ // variable doesn't exist
					addVariable(currPixel);
					for(++curr;getPixel(data,curr) == currPixel;++curr){
						variables[currVariable-1].value++;
					}
				} else {
					variables[exists].value = 1;
					for(++curr;getPixel(data,curr) == currPixel;++curr){
						variables[exists].value++;
					}
				}
				break;
			}
		}
	}
	return -1;
}

int __in(uint32_t data[], int length, uint32_t colour){
	for(int i = 0; i < length; i++){
		if(data[i]==colour)
			return 1;
	}
	return 0;
}

int getAmountOfVariables(uint8_t data[], int length){
	uint32_t colours[length]; // could use a linked list to dynamically expand the size in order not to spam memeory ¯\_(ツ)_/¯
	int count = 0;
	for(int i = 0; i < length; i++){
		if(!_isReserved(getPixel(data,i))){
			if(!__in(colours, count, data[i]))
				colours[count++] = data[i];
		}
	}
	return count;
}
