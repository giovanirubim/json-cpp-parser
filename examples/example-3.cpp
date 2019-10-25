#include "json.h"
#include <cstdio>

int main() {

	char str[10240];

	printf("Insert a string\n> ");
	gets(str);
	str[0] = 0xbf;

	std::string temp = json::stringify(std::string(str));
	printf("[%s]\n", temp.c_str());

}