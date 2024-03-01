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
// Идентификаторы объектов - событий, которые используются
// для синхронизации задач, принадлежащих разным процессам, для службы - ГЛОБАЛЬНЫЕ!
int nclients = 0;
SOCKET mysocket;
sockaddr_in local_addr;
SOCKET client_socket;    // сокет клиента
sockaddr_in client_addr;
int client_addr_size;
/////////////

int Server()
{
	DWORD dwRetCode;
	CHAR str[80];
	FILE* hdl, * hout;
	DWORD   total = 0;
	// буфер для  сообщения об ошибке, результата
	char message[512] = { 0 }, Buffer[256];
	addLogMessage("Mapped and shared file, event sync, server process\n");
	// создаем объекты в ServiceStart()
	// Цикл чтения/записи. Этот цикл завершает свою работу
	// при завершении процесса ввода
	addLogMessage("Server Ready!");
	
	while ((client_socket = accept(mysocket, (sockaddr*)
		&client_addr, &client_addr_size)))
	{
		nclients++; // увеличиваем счетчик
		// подключившихся клиентов

		// вывод сведений о клиенте
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

			// Вызов нового потока для обслужвания клиента
			DWORD thID;
		CreateThread(NULL, NULL, dostuff, &client_socket, NULL, &thID);
	}

	// Закрываем идентификаторы объектов-событий  в ServiceStop()
	return 0;
}
int ServiceStart()
{
	char message[80] = { 0 };
	DWORD res;
	char buff[1024];
	sprintf(message,"TCP SERVER DEMO\n");
	addLogMessage(message);
	// Шаг 1 - Инициализация Библиотеки Сокетов
	// Т.к. возвращенная функцией информация
	// не используется ей передается указатель на
	// рабочий буфер, преобразуемый
	// к указателю  на структуру WSADATA.
	// Такой прием позволяет сэкономить одну
	// переменную, однако, буфер должен быть не менее
	// полкилобайта размером (структура WSADATA
	// занимает 400 байт)
	if (WSAStartup(0x0202, (WSADATA*)&buff[0]))
	{
		// Ошибка!
		sprintf(message, "Error WSAStartup %d\n %ld\n", WSAGetLastError());
		addLogMessage(message);
		return -1;
	}

	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// Ошибка!
		sprintf(message, "Error socket %d\n %ld\n", WSAGetLastError());
		addLogMessage(message);
		WSACleanup();
		// Деиницилизациябиблиотеки Winsock
		return -1;
	}

	// Шаг 3 связывание сокета с локальным адресом

	local_addr.sin_family = AF_INET;	// заданиесистемыадресации
	local_addr.sin_port = htons(PORT); // задниепорта
	local_addr.sin_addr.s_addr = INADDR_ANY;	// серверпринимаетподключения
	// на все IP-адреса

	// вызываем bind для связывания
	if (bind(mysocket, (sockaddr*)&local_addr, sizeof(local_addr)))
	{
		// обработкаошибка
		sprintf(message, "Error bind %ld\n", WSAGetLastError());
		addLogMessage(message);
		closesocket(mysocket);  // закрываемсокет!
		WSACleanup();
		return -2;
	}

	// Шаг 4 ожидание подключений
	// размер очереди – 0x100
	if (listen(mysocket, 0x100))
	{
		// Ошибка
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
	// отправляемклиентусообщение
	send(my_sock, str1, sizeof(str1), 0);
	int bytes_recv, len, lenstr; // размерпринятогосообщения
	//int c; // переменныедля myfunc
	char* a = (char*)calloc(1024, sizeof(char));
	char bytes[3] = { 0 };
	if ((len = recv(my_sock, bytes, 2, 0)) > 0 && len != SOCKET_ERROR) // принятие длины сообщения от клиента
	{
		sprintf(message, "Wait %s bytes...\n", bytes);
		addLogMessage(message);
	}
	len = atoi(bytes);
	lenstr = len;
	if (((bytes_recv = recv(my_sock, buff, len, 0)) == len) && bytes_recv != SOCKET_ERROR) // принятиесообщенияотклиента
	{
		sprintf(message, "Get %d bytes:%s\n", bytes_recv, buff);
		addLogMessage(message);
		strncpy(a, buff, bytes_recv); // копирование текста для освобождения буфера
		//a[len] = '\n';
		ZeroMemory(buff, sizeof(buff));
		send(my_sock, str2, sizeof(str2), 0); // отправляем клиенту сообщение
	}
	// обработка второго параметра
	ZeroMemory(bytes, sizeof(bytes));
	bytes_recv = 0;
	if ((len = recv(my_sock, bytes, 2, 0)) > 0 && len != SOCKET_ERROR) // принятие длины сообщения от клиента
	{
		sprintf(message, "Wait %s bytes...\n", bytes);
		addLogMessage(message);
	}
	len = atoi(bytes);
	lenstr = len;
	if ((bytes_recv = recv(my_sock, buff, len, 0) > 0) && bytes_recv != SOCKET_ERROR) // принятиесообщенияотклиента
	{
		sprintf(message, "Number of changes:%s\n", buff);
		addLogMessage(message);

		//проверки на тупейшие просьбы о вводе от гунько


		len = myfunc(a, buff); // вызов пользовательской функции
		ZeroMemory(buff, sizeof(buff));
		//itoa(len, bytes, 10); // преобразование результата выполнения в строку

		if (len >= 0)
		{
			strcpy(buff, "Answer:");
			strcat(buff, a);
		}
		else strcat(buff, a);
		buff[strlen(buff)] = '\n';// '\n'; // добавлениексообщениюсимволаконцастроки
		sprintf(message, "Server:%s\n", buff);
		addLogMessage(message);
		buff[strlen(buff) + 1] = '\r';
		send(my_sock, buff, strlen(buff), 0); // отправляем клиенту результат
	}

	nclients--; // уменьшаем счетчик активных клиентов
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

		// закрываем сокет
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
			// цикл чтения до конца файла 
			a[0] = '\0';
			while (!feof(hdl) && i < 1024) {
				a[i] = (char)fgetc(hdl);
				i++;
			}
			a[i] = '\0';
			HINSTANCE hMyDll;
			hMyDll = LoadLibrary((LPCSTR)"LR2.dll");
			if (hMyDll == NULL) {
				sprintf(message, "Ошибка загрузки библиотеки");
				addLogMessage(message);
				exit(-1);
			}
			Func MyFunc;    
			MyFunc = (Func)GetProcAddress(hMyDll, "MyFunction");
			if (MyFunc == NULL) {
				sprintf(message, "функция MyFunc ненайдена");
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
		// сообщениевканал
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

