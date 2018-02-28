#include<stdio.h>
#include<stdint.h>

/*this is buggy code, when lenght =0, will implicit conversion signed to unsigned*/
float sum_elements(float a[], unsigned int lenght)
{
	int i;
	float result = 0;

	for (i = 0; i <= lenght - 1; i++) {
		result += a[i];
	}
	return result;
}

/*this is buggy code ,when strlen(s)=0, will implicit conversion signed to unsigned*/
int strlonger(char *s, char *t)
{
	return strlen(s)-strlen(t) > 0;
}


int func1(unsigned int word)
{
	return (int)((word << 24) >> 24);
}

int func2(unsigned int word)
{
	return (((int)word << 24) >> 24);
}
int main()
{
	int8_t num = -5;
	uint8_t unum = num;

	printf(" %d , %u, 0x%02hhx, 0x%02hhx\n", num, (uint8_t)num,  num, (uint8_t)num);
	printf(" %d , %u, 0x%04hhx, 0x%04hhx\n", num, (uint16_t)num,  num, (uint16_t)num);
	
	unum = 255;
	printf(" %d , %u, 0x%02hhx, 0x%02hhx\n", (int8_t)unum, unum, (int8_t)unum, unum);
	printf(" %d , %u, 0x%04hhx, 0x%04hhx\n", (int16_t)unum, unum, (int16_t)unum, unum);
	
	unum = 10;
	printf(" %d , %u, 0x%02hhx, 0x%02hhx\n", (int8_t)unum, unum, (int8_t)unum, unum);


	printf("fun1=%d, func2=%d \n", func1(0x00000076), func2(0x00000076));
	printf("fun1=%d, func2=%d \n", func1(0x87654321), func2(0x87654321));
	printf("fun1=%d, func2=%d \n", func1(0x000000c9), func2(0x000000c9));
	printf("fun1=%d, func2=%d \n", func1(0xEDCBA987), func2(0xEDCBA987));


	printf("53191 = 0x%x , %d\n", 53191, (int16_t)53191);

	float a[10] = {1.0};
//	sum_elements(a, 0);

	char s[] = "";
	char t[10] = "hhdhd";
	printf("result = %d \n", strlonger(s, t));
}

