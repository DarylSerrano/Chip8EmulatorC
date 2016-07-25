#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

typedef struct testStruct{
	int x;
	int y;
}testS;

void pruebaFuncion(testS p){
	printf("este es x:%d y y: %d\n",p.x,p.y);
	p.x = 20;
	p.y = 30;
	printf("Ahora he cambiado el valor, este es x:%d y y: %d\n",p.x,p.y);
}

typedef struct Chip8State{
	uint16_t PC;
	uint16_t I; //Special memory address register
	uint8_t V[16];
	uint8_t SP;
	uint8_t DT;
	uint8_t ST;
	uint8_t * memory;
	uint8_t * screen;
	uint8_t keys[16];
} State;

typedef struct Instruction{
	uint8_t firstNib;
	uint8_t secondNib;
	uint8_t finalNib;
	uint16_t firstByte;
	uint16_t secondByte;
} Instruction; 

int main(int argc, char ** argv){
	/*
	uint8_t x = 0x02;
	uint8_t aux;
	aux = 0x0F & x;
	printf("%01x\n",aux);
	printf("%dbytes\n",sizeof(int));
	
	
	uint8_t code[2];
	code[0] = 0x01;
	code[1] = 0x23;
	uint8_t n = code[0] & 0x0F;
	uint8_t n1 = code[1] & 0xFF;
	printf("SYS \t%01x%02x\n", n, n1);
	
	
	uint8_t secondbyte = (code[0] & 0x0F);
	printf("%01x\n",secondbyte);
	if(secondbyte != 0){
		printf("Funciona los comparadores asi\n");
	}else{
		printf("No va\n");
	}
	*/
	
	/*
	testS * prueba = malloc(sizeof(testS));
	prueba->x = 10;
	prueba->y = 20;  
	pruebaFuncion(*prueba);
	printf("Valores despues de cambiar dentro de la funcion este es x:%d y y: %d\n",prueba->x,prueba->y);
	*/

	/*
	uint16_t dir;
	uint8_t mem[2];
	mem[0] = 0x01;
	mem[1] = 0xF1;
	dir = (0x0F & mem[0]) << 8;
	printf("dir con mem[0] desplazado 8 bits %04x\n",dir);
	dir = (dir | mem[1]);
	printf("%04x\n",dir);
	dir = 0x200;
	printf("%04x\n",dir);
	
	dir = (0x0F00 & (0x11 << 8)) | 0xFA;
	printf("%04x\n",dir);
	
	dir = 0xF1F2;
	mem[1] = (dir & 0xFF);;
	printf("%04x\n",mem[1]); 
	*/
	
	/*
	uint16_t x = 0x0000;
	x = x | (0xF << 8);
	printf("%04x\n",x);
	x = x | 0xFF;
	printf("%04x\n",x);
	*/
	
	/*
	uint16_t z1;
	uint8_t z;
	uint8_t x = 0xF0;
	uint8_t y = 0x11;
	if(0xFF < x + y){
		printf("Es mayor\n");
		z = x + y;
		z1 = x + y;
	}
	printf("%04x\n",z);
	printf("%04x\n",z1);
	*/
	
	uint8_t x = 0xF2;
	if(0x01 & x)
	{
		printf("Si\n");
	}
	
	return 0;
} 
