/* main.cpp */

#include "headers/compiler.h"

int main()
{
	Compiler compiler;
	compiler.setScannerInputFile("samples/test4.c");
	compiler.setParserOutputFile("results/intermediate4.txt");
	compiler.setGeneratorOutputFile("results/asm4.s");
	if (compiler.compile()) {
		system("gcc results/asm4.s -o results/test4.exe");
	}
	system("pause");
	return 0;
}
