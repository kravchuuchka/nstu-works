#include <windows.h>
#include <stdio.h>
#include <conio.h>

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

DWORD main(int argc, char* argv[])
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	HANDLE hMailslot1, hMailslot2;
	LPSTR lpszReadMailslotName = LPTSTR("\\\\.\\mailslot\\$5api_1$");
	char szMailslotName[256], szBuf[512];
	BOOL fReturnCode;
	DWORD cbMessages, cbMsgNumber, cbWritten, cbRead;
	printf("Демонстрация почтового ящика клиента\n");
	printf("Синтакс: mslotclient [имя сервера]\n");
	if (argc > 1) sprintf(szMailslotName, "\\\\%s\\mailslot\\$5api$", argv[1]);
	else strcpy(szMailslotName, "\\\\.\\mailslot\\$5api$");
	hMailslot1 = CreateFile(szMailslotName, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hMailslot1 == INVALID_HANDLE_VALUE) {
		fprintf(stdout, "CreateFile для пересылки: Ошибка %ld\n", GetLastError()); _getch(); return 0;
	}
	fprintf(stdout, "Соединение завершено. Введите 'exit' для завершения\n");
	hMailslot2 = CreateMailslot(lpszReadMailslotName, 0, MAILSLOT_WAIT_FOREVER, NULL);
	if (hMailslot2 == INVALID_HANDLE_VALUE) {
		fprintf(stdout, "CreateMailslot для ответа: Ошибка %ld\n", GetLastError());
		CloseHandle(hMailslot2); _getch(); return 0;
	}
	fprintf(stdout, "Почтовый ящик для ответа создан\n");
	while (1) {
		printf("cmd>"); gets_s(szBuf);
		if (!WriteFile(hMailslot1, szBuf, strlen(szBuf) + 1, &cbWritten, NULL)) break;
		if (!strcmp(szBuf, "exit")) break;
		fprintf(stdout, "Ожидание ответа...\n");
		fReturnCode = GetMailslotInfo(hMailslot2, NULL, &cbMessages, &cbMsgNumber, NULL);
		if (!fReturnCode) {
			fprintf(stdout, "GetMailslotInfo для ответа: Ошибка %ld\n", GetLastError()); _getch();
			break;
		}
		if (ReadFile(hMailslot2, szBuf, 512, &cbRead, NULL)) printf("Полученный: <%s>\n", szBuf);
	}
	CloseHandle(hMailslot1); CloseHandle(hMailslot2);
	return 0;
}
