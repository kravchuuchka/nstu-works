#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <AclAPI.h> // for SetSecurityInfo
#pragma comment(lib,"Ws2_32.lib")
#define PORT 60361


typedef int (*Func)(char*, char*);
using namespace std;
extern int addLogMessage(const char* text);
DWORD WINAPI dostuff(LPVOID client_socket);
int myfunc(char* a, char* c);
int ServiceStart();
void ServiceStop();
// �������������� �������� - �������, ������� ������������
// ��� ������������� �����, ������������� ������ ���������, ��� ������ - ����������!
int nclients = 0;
SOCKET mysocket;
sockaddr_in local_addr;
SOCKET client_socket;    // ����� �������
sockaddr_in client_addr;
int client_addr_size;
/////////////

int Server()
{
	DWORD dwRetCode;
	CHAR str[80];
	FILE* hdl, * hout;
	DWORD   total = 0;
	// ����� ���  ��������� �� ������, ����������
	char message[512] = { 0 }, Buffer[256];
	addLogMessage("Mapped and shared file, event sync, server process\n");
	// ������� ������� � ServiceStart()
	// ���� ������/������. ���� ���� ��������� ���� ������
	// ��� ���������� �������� �����
	addLogMessage("Server Ready!");
	
	while ((client_socket = accept(mysocket, (sockaddr*)
		&client_addr, &client_addr_size)))
	{
		nclients++; // ����������� �������
		// �������������� ��������

		// ����� �������� � �������
		HOSTENT* hst;
		hst = gethostbyaddr((char*)&client_addr.sin_addr.s_addr, 4, AF_INET);
		printf("+%s [%s] new connect!\n",
			(hst) ? hst->h_name : "",
			inet_ntoa(client_addr.sin_addr));
		if (nclients)
		{
			sprintf(message, "%d user on-line\n", nclients);
			addLogMessage(message);
		}
		else
		{
			sprintf(message, "No User on line\n");
			addLogMessage(message);
		}

			// ����� ������ ������ ��� ����������� �������
			DWORD thID;
		CreateThread(NULL, NULL, dostuff, &client_socket, NULL, &thID);
	}

	// ��������� �������������� ��������-�������  � ServiceStop()
	return 0;
}
int ServiceStart()
{
	char message[80] = { 0 };
	DWORD res;
	char buff[1024];
	sprintf(message,"TCP SERVER DEMO\n");
	addLogMessage(message);
	// ��� 1 - ������������� ���������� �������
	// �.�. ������������ �������� ����������
	// �� ������������ �� ���������� ��������� ��
	// ������� �����, �������������
	// � ���������  �� ��������� WSADATA.
	// ����� ����� ��������� ���������� ����
	// ����������, ������, ����� ������ ���� �� �����
	// ������������ �������� (��������� WSADATA
	// �������� 400 ����)
	if (WSAStartup(0x0202, (WSADATA*)&buff[0]))
	{
		// ������!
		sprintf(message, "Error WSAStartup %d\n %ld\n", WSAGetLastError());
		addLogMessage(message);
		return -1;
	}

	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// ������!
		sprintf(message, "Error socket %d\n %ld\n", WSAGetLastError());
		addLogMessage(message);
		WSACleanup();
		// ������������������������ Winsock
		return -1;
	}

	// ��� 3 ���������� ������ � ��������� �������

	local_addr.sin_family = AF_INET;	// �����������������������
	local_addr.sin_port = htons(PORT); // �����������
	local_addr.sin_addr.s_addr = INADDR_ANY;	// ��������������������������
	// �� ��� IP-������

	// �������� bind ��� ����������
	if (bind(mysocket, (sockaddr*)&local_addr, sizeof(local_addr)))
	{
		// ���������������
		sprintf(message, "Error bind %ld\n", WSAGetLastError());
		addLogMessage(message);
		closesocket(mysocket);  // ��������������!
		WSACleanup();
		return -2;
	}

	// ��� 4 �������� �����������
	// ������ ������� � 0x100
	if (listen(mysocket, 0x100))
	{
		// ������
		sprintf(message, "Error listen %ld\n", WSAGetLastError());
		addLogMessage(message);
		closesocket(mysocket);
		WSACleanup();
		return -3;
	}
	printf("Listen...\n");
	client_addr_size = sizeof(client_addr);
	return 0;

}


