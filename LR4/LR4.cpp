#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <conio.h>

DWORD main(int argc, char* argv[])
{
	// Идентификатор канала Pipe
	HANDLE hNamedPipe;
	// Количество байт, переданных через канал
	DWORD cbWritten;
	// Количество байт, принятых через канал
	DWORD cbRead;
	// Буфер для передачи данных
	char szBuf[256];
	// Буфер для имени канала Pipe
	char szPipeName[256];
	printf("Named pipe client demo\n");
	printf("Syntax: namedpipeclient [servername]\n");
	// Если при запуске было указано имя сервера, указываем его в имени канала Pipe
	if (argc > 1)
		sprintf(szPipeName, "\\\\%s\\pipe\\$MyPipe$",
			argv[1]);
	// Если имя сервера задано не было, создаем канал с локальным процессом
	else
		strcpy(szPipeName, "\\\\.\\pipe\\$MyPipe$");
	// Создаем канал с процессом NAMEDPIPESERVER
	hNamedPipe = CreateFile(
		szPipeName, GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, 0, NULL);
	// Если возникла ошибка, выводим ее код и завершаем работу приложения
	if (hNamedPipe == INVALID_HANDLE_VALUE)
	{
		fprintf(stdout, "CreateFile: Error %ld\n",
			GetLastError());
		return 0;
	}
	// Выводим сообщение о создании канала
	fprintf(stdout, "\nConnected. Type 'exit' to terminate\n");
	// Цикл обмена данными с серверным процессом
	while (1)
	{
		// Выводим приглашение для ввода команды
		printf("cmd>");
		// Вводим текстовую строку
		gets_s(szBuf);
		// Передаем введенную строку серверному процессу в качестве команды
		if (!WriteFile(hNamedPipe, szBuf, strlen(szBuf) + 1,
			&cbWritten, NULL))
			break;
		// Получаем эту же команду обратно от сервера
		if (ReadFile(hNamedPipe, szBuf, 512, &cbRead, NULL)){
			szBuf[cbRead] = '\0';
			printf("Received back: <%s>\n", szBuf);
		}

		// Если произошла ошибка, выводим ее код и завершаем работу приложения
		else
		{
			fprintf(stdout, "ReadFile: Error %ld\n",
				GetLastError());
			break;
		}
		// В ответ на команду "exit" завершаем цикл обмена данными с серверным процессом
		if (!strcmp(szBuf, "exit"))
			break;
	}
	// Закрываем идентификатор канала
	CloseHandle(hNamedPipe);
	return 0;
}

