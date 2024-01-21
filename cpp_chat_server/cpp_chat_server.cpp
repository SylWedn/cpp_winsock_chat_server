#pragma comment(lib, "ws2_32.lib") // win sock
#include <winsock2.h> //using 2nd version of winsock
#include <iostream>


#pragma warning(disable: 4996)

SOCKET Connections[100]; //massive for 100 clients
int Counter = 0; // 0 for first user that connected

enum Packet {
	P_ChatMessage, // type of packet
	P_Test
};

bool ProcessPacket(int index, Packet packettype) {
	switch (packettype) {
	case P_ChatMessage:
	{
		int msg_size;
		recv(Connections[index], (char*)&msg_size, sizeof(int), NULL);
		char* msg = new char[msg_size + 1];
		msg[msg_size] = '\0';
		recv(Connections[index], msg, msg_size, NULL);
		for (int i = 0; i < Counter; i++) {
			if (i == index) { //avoid double send messege 
				continue;
			}
			Packet msgtype = P_ChatMessage;
			send(Connections[i], (char*)&msgtype, sizeof(Packet), NULL);
			send(Connections[i], (char*)&msg_size, sizeof(int), NULL);
			send(Connections[i], msg, msg_size, NULL);
		}

	}
	default:
		std::cout << "Unrecognized packet : " << packettype << std::endl;
		break;
	}
	
	return true;
}

void ClientHandler(int index) {
	Packet packettype;
	while (true) {
		recv(Connections[index], (char*)&packettype, sizeof(Packet), NULL);

		if (ProcessPacket(index, packettype)) {
			break;
		}
	}
	closesocket(Connections[index]);
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
			std::string msg = "Hello. It's my first network program!";
			int msg_size = msg.size();
			Packet msgtype = P_ChatMessage;
			send(newConnection, (char*)&msgtype, sizeof(Packet), NULL);
			send(newConnection, (char*)&msg_size, sizeof(int), NULL);
			send(newConnection, msg.c_str(), msg_size, NULL);

			Connections[i] = newConnection;
			Counter++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, (LPVOID)(i), NULL, NULL); // multithreading

			Packet testpacket = P_Test;
		
			send(newConnection, (char*)&testpacket, sizeof(Packet), NULL);
		}
	}


	system("pause");
	return 0;
	
}