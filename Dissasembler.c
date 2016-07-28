#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

void dissasemble(uint8_t * buffer, int pc)
{
	uint8_t * code = &buffer[pc];
	uint8_t firstNib = (code[0] >> 4); //Right Shift
	uint8_t secondNib = (code[0] & 0x0F);
	//uint8_t n = code[0] & 0x0F; 
	//uint8_t n1 = code[1] & 0xFF;
	uint8_t secondByte = code[1];
	uint8_t finalNib = code[1] & 0x0F;
	uint8_t vy = (code[1] >> 4);			
	
	printf("%04x, %02x %02x\t",pc, code[0], code[1]); //[PC address] [Instruction]
	
	switch(firstNib)
	{
		case 0x00:
			//uint8_t finalbyte = code[1];
			if(secondNib != 0 && secondByte != 0xEE)
			{
				printf("SYS %01x%02x", secondNib, secondByte);
			}
			else if(secondByte != 0XEE && secondByte != 0x00)
			{
				printf("CLS");
			}
			else if(secondByte != 0x00)
			{
				printf("RET");
			}
			else
			{
				printf("NOP");
			}
			break;
		case 0x01:
			printf("JP %01x%02x", secondNib, secondByte);	
			break;
		case 0x02:
			printf("CALL %01x%02x", secondNib, secondByte);	
			break;
		case 0x03:
			printf("SE V%01x, %02x",secondNib,secondByte);
			break;
		case 0x04:
			printf("SNE V%01x, %02x",secondNib,secondByte);
			break;
		case 0x05:
			printf("SE V%01x, V%01x, %01x",secondNib,vy, finalNib);
			break;
		case 0x06:
			printf("LD V%01x, %02x",secondNib,secondByte);
			break;
		case 0x07:
			printf("ADD V%01x, %02X",secondNib,secondByte);
			break;
		case 0x08:
				switch(finalNib)
				{
					case 0x00:
						printf("LD V%01x, V%01x, %01x",secondNib,vy,finalNib);
						break;
					case 0x01:
						printf("OR V%01x, V%01x, %01x",secondNib,vy,finalNib);
						break;
					case 0x02:
						printf("AND V%01x, V%01x, %01x",secondNib,vy,finalNib);
						break;
					case 0x03:
						printf("XOR V%01x, V%01x, %01x",secondNib,vy,finalNib);
						break;
					case 0x04:
						printf("ADD V%01x, V%01x, %01x",secondNib,vy,finalNib); //Add with carry
						break;
					case 0x05:
						printf("SUB V%01x, V%01x, %01x",secondNib,vy,finalNib); 
						break;
					case 0x06:
						printf("SHR V%01x, V%01x, %01x",secondNib,vy,finalNib); 
						break;
					case 0x07:
						printf("SUBN V%01x, V%01x, %01x",secondNib,vy,finalNib); 
						break;
					case 0x0E:
						printf("SHL V%01x, V%01x, %01x",secondNib,vy,finalNib); 
						break;
					default:
						printf("Not implemented");
				}
			break;
		case 0x09:
			printf("SNE V%01x, V%01x, %01x",secondNib,vy,finalNib); 
			break;
		case 0x0A:
			printf("LD I, %01x%02x",secondNib,secondByte);
			break;
		case 0x0B:
			printf("JP V0, %01x%02x",secondNib,secondByte);
			break;
		case 0x0C:
			printf("RND V%01x, %02x",secondNib,secondByte);
			break;
		case 0x0D:
			printf("DRW V%01x, V%01x",secondNib,vy);
			break;
		case 0x0E:
			switch(secondByte)
			{
				case 0x9E:
					printf("SKP V%01x",secondNib);
					break;
				case 0xA1:
					printf("SKNP V%01x",secondNib);
					break;
				default:
					printf("Not implemented");
			}
			break;
		case 0x0F:
			switch(secondByte)
			{
				case 0x07:
					printf("LD, V%01x, DT",secondNib);
					break;
				case 0x0A:
					printf("LD V%01x, K",secondNib);
					break;
				case 0x15:
					printf("LD DT, V%01x",secondNib);
					break;
				case 0x18:
					printf("LD ST, V%01x",secondNib);
					break;
				case 0x1E:
					printf("ADD I, V%01x",secondNib);
					break;
				case 0x29:
					printf("LD F, V%01x",secondNib);
					break;
				case 0x33:
					printf("LD B, V%01x",secondNib);
					break;
				case 0x55:
					printf("LD [I], V%01x",secondNib);
					break;
				case 0x65:
					printf("LD V%01x, [I]",secondNib);
					break;
				default:
					printf("Not implemented");
					break;
			}
			break;
		default:
			printf("Not implemented");
			break;
	}
} 

int main(int argc, char ** argv)
{
	FILE * file;
	long pc;
	uint8_t * buffer;
	long size;
	
	if(argc < 2)
	{
		fprintf(stderr,"Error using the program: %s \"program\"\n",argv[0]);
		exit(1);
	}
	
	if(NULL == (file = fopen(argv[1], "rb")))
	{
		fprintf(stderr,"Error opening the file: %s\n",argv[1]);
		exit(1);
	}
	
	//Get size of the file in bytes
	if(-1 == (fseek(file, 0L, SEEK_END)))
	{
		perror("Error setting file position to end\n");
		exit(1);
	}
	size = ftell(file);
	if(-1 == (fseek(file, 0L, SEEK_SET)))
	{
		perror("Error setting file position to begining\n");
		exit(1);
	}
	
	//Initialize the buffer
	buffer = malloc(size+0x200);
	
	//Read the file and store it on the buffer
	if(0 == (fread(buffer+0x200, size, 1, file)))
	{
		if(ferror(file))
		{
			perror("Error reading the file\n");
			exit(1);
		}
		else
		{
			perror("End of file reached, cant read more\n");
			exit(1);
		}
	}
	
	// Set the program counter (PC), Chip-8 programs start at 0x200
	pc = 0x200;
	
	//Iterate and disassemble the program
	while(pc < size+0x200)
	{
		dissasemble(buffer,pc);
		pc += 2;
		printf("\n");
	}
	
	fclose(file);
	
	return 0;
}

