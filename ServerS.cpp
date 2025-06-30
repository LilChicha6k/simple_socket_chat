#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <mutex>
#include <string>
#pragma comment (lib, "ws2_32")
#pragma warning (disable: 4996)

SOCKET Connections[100];
int Count{};
std::mutex mtx{};

void clietnHandler(int index)
{
	int name_size{};
	int msg_size{};
	int bytes_received{};
	while (true)
	{
		recv(Connections[index], (char*)&name_size, sizeof(int), NULL);

		char* name = new char[name_size + 1];
		name[name_size] = '\0';

		recv(Connections[index], name, name_size, NULL);
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);

		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';

		bytes_received = recv(Connections[index], msg, msg_size, NULL);

		if (bytes_received <= 0)
		{
			mtx.lock();
			if (Connections[index] != INVALID_SOCKET)
			{
				closesocket(Connections[index]);
				Connections[index] = INVALID_SOCKET;
				std::cout << "Client " << index << "Disconect\n";
			}
			mtx.unlock();
			return;
		}
			
		for (int i = 0; i < Count; i++)
		{
			if (i != index&& Connections[index]!= INVALID_SOCKET)
			{

				send(Connections[i], (char*)&name_size, sizeof(int), NULL);
				send(Connections[i], name, name_size, NULL);
				send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
				send(Connections[i], msg, msg_size, NULL);
			}
		}

		delete[] name;
		delete[] msg;
	}
}

int main()
{
	WSAData ws_data;
	WORD DLL_version = MAKEWORD(2, 1);

	if (WSAStartup(DLL_version, &ws_data) != 0)
	{
		std::cout << "Error WSASStartup" << '\n';
		exit(EXIT_FAILURE);
	}

	SOCKADDR_IN addr;
	int sizeof_addr{ sizeof(addr) };
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);
	addr.sin_family = AF_INET;

	SOCKET s_listen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(s_listen, (SOCKADDR*)&addr, sizeof_addr);
	listen(s_listen, SOMAXCONN);

	SOCKET new_connection{};
	for (int i = 0; i < 100; i++)
	{
		new_connection = accept(s_listen, (SOCKADDR*)&addr, &sizeof_addr);

		if (new_connection == 0)
			std::cout << "new_connection Error\n";
		else
		{
			std::cout << "Client connected\n";
			Connections[i] = new_connection;
			std::thread thread = std::thread(clietnHandler, Count);
			thread.detach();
			++Count;
		}
	}
	return 0;
}