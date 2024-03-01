#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"Ws2_32.lib")
#define PORT 60361
#define SERVERADDR "127.0.0.1"

int main(int argc, char* argv[])
{
	char buff[1024];
	char servaddr[80] = { 0 };
	char close[5] = { 0 };
	printf("TCP DEMO CLIENT\n");

	// Шаг 1 - инициализациябиблиотеки Winsock
	if (WSAStartup(0x202, (WSADATA*)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 2 - созданиесокета
	SOCKET my_sock;
	my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 3 - установка соединения
	// заполнение структуры sockaddr_in - указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT* hst;

	// получение IP адреса и установка соединения
	if (argc > 1) strcpy(servaddr, argv[1]);
	else strcpy(servaddr, SERVERADDR);
	printf("Connecting %s...\n", servaddr);
	if (inet_addr(servaddr) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(servaddr); //преобразование IP адресаизсимвольноговсетевойформат

	else
	{
		// попытка получить IP адрес по доменному имени сервера
		if (hst = gethostbyname(servaddr))
			// hst->h_addr_list содержитнемассивадресов,
			// а массив указателей на адреса
			((unsigned long*)&dest_addr.sin_addr)[0] =
			((unsigned long**)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", servaddr);
			closesocket(my_sock);
			WSACleanup();
			return -1;
		}
	}

	// адрес сервера получен - пытаемся установить соединение
	if (connect(my_sock, (sockaddr*)&dest_addr, sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		return -1;
	}

	printf("Connect with %s succeed\n \
		                Type quit for quit\n\n", servaddr);

	// Шаг 4 - чтение и передача сообщений
	int nsize;
	while ((nsize = recv(my_sock, &buff[0], sizeof(buff) - 1, 0)) != SOCKET_ERROR)
	{
		// ставим завершающий ноль в конце строки
		buff[nsize] = 0;
		// выводим на экран
		//buff[nsize+1] = '\n';
		printf("S=>C:%s", buff);
		ZeroMemory(buff, sizeof(buff));
		// читаем пользовательский ввод (имя файла) с клавиатуры
		printf("S<=C:"); fgets(buff, sizeof(buff) - 1, stdin);
		// проверка на "quit"
		if (!strcmp(&buff[0], "quit\n"))
		{
			// Корректный выход
			printf("Exit...");
			closesocket(my_sock);
			WSACleanup();
			return 0;
		}
		//sprintf(close, "%s", strlen(buff));
		_itoa(strlen(buff) - 1, close, 10);
		// передаем длину строки клиента серверу
		send(my_sock, close, strlen(close), 0);
		// передаем строку клиента серверу
		//buff[strlen(buff)] = 0;
		nsize = send(my_sock, buff, strlen(buff) - 1, 0);
		//send(my_sock, close, 2, 0);
	}
	printf("Recv error %d\n", WSAGetLastError());
	closesocket(my_sock);
	WSACleanup();
	return -1;
}

