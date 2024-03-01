#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <conio.h>
typedef int (*Func)(char*, char*);
// Идентификаторы объектов-семафоров, которые используются
// для синхронизации задач, принадлежащих разным процессам
HANDLE hSemSend;
HANDLE hSemRecv;
HANDLE hSemTermination;
HANDLE hSems[2];
// Имя объектов-семафоров для синхронизации записи и чтения из отображаемого файла
CHAR lpSemSendName[] =
"$MySemSendName$";
CHAR lpSemRecvName[] =
"$MySemRecvName$";
// Имя объекта-события для завершения процесса
CHAR lpSemTerminationName[] =
"$MySemTerminationName$";
// Имя отображния файла на память
CHAR lpFileShareName[] =
"$MyFileShareName$";
// Идентификатор отображения файла на память
HANDLE hFileMapping;
// Указатель на отображенную область памяти
LPVOID lpFileMap;
LONG res;
LPLONG lpres = &res;
int main()
{
	DWORD dwRetCode;
	CHAR str[80];
	FILE* hdl;
	DWORD  cbWritten;
	DWORD   total = 0;
	// буфер для  сообщения об ошибке, результата
	char message[256] = { 0 }, Buffer[256];
	int i = 0, j = 0;
	printf("Mapped and shared file, semaphore sync, server process\n");
	// Создаем объекты-семафоры для синхронизации 
	// записи и чтения в отображаемый файл, выполняемого в разных процессах
	hSemSend = CreateSemaphore(NULL, 0, 1, lpSemSendName);
	hSemRecv = CreateSemaphore(NULL, 0, 1, lpSemRecvName); // установлен изначально!
	// Если произошла ошибка, получаем и отображаем ее код,
	// а затем завершаем работу приложения
	if (hSemSend == NULL || hSemRecv == NULL)
	{
		fprintf(stdout, "CreateSemaphore: Error %ld\n",
			GetLastError());
		return 0;
	}
	// Если объект-семафор с указанным именем существует,
	// считаем, что приложение-сервер уже было запущено
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		printf("\nServerApplication already started\n"
			"Press any key to exit...");
		return 0;
	}
	// Создаем объект-семафор для определения момента
	// завершения работы процесса ввода
	hSemTermination = CreateSemaphore(NULL, 0, 1, lpSemTerminationName);
	if (hSemTermination == NULL)
	{
		fprintf(stdout, "CreateSemaphore (Termination): Error %ld\n",
			GetLastError());
		return 0;
	}
	// Создаем объект-отображение, файл не создаем!!!
	hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE,
		NULL, PAGE_READWRITE, 0, 100, lpFileShareName);
	// Если создать не удалось, выводим код ошибки
	if (hFileMapping == NULL)
	{
		fprintf(stdout, "CreateFileMapping: Error %ld\n",
			GetLastError());
		return 0;
	}
	// Выполняем отображение файла на память.
	// В переменную lpFileMap будет записан указатель на
	// отображаемую область памяти
	lpFileMap = MapViewOfFile(hFileMapping,
		FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
	// Если выполнить отображение не удалось,
	// выводим код ошибки
	if (lpFileMap == 0)
	{
		fprintf(stdout, "MapViewOfFile: Error %ld\n",
			GetLastError());
		return 0;
	}
	// Готовим массив идентификаторов семафоров
	// для функции WaitForMultipleObjects
	hSems[0] = hSemTermination;
	hSems[1] = hSemSend;
	// Цикл чтения/записи. Этот цикл завершает свою работу
	// при завершении процесса-клиента
	while (TRUE)
	{
		total = 0;
		// Выполняем ожидание одного из двух событий:
		//   - завершение клиентского процесса;
		//   - завершение записи данных клиентом
		dwRetCode = WaitForMultipleObjects(2,
			hSems, FALSE, INFINITE);
		// Если ожидание любого из двух событий было отменено,
		//  или если произошла ошибка, прерываем цикл
		if (dwRetCode == WAIT_ABANDONED_0 ||
			dwRetCode == WAIT_ABANDONED_0 + 1 ||
			dwRetCode == WAIT_OBJECT_0 ||
			//dwRetCode == WAIT_OBJECT_0 + 1||
			dwRetCode == WAIT_FAILED)
			break;
		// если произошла установка любого из двух семафоров из массива
		// Читаем данные (имя файла для обработки) из отображенной
		// области памяти, записанный туда клиентским 
		// процессом, и отображаем его в консольном окне
		else
		{
			puts(((LPSTR)lpFileMap));
			// обработка данных
			strcpy(str, ((LPSTR)lpFileMap));

			char path[256], symbols[3];
			bool ch = true;
			i = 0;
			while (str[i] != ' ' && str[i] != '\0')
			{
				path[i] = str[i];
				i++;
			}
			path[i] = '\0';
			if (str[i] == ' ')
				i++;
			j = 0;
			while (str[i] != '\0')
			{
				symbols[j] = str[i];
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

			if (hdl = fopen(path, "rt")) {
				// цикл чтения до конца файла 
				i = 0;
				while (!feof(hdl) && i < 512) {
					// чтение одного символа из файла
					Buffer[i] = (char)fgetc(hdl);
					i++;
				}
				Buffer[i] = '\0';
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
				// сообщение в консоль ошибок 
				sprintf(message, "(Server): file:%s, spaces = %s\n", str, Buffer);
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message), &cbWritten, NULL);
				// сообщение в канал 
				sprintf(message, "%s", Buffer);
				//WriteFile(hNamedPipe, message, strlen(message) + 1, &cbWritten, NULL);
				strcpy(((LPSTR)lpFileMap), message);
				// закрытие файла
				fclose(hdl);
			}




			else {
				// сообщение в канал 
				sprintf(message, "(Server)Can't open %s!", str);
				WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, strlen(message) + 1, &cbWritten, NULL);
				printf("\n");
				//WriteFile(hNamedPipe, message, strlen(message) + 1, &cbWritten, NULL);
				strcpy(((LPSTR)lpFileMap), message);
			}
			// разрешакм читать клиенту
			ReleaseSemaphore(hSemRecv, 1, lpres);
		}
	}
	// Закрываем идентификаторы объектов-семафоров  
	CloseHandle(hSemSend);
	CloseHandle(hSemRecv);
	CloseHandle(hSemTermination);
	// Отменяем отображение файла
	UnmapViewOfFile(lpFileMap);
	// Освобождаем идентификатор созданного
	// объекта-отображения
	CloseHandle(hFileMapping);

	return 0;
}

