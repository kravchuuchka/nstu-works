#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string>
#define BUF_SIZE 256

#pragma argsused
typedef int (*Func)(char*, char*);
using namespace std;
int main(int argc, char* argv[])
{
	HANDLE hIn, hOut;
	char* line;
	DWORD nIn, nOut;
	CHAR Buffer[BUF_SIZE];
	int i = 0; char* filename = argv[1];

	if (argc < 3) {
		printf("No file to process!\n");
		exit(-1);
	}

	line = (char*)GetCommandLine();

	hIn = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Невозможно открыть входной файл. Ошибка: %x\n", GetLastError());
		return  2;
	}

	i = 0;
	while (filename[i] != '.' && filename[i] != '\0') i++;
	filename[i] = '\0';
	strcat(filename, ".log");

	hOut = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hOut == INVALID_HANDLE_VALUE) {
		printf("Невозможно открыть выходной файл. Ошибка: %x\n", GetLastError());
		return 3;
	}

	HINSTANCE hMyDll;
	hMyDll = LoadLibrary((LPCSTR)"LR2.dll");
	if (hMyDll == NULL) {
		printf("Ошибка загрузки библиотеки");
		exit(-1);
	}
	Func MyFunc;
	MyFunc = (Func)GetProcAddress(hMyDll, "MyFunction");
	if (MyFunc == NULL) {
		printf("функция MyFunc не найдена");
		exit(-2);
	}

	while (ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {
		MyFunc(Buffer, argv[2]);

		WriteFile(hOut, Buffer, nIn, &nOut, NULL);
		if (nIn != nOut) {
			printf("Неустранимая ошибка записи: %x\n", GetLastError());
			return 4;
		}
	}
	printf("(ProcesslD: %lu), File %s , %s\n", GetCurrentProcessId(), argv[1], argv[2]);
	FreeLibrary(hMyDll);
	CloseHandle(hIn);
	CloseHandle(hOut);
	return 0;
}
