#include "stdafx.h"
#include "ChatClient.h"


ChatClient::ChatClient()
{
	_isConnected = false;

}

void ChatClient::init(string ipAddress, int port)
{

	_serverIPAddress = ipAddress;
	_serverPort = port;
	struct hostent *hp;
	unsigned int addr;
	struct sockaddr_in server;
	

	WSADATA wsaData;

	int wsaret = WSAStartup(0x101, &wsaData);


	if (wsaret != 0)
	{
		return;
	}

	_connect = socket(AF_INET, SOCK_STREAM, 0);
	if (_connect == INVALID_SOCKET)
		return;

	addr = inet_addr(_serverIPAddress.c_str());
	hp = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	
	if (hp == NULL)
	{
		closesocket(_connect);
		return;
	}

	server.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(_serverPort);
	if(connect(_connect,(struct sockaddr*)&server,sizeof(server)))
	{
		closesocket(_connect);
		return;	
	}
	_isConnected = true;
	return;
}

ChatClient::~ChatClient()
{
	if(_isConnected)
		closesocket(_connect);
}

int ChatClient::sendMessagePort(WCHAR* message, int len)
{
		int iStat = 0;

		iStat = send(_connect, (char*)message, len * 2 + 2, 0);
		if(iStat == -1)
			return 1;

		return 0;

}

int ChatClient::recMessagePort()
{

		char acRetData[4096];
		int iStat = 0;

		iStat = recv(_connect, acRetData, 4096, 0);
		if(iStat == -1)
			return 1;
		SendMessage(_hwnd, WM_COMMAND, (WPARAM)IDC_RECEIVE, (LPARAM)acRetData);

		return 0;

}

bool ChatClient::isConnected()
{
	return _isConnected;
}

void ChatClient::setHWND(HWND hwnd)
{
	_hwnd = hwnd;
}

void ChatClient::setUsername(wstring username)
{
	_username = username;
}

wstring & ChatClient::getUsername()
{
	return _username;
}
