#include<stdio.h>
#include<stdlib.h>
#include<string.h>


void constFuncInput(int in, int& out);


void constFuncInput(const int in, int& out)
{
	//in = in +1; //error: cannot assign to variable 'in' with const-qualified type 'const int'
	if (in == 0) {
		out = 0;
	}
	else {
		out = 1;
	}
}

void setString(const char* src, char* des)
{
	src = (char*)"45555";
	//src[0] = 'z'; //error: read-only variable is not assignable
	if (src != NULL) {
		strcpy(des, src);
	}
}

const char* getString()
{
	char* src = new char[10];
	strcpy(src, (char*)"456");
	return src;
}


int main()
{
	int out = -1;;
	constFuncInput(2, out);

	char src[20]="123456";
	src[5] = 'A';
	char des[50];
	setString((char*)src, (char*)&des);
	printf("des =%s \n", des);

	char* test = (char*)"jslslsl0";
	//test[5]='A'; //Segmentation fault (core dumped)
	

	//char* res = getString();//error: cannot initialize a variable of type 'char *' with an rvalue of type 'const char *'
	const char* res = getString();
	//res[0] = 'A';//rror: read-only variable is not assignable
	printf("res = %s\n", res);
	return 0;
}

