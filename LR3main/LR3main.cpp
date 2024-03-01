#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#pragma argsused
int main(int argc, char* argv[])
{
	int i;
	DWORD finish, result, total = 0;
	LPDWORD res = &result;
	char line[80];
	char* ln = line;
	HANDLE hThread[255];
	STARTUPINFO si[255];
	PROCESS_INFORMATION pi[255];
	if (argc < 3) {
		printf("No file to process!\n");
		exit(-1);
	}

	if (strlen(argv[2]) != 2) {
		printf("Incorrect number of characters entered!\n");
		exit(-1);
	}

	for (i = 0; i < (argc - 1); i = i + 2) {
		strcpy(ln, "LR3.exe");
		ln = strcat(ln, " ");
		ln = strcat(ln, argv[i + 1]);
		ln = strcat(ln, " ");
		ln = strcat(ln, argv[i + 2]);

		ZeroMemory(&si[i], sizeof(si[i]));
		si[i].cb = sizeof(si);
		ZeroMemory(&pi[i], sizeof(pi[i]));
		printf("Command line:%s\n", line);
		if (!CreateProcess(NULL,
			line,
			NULL,
			NULL,
			TRUE,
			NULL,
			NULL,
			NULL,
			&si[i],
			&pi[i])
			)
		{
			printf("CreateProcess failed.\n");
			exit(-2);
		}
		else {
			printf("Process %lu started for file %s\n", pi[i].dwProcessId, argv[i + 1]);
			hThread[i] = pi[i].hProcess;
			Sleep(1000);
		}
	}

	for (i = 0; i < argc - 1; i = i + 2)
	{
		finish = WaitForSingleObject(pi[i].hProcess, INFINITE);
		if (finish == WAIT_OBJECT_0) {

			GetExitCodeProcess(pi[i].hProcess, res);
			printf("Process %lu finished by %d\n", pi[i].dwProcessId, result);
		}
		else
		{
			CloseHandle(pi[i].hProcess);
			CloseHandle(pi[i].hThread);
			TerminateProcess(pi[i].hProcess, 0);
		}
	}

	return 0;
}

