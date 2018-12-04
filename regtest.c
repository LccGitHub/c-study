/*************************************************************************
  > File Name: regtest.c
  > Author: ma6174
  > Mail: ma6174@163.com 
  > Created Time: Tue 04 Dec 2018 12:02:14 PM CST
 ************************************************************************/

#include<stdio.h>
#include <regex.h>



const char ScanResultsPattern[] = "(.{17})\\s+(\\w+)+(-\\w+)\\s+(.*)\t(.*)";



int main(void)
{
	char pstr[100] = "50:fa:84:01:6b:66	5745-59	[WPA-PSK-CCMP+TKIP][WPA2-PSK-CCMP+TKIP][WPS][ESS]	AP-D11F-06-5G";
	regex_t reg;
	int cflags = REG_EXTENDED;
	int res = regcomp(&reg, ScanResultsPattern, cflags);

	if (res) {

		printf("%s,%d, res = %d\n", __func__, __LINE__, res);
	}
	else {
		regmatch_t pmatch[100];
		const int nmatch = 100;
		res = regexec(&reg, pstr, (size_t)nmatch, pmatch, 0);
		printf("%s,%d, res = %d\n", __func__, __LINE__, res);
	}

	return 0;
}
