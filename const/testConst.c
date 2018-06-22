#include<stdio.h>
#include<stdlib.h>


int main()
{
	/*****************************const T ***************************/
	int i = 0;
	const int constInt = 10;
	const int constInt2 = i;
	//constInt = 20; // this is error, const is not changed
	
	const int constInt3;
	//constInt3 = 1;
	/****************************************************************/

	/**************************const T* ******************************/
	const int *constIntP = &i;
	//*constIntP = 20; // this is error
	int j = 2;
	constIntP = &j; 

	int* const constIntP1 = &i;
	*constIntP1 = 20;
	//constIntP1 = &j; //error: cannot assign to variable 'constIntP1' with const-qualified type 'int *const'
	/*******************************************************************/

	/***************************const T& *******************************/
	int k = 0;
	const int constInti = 10;
	//const int& rConstInt = 20;

	/*******************************************************************/



	return 0;


}
