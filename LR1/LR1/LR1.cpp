#define _CRT_SECURE_NO_WARNINGS
#include <windows.h> 
#include <stdio.h> 
#include <string>

#define BUF_SIZE 256

using namespace std;

int main(int argc, LPTSTR argv[]) {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	HANDLE hIn, hOut;
	DWORD nIn, nOut, count = 0, i = 0;
	CHAR Buffer[BUF_SIZE];

	if (argc != 3) {
		printf("Использование: cpw, файл ввода, пара символов для замены\n");
		return  1;
	}

	hIn = CreateFile(argv[1], GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hIn == INVALID_HANDLE_VALUE) {
		printf("Невозможно открыть входной файл. Ошибка: %x\n", GetLastError());
		return  2;
	}
	char* p = strchr(argv[1], '.');
	if (p) * (p + 1) = '\0'; 
	strcat(argv[1], "log"); 
	
	hOut = CreateFile(argv[1], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOut == INVALID_HANDLE_VALUE) {
		printf("Невозможно открыть выходной файл. Ошибка: %x\n", GetLastError());
		return 3;
	}

	while (ReadFile(hIn, Buffer, BUF_SIZE, &nIn, NULL) && nIn > 0) {

		while (i < BUF_SIZE - 1)
		{
			if (Buffer[i] == argv[2][0] && Buffer[i + 1] == argv[2][1]) {
				Buffer[i] = '#';
				Buffer[i + 1] = '@';
				i++;
			}
			i++;
		}
		WriteFile(hOut, Buffer, nIn, &nOut, NULL);
		if (nIn != nOut) {
			printf("Неустранимая ошибка записи: %x\n", GetLastError());
			return 4;
		}
	}
	CloseHandle(hIn);
	CloseHandle(hOut);
	return 0;
}

