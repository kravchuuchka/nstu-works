#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string>

#define BUF_SIZE 256
typedef int (*Func)(char*, char*);
using namespace std;

int main(int argc, LPTSTR argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	HANDLE hIn, hOut;
	DWORD nIn, nOut, count = 0, i = 1;
	CHAR Buffer[BUF_SIZE];

	if (argc != 3) {
		printf("Использование: cpw, файл ввода, пара символов\n");
		return  1;
	}

	if (strlen(argv[2]) != 2) {
		printf("Введено некорректное число символов\n");
		return 5;
	}

	hIn = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Невозможно открыть входной файл. Ошибка: %x\n", GetLastError());
		return  2;
	}

	char* p = strchr(argv[1], '.');
	if (p) * (p + 1) = '\0';
	strcat(argv[1], "log");
	hOut = CreateFile(argv[1], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, NULL);

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
	FreeLibrary(hMyDll);
	CloseHandle(hIn);
	CloseHandle(hOut);
	return 0;
}

