#include<stdio.h>
#include<stdlib.h>

void funcValue(int a)
{
	a = 10;
	printf("a addr = %p, value=%d \n", &a, a);
}


void funcPointer(int *p)
{
	*p = 20; // change the value of pointer, not change pointer
	p = new int(2);
	printf("p addr = %p, value=%p, pointer to memory=%d \n", &p, p, *p);
}

void funcRef(int &r)
{
	r = 30;
	printf("r addr = %p, value=%d \n", &r, r);

}


int main()
{
	int a = 0;
	printf("a start addr = %p, value=%d \n", &a, a);
	funcValue(a);
	printf("a end addr = %p, value=%d \n", &a, a);


	int* p = &a;
	printf("p start addr = %p, value=%p, pointer to memory=%d \n", &p, p, *p);
	funcPointer(p);
	printf("p end addr = %p, value=%p, pointer to memory=%d \n", &p, p, *p);



	int r = 3;
	printf("r start addr = %p, value=%d \n", &r, a);
	funcRef(r);
	printf("r end addr = %p, value=%d \n", &r, a);
	return 0;
}
