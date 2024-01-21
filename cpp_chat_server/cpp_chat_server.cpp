#pragma comment(lib, "ws2_32.lib") // win sock
#include <winsock2.h> //using 2nd version of winsock
#include <iostream>

#pragma warning(disable: 4996)

SOCKET Connections[100]; //massive for 100 clients
int Counter = 0; // 0 for first user that connected

void ClientHandler(int index) {
	char msg[256];
	while (true) {
		recv(Connections[index], msg, sizeof(msg), NULL);
		for (int i = 0; i < Counter; i++) {
			if (i == index) { //avoid double send messege 
				continue;
			}

			send(Connections[i], msg, sizeof(msg), NULL);
		}
	}
}

int main(int arg, char* argv[]) {  //check if lib loaded 
	//WSASTARTUP
	WSADATA wsaData;
	WORD DLLVersion = MAKEWORD(2, 1); //lib version
	if (WSAStartup(DLLVersion, &wsaData) != 0) { // link to struc wsadata
	std::cout << "Error" << std::endl;
	exit(1);
	}


	SOCKADDR_IN addr; //sockaddr is for saving address // name addr
	int sizeofaddr = sizeof(addr); // sizeoff operator that counts size or datatype in bites
	addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //sin_addr is structure of sockaddr_in
	addr.sin_port = htons(1111); //port 
	addr.sin_family = AF_INET; //internet protocol AF_INET

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	bind(sListen, (SOCKADDR*)&addr, sizeof(addr));
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	for (int i = 0; i < 100; i++) {
		newConnection = accept(sListen, (sockaddr*)&addr, &sizeofaddr);

		if (newConnection == 0) {
			std::cout << "Error #2\n";
		}
		else {
			std::cout << "Client Connected!\n";
			char msg[256] = "Hello. It's my first network program!";
			send(newConnection, msg, sizeof(msg), NULL); 

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL); // multithreading
		}
	}


	system("pause");
	return 0;
	
}