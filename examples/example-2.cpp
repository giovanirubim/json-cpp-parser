#include "json.h"
#include <cstdio>

int main() {

	char str[10240];

	char* ptr = str - 1;
	while (((*++ptr)=getchar()) != EOF);
	*ptr = '\0';

	json_parse_result res = json::parse(str);

	if (res.result()) {

		printf("Parsed: ");
		puts(res.result()->toString().c_str());

	} else {

		int row, col;
		res.count_row_col(row, col);
		printf("Syntax error at line %d and column %d\n", row, col);
	
	}

}