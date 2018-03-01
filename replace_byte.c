#include<stdio.h>

unsigned long put_byte(unsigned long x, unsigned char b, int i)
{
	unsigned long result;
	result = (x & ~(0xFF<<(i<<3))) | (b << (i <<3));
	return result;
}

int main()
{
	printf("result =0x%x \n", put_byte(0x12345678, 0xAB, 2));
	printf("result =0x%x \n", put_byte(0x12345678, 0xAB, 0));
	return 0;
}

