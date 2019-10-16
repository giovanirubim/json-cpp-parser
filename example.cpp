#include "json.h"
#include <cstdio>

int main() {

	char str[10240];

	printf("Insert a JSON string\n> ");
	gets(str);

	json_parse_result res = json::parse(str);

	if (res.result()) {

		printf("Parsed: ");
		puts(res.result()->toString().c_str());

	} else {

		int e = res.error_index(); // Posição do erro
		int a = e - 15; // Posição incial a imprimir a string
		int b = e + 15; // Posição final a se imprimir da string
		if (a < 0) a = 0; // Posição incial não pode ser negativa
		int c = e - a; // Número de caracteres a serem printados
		int n = 0; // Número de caracteres printados

		if (a > 4) { // Se mais de 4 caracteres foram "pulados" imprime-se reticências
			printf("... ");
		} else {
			a = 0; // Se menos de 4 caracteres foram "pulados" imprime-se a string do começo
		}

		// Imprime a string de a até b - 1 ou até o caractere \0, contando quantos foram impressos
		for (int i=a; i<b && str[i]; ++i, ++n) {
			putchar(str[i]);
		}

		// Se ainda haviam mais caracteres imprime-se reticências
		if (str[a + n]) printf(" ...");

		// Quebra de linha para apontar o erro
		putchar('\n');

		// Imprime o número de espaços necessários para apontar o caractere que causou o erro
		for (int i = a>0? 4+c: c; i--; putchar(' '));

		printf("^ Syntax error at %d\n", e + 1);

	}

}