#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<tchar.h>
#include<thread>
#include<vector>

using namespace std;


#pragma comment(lib,"ws2_32.lib")


/*
  //STEPS 
	//innitialize winsock library

	//create the socket

	//get ip and port

	//bind ip/port to the socket

	//listen to the socket

	//accept

	//recv and send

	//close the socket 

	//cleanup

*/


bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2,2) , &data) == 0;
}


void InteractWithClient(SOCKET clientSocket,vector<SOCKET>& clients) {
	//here we will do send and recv operation on another thread so that it will free the thread for the server to listen up to other client connection request
	cout << "client Connected" << endl;

	while (1) {

		char buffer[4096];

		int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

		if (bytesrecvd <= 0) {
			cout << "Client Disconnected!";
			break;
		}

		string message(buffer, bytesrecvd);
		cout << "Message from client : " << message << endl;

		for (auto client : clients) {
			if (client != clientSocket) {
				send(client, message.c_str(), message.length(), 0);
			}
		}



	}

	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end()) {
		clients.erase(it);
	}

	closesocket(clientSocket);

}

int main() {
	if (!Initialize()) {
		cout << "Initialization failed! " << endl;
		return 1;
	}

	cout << "Server Program" << endl;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET) {
		cout << "Socket Creation Failed!" << endl;
		return 1;
	}


	//Creating address Structure

	int port = 6969;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//Convert ip add (0.0.0.0) to binary , put it inside the sin family in binary format

	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1) {
		cout << "Setting address structure failed!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//Bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Binding failed!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	//listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {
		cout << "Listen failed!" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server started listening on port: " << port << endl;

	vector<SOCKET> clients;

	while (1) {

		//accept
		SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
		if (clientSocket == INVALID_SOCKET) {
			cout << "invalid client socket" << endl;
		}

		clients.push_back(clientSocket);
		thread t1(InteractWithClient, clientSocket,std::ref(clients));
		t1.detach();

		
	
	}
	
	

	closesocket(listenSocket);

	WSACleanup();

	return 0;
}
