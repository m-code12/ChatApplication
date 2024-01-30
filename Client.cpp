#include<iostream>
#include<WinSock2.h>
#include<WS2tcpip.h>
#include<thread>
#include<iomanip>

using namespace std;

#pragma comment(lib,"ws2_32.lib")


/*
  //STEPS
	//initialize winsock

	//create socket

	//connect to server 

	//send/recv

	//close the socket
*/


bool Initialize() {
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}

void sendMessage(SOCKET s) {
	cout << "Enter your name: " << endl;
	string name;
	getline(cin, name);
	string message;
	
	while (1) {
		getline(cin, message);
		string msg = name + ":" + message;
		int bytes = send(s, msg.c_str(), msg.length(), 0);
		if (bytes == SOCKET_ERROR) {
			cout << "Error sending the message!" << endl;
			break;
		}
		if (message == "quit") {
			cout << "Stopping the program!" << endl;
			break;
		}

	}

	closesocket(s);
	WSACleanup();

}

void receiveMessage(SOCKET s) {
	char buffer[4096];
	int recvlength;
	string msg = "";
	while (1) {
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0) {
			cout << "Error occured!" << endl;
			break;
		}
		else {
			msg = string(buffer, recvlength);
			cout << "                           " + msg << endl;
		}
	}
	//closesocket(s);
	//WSACleanup();
}


int main() {
	if (!Initialize()) {
		cout << "Initialize Failed" << endl;
		return 1;
	}

	cout << "Client Program started" << endl;
	SOCKET s;   // we need both ip and port
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET) {
		cout << "Invalid Socket created" << endl;
		return 1;
	}

	int port = 6969;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//bind 
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	if(connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR) {
		cout << "Not able to connect to server" << endl;
		closesocket(s);
		WSACleanup();
	}

	cout << "Successfully connected to server !" << endl;

	thread senderThread(sendMessage, s);
	thread receiverThread(receiveMessage, s);

	senderThread.join();
	receiverThread.join();

	

	return 0;
}
