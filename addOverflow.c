#include<stdio.h>
#include<math.h>
#include<stdint.h>

#define MAX_INT8 127
#define MIN_INT8 -128

int add_ok1(int8_t x, int8_t y)
{
	int8_t result = x + y;

	if ((x > 0) && (y > 0) && (result < 0)) {
		result = -1;
	}
	else if ((x < 0) && (y < 0) && (result > 0)) {
		result = -1;
	}
	else {
		result = 0;
	}

	return result;
}

int add_ok2(int8_t x, int8_t y)
{
	int8_t result = x + y;

	if ((x > 0) && (y > 0) && (x > MAX_INT8 - y)) {
		result = -1;
	}
	else if ((x < 0) && (y < 0) && (x < MIN_INT8 -y)) {
		result = -1;
	}
	else {
		result = 0;
	}

	return result;
}


/* div power(2.k):num/16
 * x< 0? (x+ (1<<k)-1 : x)>>k
 * */
int div16(int x)
{
	/* compute bias to be either 0(x>0) or 15(x < 0)*/
	int bias = (x>>31)&0xf;

	return (x+ (1<<4) -1)>>4;
}

int main()
{
	printf("max  = %d \n", MAX_INT8);
	printf("min  = %d \n", MIN_INT8);
	printf("res = %d \n", add_ok1(127,1));
	printf("res = %d \n", add_ok2(127,1));
	printf("div16 = %d \n", div16(-17));
	printf("float 1= %f, 0x%x \n", 1.0, 1.0);
}

