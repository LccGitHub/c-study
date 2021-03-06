#include<stdio.h>

typedef unsigned char *byte_pointer;

void show_bytes(byte_pointer start, int len)
{
	int i;
	for (i = 0; i < len; i++)
	{
		printf(" %.2x", start[i]);
	}
	printf("\n");
}

void show_int(int x)
{
	show_bytes((byte_pointer) &x, sizeof(x));
}

void show_short(short x)
{
	show_bytes((byte_pointer) &x, sizeof(x));
}

void show_long(long x)
{
	show_bytes((byte_pointer) &x, sizeof(x));
}

void show_float(float x)
{
	show_bytes((byte_pointer) &x, sizeof(float));
}

void show_double(float x)
{
	show_bytes((byte_pointer) &x, sizeof(double));
}

void show_pointer(void *x)
{
	show_bytes((byte_pointer) &x, sizeof(void *));
}

int main()
{
	int x = 1;
	float y = 2.0;
	void *z = &x;
	show_short(x);
	show_int(x);
	show_long(x);
	show_float(y);
	show_double(y);
	show_pointer(z);

	return 0;
}

