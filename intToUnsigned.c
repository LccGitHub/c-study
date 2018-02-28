#include<stdio.h>
#include<stdint.h>

int main()
{
	int8_t num = -5;
	uint8_t unum = num;

	printf(" %d , %u, 0x%02hhx, 0x%02hhx\n", num, (uint8_t)num,  num, (uint8_t)num);
	
	unum = 255;
	printf(" %d , %u, 0x%02hhx, 0x%02hhx\n", (int8_t)unum, unum, (int8_t)unum, unum);
	
	unum = 10;
	printf(" %d , %u, 0x%02hhx, 0x%02hhx\n", (int8_t)unum, unum, (int8_t)unum, unum);

}

