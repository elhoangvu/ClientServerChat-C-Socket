#pragma once
#define _AFXDLL
#include <Afxwin.h>
#include <stdio.h>
#include <winsock2.h>
#include <conio.h> 
#include <iostream>
#include <windows.h>
#include "resource.h"
using namespace std;


class ChatClient
{
public:
	ChatClient();
	~ChatClient();
	void init(string ipAddress, int port);
	int sendMessagePort(WCHAR* message, int len);
	int recMessagePort();
	bool isConnected();
	void setHWND(HWND hwnd);
	void setUsername(wstring username);
	wstring& getUsername();
private:
	bool _isConnected; // true - connected false - not connected
	string _serverIPAddress;
	int _serverPort;
	SOCKET _connect; // socket connected to server
	HWND _hwnd;
	wstring _username;
};
