#pragma once
#define _AFXDLL
#include "Definition.h"

class ChatServer
{
public:
	ChatServer();
	~ChatServer();
	bool isConnected();
	void startListenClient();
	int sendMessageClient(ClientPacket* client, WCHAR* message, int len);
	int recClient(SOCKET recSocket);
	void setHWND(HWND hwnd);
	bool signUp(User* user);
	bool logIn(SOCKET socket, User user);
	bool isOnlineUser(wstring username);
	bool isOnlineGroup(wstring groupname);
	void sendMessageGroup(wstring groupname, wstring sender, WCHAR* message, int len, bool isSendToSender = false);
	void addUser(User* user);
	list<User*>& getUser();
private:
	bool _isConnected;
	int _serverPort;
	list<ClientPacket*> _clientList;
	SOCKET _socClient;
	SOCKET _socListenClient;
	HWND _hwnd;
	list<User*> _userData;
	list<GroupChat*> _groupchatList;
};

