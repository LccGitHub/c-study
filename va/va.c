/*************************************************************************
	> File Name: va.c
	> Created Time: Tue 20 Nov 2018 02:03:29 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdarg.h> /*　需要包含此头文件*/

int getSum(int num, ...) 
{
	va_list ap; /* 定义参数列表*/
	int sum = 0;
	int loop = 0;
	va_start(ap, num);　/* 初始化参数列表*/

	for (; loop < num; loop++) {
		sum += va_arg(ap, int);　/* 访问参数值*/
	}
	va_end(ap);　/* 清理参数列表*/
	return sum;
}

int main(void)
{
	int sum = 0;
	sum = getSum(4,1,2,3,4);
	printf("%s,%d, sum = %d\n", __func__, __LINE__, sum);
	return 0;
}


