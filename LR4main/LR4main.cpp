#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <string>
typedef int (*Func)(char*, char*);
using namespace std;
int main()
{
	BOOL   fConnected;
	HANDLE hNamedPipe;
	LPCSTR  lpszPipeName = "\\\\.\\pipe\\$MyPipe$";
	char   szBuf[512], Buffer[256];
	DWORD  cbRead;
	DWORD  cbWritten;
	DWORD   total = 0;
	int i = 0, j = 0;
	char message[512] = { 0 };
	HANDLE hIn, hOut;
	printf("Named pipe server demo\n");
	hNamedPipe = CreateNamedPipe(
		lpszPipeName,
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
		PIPE_UNLIMITED_INSTANCES,
		512, 512, 5000, NULL);
	// Если возникла ошибка, выводим ее код и зваершаем работу приложения
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "CreateNamedPipe: Error %ld\n",
			GetLastError());
		_getch();
		return 0;
	}
	// Выводим сообщение о начале процесса создания канала
	fprintf(stdout, "Waiting for connect...\n");
	// Ожидаем соединения со стороны клиента
	fConnected = ConnectNamedPipe(hNamedPipe, NULL);
	// При возникновении ошибки выводим ее код
	if (!fConnected)
	{
		switch (GetLastError())
		{
		case ERROR_NO_DATA:
			fprintf(stdout, "ConnectNamedPipe: ERROR_NO_DATA");
			_getch();
			CloseHandle(hNamedPipe);
			return 0;
			break;
		case ERROR_PIPE_CONNECTED:
			fprintf(stdout,
				"ConnectNamedPipe: ERROR_PIPE_CONNECTED");
			_getch();
			CloseHandle(hNamedPipe);
			return 0;
			break;
		case ERROR_PIPE_LISTENING:
			fprintf(stdout,
				"ConnectNamedPipe: ERROR_PIPE_LISTENING");
			_getch();
			CloseHandle(hNamedPipe);
			return 0;
			break;
		case ERROR_CALL_NOT_IMPLEMENTED:
			fprintf(stdout,
				"ConnectNamedPipe: ERROR_CALL_NOT_IMPLEMENTED");
			_getch();
			CloseHandle(hNamedPipe);
			return 0;
			break;
		default:
			fprintf(stdout, "ConnectNamedPipe: Error %ld\n",
				GetLastError());
			_getch();
			CloseHandle(hNamedPipe);
			return 0;
			break;
		}
		CloseHandle(hNamedPipe);
		_getch();
		return 0;
	}
	// Выводим сообщение об успешном создании канала
	fprintf(stdout, "\nConnected. Waiting for command...\n");
	// Цикл получения команд через канал
	while (1)
	{
		// Получаем очередную команду через канал Pipe
		if (ReadFile(hNamedPipe, szBuf, 512, &cbRead, NULL))
		{
			// Выводим принятую команду на консоль 
			printf("Received: %s\n", szBuf);
			// Если пришла команда "exit", завершаем работу приложения
			if (!strcmp(szBuf, "exit"))
			{
				WriteFile(hNamedPipe, szBuf, strlen(szBuf) + 1, &cbWritten, NULL);
				break;
			}
			// Иначе считаем что принято имя файла
			else
			{
				char path[256], symbols[3];
				symbols[0] = '0'; symbols[1] = '\0';
				bool ch = true;
				i = 0;
				while (szBuf[i] != ' ' && szBuf[i] != '\0')
				{
					path[i] = szBuf[i];
					i++;
				}
				path[i] = '\0';
				if (szBuf[i] == ' ')
					i++;
				while (szBuf[i] != '\0' && j < 2)
				{
					symbols[j] = szBuf[i];
					i++;
					j++;
				}
				symbols[j] = '\0';
				if (symbols[1] == '\0')
				{
					symbols[1] = 'b';
					printf("The second character was not entered, by default it will become b\n");
					if (symbols[0] == '\0') {
						symbols[0] = 'a';
						printf("The first character was not entered, by default it will become a\n");
					}
					symbols[2] = '\0';
				}
				
				printf("Symbols: %s\n", symbols);

				hIn = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);

				if (hIn == INVALID_HANDLE_VALUE) {
					sprintf(message, "(Server)Can't open %s!", szBuf);
					WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
					printf("\n");
					WriteFile(hNamedPipe, message, strlen(message), &cbWritten, NULL);
				}
				i = 0;
				while (path[i] != '.' && path[i] != '\0') i++;
				path[i] = '\0';
				strcat(path, ".log");
				hOut = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				if (hOut == INVALID_HANDLE_VALUE) {
					sprintf(message, "(Server)Can't open %s!", szBuf);
					WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
					printf("\n");
					WriteFile(hNamedPipe, message, strlen(message), &cbWritten, NULL);
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

					while (ReadFile(hIn, Buffer, 256, &cbRead, NULL) && cbRead > 0) {
						Buffer[cbRead] = '\0';
						MyFunc(Buffer, symbols);
						Buffer[cbRead] = '\0';
						WriteFile(hOut, Buffer, cbRead, &cbWritten, NULL);
						// сообщение в консоль ошибок 
						sprintf(message, "\n(Server): file:%s, str = %s\n", szBuf, Buffer);
						WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
						// сообщение в канал 
						sprintf(message, "\n%s", Buffer);
						WriteFile(hNamedPipe, message, strlen(message), &cbWritten, NULL);
						CloseHandle(hIn);
						CloseHandle(hOut);
					}
			}
		}
		else
		{
			printf("ReadFile: Error %ld\n", GetLastError());
			break;
		}
	}
	CloseHandle(hNamedPipe);
	return 0;
}