DWORD WINAPI dostuff(LPVOID client_socket)
{
	char message[80] = { 0 };
	SOCKET my_sock;
	my_sock = ((SOCKET*)client_socket)[0];
	char* buff = (char*)calloc(1024, sizeof(char));
#define str1 "Enter filename up to 1024 bytes\r\n"
#define str2 "Enter symbol for change\r\n" 
	char symbols[3];
	// ��������������������������
	send(my_sock, str1, sizeof(str1), 0);
	int bytes_recv, len, lenstr; // ������������������������
	//int c; // ������������� myfunc
	char* a = (char*)calloc(1024, sizeof(char));
	char bytes[3] = { 0 };
	if ((len = recv(my_sock, bytes, 2, 0)) > 0 && len != SOCKET_ERROR) // �������� ����� ��������� �� �������
	{
		sprintf(message, "Wait %s bytes...\n", bytes);
		addLogMessage(message);
	}
	len = atoi(bytes);
	lenstr = len;
	if (((bytes_recv = recv(my_sock, buff, len, 0)) == len) && bytes_recv != SOCKET_ERROR) // ��������������������������
	{
		sprintf(message, "Get %d bytes:%s\n", bytes_recv, buff);
		addLogMessage(message);
		strncpy(a, buff, bytes_recv); // ����������� ������ ��� ������������ ������
		//a[len] = '\n';
		ZeroMemory(buff, sizeof(buff));
		send(my_sock, str2, sizeof(str2), 0); // ���������� ������� ���������
	}
	// ��������� ������� ���������
	ZeroMemory(bytes, sizeof(bytes));
	bytes_recv = 0;
	if ((len = recv(my_sock, bytes, 2, 0)) > 0 && len != SOCKET_ERROR) // �������� ����� ��������� �� �������
	{
		sprintf(message, "Wait %s bytes...\n", bytes);
		addLogMessage(message);
	}
	len = atoi(bytes);
	lenstr = len;
	if ((bytes_recv = recv(my_sock, buff, len, 0) > 0) && bytes_recv != SOCKET_ERROR) // ��������������������������
	{
		sprintf(message, "Number of changes:%s\n", buff);
		addLogMessage(message);

		//�������� �� �������� ������� � ����� �� ������


		len = myfunc(a, buff); // ����� ���������������� �������
		ZeroMemory(buff, sizeof(buff));
		//itoa(len, bytes, 10); // �������������� ���������� ���������� � ������

		if (len >= 0)
		{
			strcpy(buff, "Answer:");
			strcat(buff, a);
		}
		else strcat(buff, a);
		buff[strlen(buff)] = '\n';// '\n'; // ��������������������������������������
		sprintf(message, "Server:%s\n", buff);
		addLogMessage(message);
		buff[strlen(buff) + 1] = '\r';
		send(my_sock, buff, strlen(buff), 0); // ���������� ������� ���������
	}

	nclients--; // ��������� ������� �������� ��������
	sprintf(message, "-disconnect\n");
	addLogMessage(message);
	if (nclients)
	{
		sprintf(message, "%d user on-line\n", nclients);
		addLogMessage(message);
	}
	else
	{
		sprintf(message, "No User on line\n");
		addLogMessage(message);
	}

		// ��������� �����
		closesocket(my_sock);
	return 0;
}



int myfunc(char* a, char* c)
{
	int i = 0;
	int total = 0;
	FILE* hdl,*hout;
	char message[80] = { 0 };
	if (hdl = fopen(a, "rt")) {
		char* p = strchr(a, '.');
		if (p) *(p + 1) = '\0';
		strcat(a, "log");
		if (hout = fopen(a, "w+")) {
			// ���� ������ �� ����� ����� 
			a[0] = '\0';
			while (!feof(hdl) && i < 1024) {
				a[i] = (char)fgetc(hdl);
				i++;
			}
			a[i] = '\0';
			HINSTANCE hMyDll;
			hMyDll = LoadLibrary((LPCSTR)"LR2.dll");
			if (hMyDll == NULL) {
				sprintf(message, "������ �������� ����������");
				addLogMessage(message);
				exit(-1);
			}
			Func MyFunc;    
			MyFunc = (Func)GetProcAddress(hMyDll, "MyFunction");
			if (MyFunc == NULL) {
				sprintf(message, "������� MyFunc ���������");
				addLogMessage(message);
				exit(-2);
			}
			MyFunc(a, c);
			a[strlen(a)] = '\0';
			fprintf(hout, "%s", a);
			fclose(hdl);
			fclose(hout);
		}
		else
		{
			sprintf(message, "(Server)Can't open %s!", a);
			addLogMessage(message);
			total = -1;
			fclose(hdl);
		}
	}
	else {
		// ���������������
		sprintf(message, "(Server)Can't open %s!",a);
		addLogMessage(message);
		total = -1;
	}
	return 1;
}






void ServiceStop()
{

	addLogMessage("All Kernel objects closed!");
}

