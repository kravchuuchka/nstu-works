#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include "dllh.h"
#define BUF_SIZE 256
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		printf("The DLL is loaded");
		break;
	case DLL_THREAD_ATTACH:
		printf("A thread is created in this process");
		break;
	case DLL_THREAD_DETACH:
		printf("A thread is destroyed in this process");
		break;
	case DLL_PROCESS_DETACH:
		printf("The DLL is unloaded");
		break;
	}
	return TRUE;
}
EXPORT int MyFunction(char* Buffer, int count) {
	for (int i = 1; i < strlen(Buffer) && count>0; i++) {
		if (Buffer[i] == Buffer[i - 1]) {
			Buffer[i] = ' ';
			count--;
		}
	}
	return count;
}

