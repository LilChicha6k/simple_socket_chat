#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <string>
#pragma comment (lib, "ws2_32")
#pragma warning (disable: 4996)

SOCKET s_connect;

void catchmsg()
{
	int msg_size{};
	int name_size{};
	while (true)
	{
		recv(s_connect, (char*)&name_size, sizeof(int), NULL);
		char* name = new char[name_size + 1];
		name[name_size] = '\0';
		recv(s_connect, name, name_size, NULL);
		std::cout << '\n' << name << " Send:\n";
		recv(s_connect, (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(s_connect, msg, msg_size, NULL);
		std::cout << msg << '\n';
		delete[] msg;
		delete[] name;
	}
}

int main(int argc, char* argv[])
{
	WSADATA wsa_data;
	WORD DLL_version = MAKEWORD(2, 1);

	if (WSAStartup(DLL_version, &wsa_data) != 0)
	{
		std::cout << "Error WSASStartup" << '\n';
		exit(EXIT_FAILURE);
	}

	SOCKADDR_IN addr;
	int sizeof_addr{ sizeof(addr) };
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(8080);
	addr.sin_family = AF_INET;

	s_connect = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(s_connect, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		std::cout << "Error connect to server\n";
		return 1;
	}
	std::cout << "Connected to server!\n";

	std::cout << "Enter your name:";
	std::string name{};
	std::getline(std::cin, name);
	int name_length = name.size();

	std::thread t1(catchmsg);
	t1.detach();

	std::string msgl;
	while (true)
	{
		std::getline(std::cin, msgl);
		int msgl_size = msgl.size();
		if (msgl_size <= 0)
			continue;
		send(s_connect, (char*)&name_length, sizeof(int), NULL);
		send(s_connect, name.c_str(), name_length, NULL);
		send(s_connect, (char*)&msgl_size, sizeof(int), NULL);
		send(s_connect, msgl.c_str(), msgl_size, NULL);
	}


	return 0;
}