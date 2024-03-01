#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)
#define BUF_SIZE 256
typedef int (*Func)(char*, char*);

int main() {
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	BOOL fReturnCode;
	DWORD cbMessages, cbMsgNumber, cbRead, cbWritten;
	HANDLE hMailslot1, hMailslot2 = nullptr, hIn = nullptr, hOut = nullptr;
	LPSTR lpszReadMailslotName = (LPSTR)"\\\\.\\mailslot\\$5api$";
	LPSTR lpszWriteMailslotName = (LPSTR)"\\\\*\\mailslot\\$5api_1$";
	char szBuf[512], message[80] = { 0 }, filename[BUF_SIZE];
	CHAR Buffer[BUF_SIZE];
	printf("Демонстрация почтового ящика сервера\n");
	hMailslot1 = CreateMailslot(lpszReadMailslotName, 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (hMailslot1 == INVALID_HANDLE_VALUE) {
		fprintf(stdout, "Ошибка создания почтового ящика: %ld\n", GetLastError());
		return 0;
	}
	fprintf(stdout, "Почтовый ящик создан\n");
	while (1) {
		fReturnCode = GetMailslotInfo(hMailslot1, NULL, &cbMessages, &cbMsgNumber, NULL);
		if (!fReturnCode) {
			fprintf(stdout, "GetMailslotInfo: Ошибка %ld\n", GetLastError()); _getch();
			break;
		}
		if (cbMsgNumber != 0) {
			if (ReadFile(hMailslot1, szBuf, 512, &cbRead, NULL)) {
				printf("Полученный: <%s>\n", szBuf);
				if (!strcmp(szBuf, "exit")) break;
				else {
					hMailslot2 = CreateFile(lpszWriteMailslotName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
					if (hMailslot2 == INVALID_HANDLE_VALUE) {
						fprintf(stdout, "CreateFile для пересылки: Ошибка %ld\n", GetLastError()); _getch();
						break;
					}
					char symbols[3];
					int i = 0;
					while (szBuf[i] != ' ' && szBuf[i] != '\0')
					{
						filename[i] = szBuf[i];
						i++;
					}
					filename[i] = '\0';
					if (szBuf[i] == ' ')
						i++;
					int j = 0;
					while (szBuf[i] != '\0')
					{
						symbols[j] = szBuf[i];
						i++;
						j++;
					}
					symbols[j] = '\0';
					if (symbols[1] == '\0')
					{
						symbols[1] = 'b';
						if (symbols[0] == '\0')
							symbols[0] = 'a';
						symbols[2] = '\0';
					}
					
					hIn = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
					if (hIn == INVALID_HANDLE_VALUE) {
						sprintf(message, "\nНевозможно открыть входной файл. Ошибка: %x\n", GetLastError());
						WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
						WriteFile(hMailslot2, message, strlen(message) + 1, &cbWritten, NULL);
					}
					i = 0;
					while (filename[i] != '.' && filename[i] != '\0') i++;
					filename[i] = '\0';
					strcat(filename, ".docx");
					hOut = CreateFile(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hOut == INVALID_HANDLE_VALUE) {
						sprintf(message, "\nНевозможно открыть выходной файл. Ошибка: %x\n", GetLastError());
						WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
						WriteFile(hMailslot2, message, strlen(message) + 1, &cbWritten, NULL);
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
					MyFunc(Buffer, symbols);
					while (ReadFile(hIn, Buffer, BUF_SIZE, &cbRead, NULL) && cbRead > 0) {
						Buffer[cbRead] = '\0';
						MyFunc(Buffer, symbols);
						WriteFile(hOut, Buffer, cbRead, &cbWritten, NULL);
						sprintf(message, "(Сервер): Файл %s успешно обработан", szBuf);
						WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
						sprintf(message, "Обработка выполнена, str: %s", Buffer);
						WriteFile(hMailslot2, message, strlen(message) + 1, &cbWritten, NULL);
						printf("Байтов отправлено %d\n", cbWritten);
						CloseHandle(hIn);
						CloseHandle(hOut);
					}
				}
			}
			else {
				fprintf(stdout, "ReadFile: Ошибка %ld\n", GetLastError()); getch(); break;
			}
		}
		Sleep(500);
	}
	CloseHandle(hMailslot1); CloseHandle(hMailslot2);
	return 0;
}
