#include<stdio.h>

int is_little_endian()
{
	int result = 0;
	int x = 0x1234;
	if (*((unsigned char*) &x) == 0x12) {
		result = 1;
	}
	else {
		result = 0;
	}
	int ressult = 0;
}

int main()
{
	if (is_little_endian() == 1) {
		printf("this machine is little endian \n");
	}
	else {
		printf("this machine is little endian \n");
	}
	return 0;
}

