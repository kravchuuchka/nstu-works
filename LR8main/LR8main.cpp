#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <winsock2.h>  // Wincosk2.h долженбыть
// подключенраньше windows.h!
#include <windows.h>
// подключениебиблиотеки
#pragma comment(lib,"Ws2_32.lib")

#define PORT 60361

// макрос для печати количества активных
// пользователей
#define PRINTNUSERS if(nclients)\
  printf("%d user on-line\n",nclients);\
    else printf("No User on line\n");

typedef int (*Func)(char*, char*);
// прототипфункции, обслуживающий
// подключившихсяпользователей
DWORD WINAPI dostuff(LPVOID client_socket);

// глобальная переменная – количество
// активных пользователей 
int nclients = 0;

// функция по варианту
int myfunc(char* a, char* c)
{
	int i = 0;
	int total = 0;
	FILE* hdl;
	char message[80] = { 0 };
	if (hdl = fopen(a, "rt")) {
		// цикл чтения до конца файла 
		while (!feof(hdl) && i < 1024) {
			a[i] = (char)fgetc(hdl);
			i++;
		}
		a[i] = '\0';
		HINSTANCE hMyDll;
		hMyDll = LoadLibrary((LPCSTR)"LR2.dll");
		if (hMyDll == NULL) {
			printf("Ошибка загрузки библиотеки");
			exit(-1);
		}
		Func MyFunc;
		MyFunc = (Func)GetProcAddress(hMyDll, "MyFunction");
		if (MyFunc == NULL) {
			printf("функция MyFunc ненайдена");
			exit(-2);
		}
		MyFunc(a, c);
	}
	else {
		// сообщениевканал
		sprintf(message, "(Server)Can't open %s!", a);
		strcpy(a, message);
		total = -1;
	}
	return 1;
}

int main(int argc, char* argv[])
{
	char buff[1024];    // Буфер для различных нужд

	printf("TCP SERVER DEMO\n");

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
		printf("Error WSAStartup %d\n",
			WSAGetLastError());
		return -1;
	}

	// Шаг 2 - созданиесокета
	SOCKET mysocket;
	// AF_INET     - система адресации
	// SOCK_STREAM  - потоковый сокет (TCP)
	// 0 - поумолчаниювыбирается TCP протокол
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		// Ошибка!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup();
		// Деиницилизациябиблиотеки Winsock
		return -1;
	}

	// Шаг 3 связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;	// заданиесистемыадресации
	local_addr.sin_port = htons(PORT); // задниепорта
	local_addr.sin_addr.s_addr = INADDR_ANY;	// серверпринимаетподключения
	// на все IP-адреса

	// вызываем bind для связывания
	if (bind(mysocket, (sockaddr*)&local_addr, sizeof(local_addr)))
	{
		// обработкаошибка
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket);  // закрываемсокет!
		WSACleanup();
		return -1;
	}

	// Шаг 4 ожидание подключений
	// размер очереди – 0x100
	if (listen(mysocket, 0x100))
	{
		// Ошибка
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}
	printf("Listen...\n");

	// Шаг 5 извлекаем сообщение из очереди
	SOCKET client_socket;    // сокет клиента
	sockaddr_in client_addr;    // адресклиента
	// (заполняется системой)

	// функции accept необходимо передать размер структуры
	int client_addr_size = sizeof(client_addr);

	// цикл извлечения запросов на подключение из очереди
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
		PRINTNUSERS

			// Вызов нового потока для обслужвания клиента
			DWORD thID;
		CreateThread(NULL, NULL, dostuff, &client_socket, NULL, &thID);
	}
	return 0;
}

// Эта функция создается в отдельном потоке и
// обсуживает очередного подключившегося клиента
// независимо от остальных
DWORD WINAPI dostuff(LPVOID client_socket)
{
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
		printf("Wait %s bytes...\n", bytes);
	len = atoi(bytes);
	lenstr = len;
	if (((bytes_recv = recv(my_sock, buff, len, 0)) == len) && bytes_recv != SOCKET_ERROR) // принятиесообщенияотклиента
	{
		printf("Get %d bytes:%s\n", bytes_recv, buff);
		strncpy(a, buff, bytes_recv); // копирование текста для освобождения буфера
		//a[len] = '\n';
		ZeroMemory(buff, sizeof(buff));
		send(my_sock, str2, sizeof(str2), 0); // отправляем клиенту сообщение
	}
	// обработка второго параметра
	ZeroMemory(bytes, sizeof(bytes));
	bytes_recv = 0;
	if ((len = recv(my_sock, bytes, 2, 0)) > 0 && len != SOCKET_ERROR) // принятие длины сообщения от клиента
		printf("Wait %s byte(s)...\n", bytes);
	len = atoi(bytes);
	lenstr = len;
	if ((bytes_recv = recv(my_sock, buff, len, 0) > 0) && bytes_recv != SOCKET_ERROR) // принятиесообщенияотклиента
	{
		printf("Number of changes:%s\n", buff); //символ для подсчета
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
		printf("Server:%s\n", buff);
		buff[strlen(buff) + 1] = '\r';
		send(my_sock, buff, strlen(buff), 0); // отправляем клиенту результат
	}

	nclients--; // уменьшаем счетчик активных клиентов
	printf("-disconnect\n");
	PRINTNUSERS

		// закрываем сокет
		closesocket(my_sock);
	return 0;
}

