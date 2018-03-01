#include<stdio.h>

int main()
{
	int x;
	float f;
	double d;

	int res = 0;
	res = (x == (int) (double)x);
	printf("res = %d \n", res);
	res = (x == (int) (float)x);
	printf("res = %d \n", res);
	res = (f == (double) (float)d);
	printf("res = %d \n", res);
	res = (d == (float) (double)f);
	printf("res = %d \n", res);
	res = (f == -(-f));
	printf("res = %d \n", res);
	res = (1.0/2 == 1/2.0);
	printf("res = %d \n", res);
	res = (d*d >=0);
	printf("res = %d \n", res);
	res = ((f+d)-f == d);
	printf("res = %d \n", res);

	return 0;
}

