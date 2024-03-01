#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include   <windows.h>
#include   <stdio.h>
#include   <string.h>
#define threads 64
typedef int (*Func)(char*, char*);
// глобальный мьютекс
HANDLE hMutex;
// стркутура для обмена данными
struct MYDATA
{
	int num;
	char file[80];
	char symbols[3];
	char Buffer[256];
} data[threads];
// функцияпотока
DWORD WINAPI threadfunc(LPVOID);
int main(int argc, char* argv[])
{
	// дескрипторы потоков
	HANDLE hThreads[threads];
	// данные для потоков
	//struct MYDATA data;
	// вспомогательные переменные
	int i;
	DWORD res;
	if (argc < 4)
	{
		printf("Usage: mthreads.exe filename ...");
		exit(-1);
	}
	// создание мьютекса
	//hMutex = CreateMutex(NULL, FALSE, NULL);
	// захват мьютекса
	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < argc - 1; i += 2)
	{

		// захватмьютекса
		//WaitForSingleObject(hMutex, INFINITE);
		// заполнение структуры данных
		data[i / 2].num = i / 2;
		strcpy(data[i / 2].file, argv[i + 1]);
		data[i / 2].file[strlen(argv[i + 1])] = '\0';
		strcpy(data[i / 2].symbols, argv[i + 2]);
		data[i / 2].symbols[strlen(argv[i + 2])] = '\0';
		//ReleaseMutex(hMutex);
		hThreads[i / 2] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadfunc, (LPVOID)&data[i / 2], 0, NULL);
		if (hThreads[i / 2] == NULL)
		{
			printf("Cant create thread %d!\n", i / 2);
			exit(-i);
		}
		else printf("(Main)Create thread %d!\n", i / 2);
	}
	// освобождение мьютекса, чтобы отработали потоки
	ReleaseMutex(hMutex);
	// удаляем мьютекс
	CloseHandle(hMutex);
	// закрываем дескрипторы потоков
	for (i = 0; i < (argc - 1) / 2; i++)
	{
		// убедимся, чтовсепотокизавершены
		WaitForSingleObject(hThreads[i], INFINITE);
		// проверка результата
		// проверка результата
		GetExitCodeThread(hThreads[i], &res);
		//printf("(Main)In file %s found spaces %d\n", argv[i+1], (int)res);
		printf("\n(Main)In file %s found str %s\n", data[i].file, data[i].Buffer);
		CloseHandle(hThreads[i]);
	}
}
DWORD WINAPI threadfunc(LPVOID num)
{
	MYDATA dat = *((MYDATA*)num);
	int i = dat.num;
	int j = 0;
	char Buffer[256];
	FILE* hdl;

	printf("(F)Thread #%d wait mutex...\n", i);
	// захват мьютекса
	WaitForSingleObject(hMutex, INFINITE);
	printf("(F)Thread #%d catch mutex! Open file %s...\n", i, dat.file);
	if (hdl = fopen(dat.file, "rt")) {


		int j = 0;
		while (!feof(hdl) && j < 512) {
			dat.Buffer[j] = (char)fgetc(hdl);
			j++;
		}
		Buffer[j] = '\0';
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
		MyFunc(dat.Buffer, dat.symbols);

		strcpy(data[i].Buffer, dat.Buffer);

		printf("(ThreadID: %lu), File %s str = %s\n", GetCurrentThreadId(), dat.file, dat.Buffer);
		// закрытие файла
		fclose(hdl);
		// освобождение мьютекса
		ReleaseMutex(hMutex);
		return 1;

	}
	else {
		printf("(ThreadId: %lu), Can't open file %s!\n", GetCurrentThreadId(), dat.file);
		// освобождение мьютекса, иначе он - покинутый!
		ReleaseMutex(hMutex);
		ExitThread(-i);
	}
}

