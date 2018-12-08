#include "stdafx.h"
#include "ChatServer.h"
#include "ThreadFunc.h"
#include "Resource.h"
ChatServer::ChatServer()
{
	cout << "Starting up TCP Chat server\n";
	_isConnected = false;

	WSADATA wsaData;

	sockaddr_in local;

	int wsaret = WSAStartup(0x101, &wsaData);

	if (wsaret != 0)
	{
		return;
	}

	local.sin_family = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port = htons((u_short)8084);

	_socListenClient = socket(AF_INET, SOCK_STREAM, 0);


	if (_socListenClient == INVALID_SOCKET)
	{
		return;
	}


	if (bind(_socListenClient, (sockaddr*)&local, sizeof(local)) != 0)
	{
		return;
	}


	if (listen(_socListenClient, 10) != 0)
	{
		return;
	}

	_isConnected = true;
	return;
}

ChatServer::~ChatServer()
{
	closesocket(_socListenClient);

	WSACleanup();

	if (_clientList.size() != 0)
	{
		for (auto client: _clientList)
		{
			delete client;
		}
	}

	if (_groupchatList.size() != 0)
	{
		for (auto group : _groupchatList)
		{
			delete group;
		}
	}
}

bool ChatServer::isConnected()
{
	return _isConnected;
}

void ChatServer::startListenClient()
{

	sockaddr_in from;
	int fromlen = sizeof(from);

	_socClient = accept(_socListenClient,
		(struct sockaddr*)&from, &fromlen);
	auto packet = new ClientPacket;
	packet->socket = _socClient;
	
	if (_socClient != INVALID_SOCKET)
	{
		SendMessage(_hwnd, WM_COMMAND, ID_USER_CONNECT, 0);
		_clientList.push_back(packet);

	}

	AfxBeginThread(recServerThread, (void *)_socClient);

}



int ChatServer::sendMessageClient(ClientPacket* client, WCHAR* message, int len)
{
	int iStat = 0;

	iStat = send(client->socket, (char*)message, len * 2 + 2, 0);
	if (iStat == -1)
		_clientList.remove(client);
	if (iStat == -1)
		return 1;

	return 0;

}

int ChatServer::recClient(SOCKET recSocket)
{
	WCHAR* message;
	WCHAR temp[4096];
	int iStat;
	int len;
	iStat = recv(recSocket, (char*)temp, 4096, 0);
	list<ClientPacket*>::iterator itl;
	for (itl = _clientList.begin(); itl != _clientList.end(); itl++)
	{
		if ((*itl)->socket == recSocket)
		{
			break;
		}
	}

	if (iStat == -1)
	{
		for (auto gc : _groupchatList)
		{
			gc->username.remove((*itl)->username);
		}
		_clientList.remove((*itl));
		SendMessage(_hwnd, WM_COMMAND, ID_USER_LEAVE, 0);
		return 1;
	}
	else
	{
		message = temp;
		switch (message[0])
		{
		case MessageType::PRIVATE_CHAT:
		{
			/*
			* receive:	message = [FLAG | receiver | NULL | sender | NULL | content | NULL]
			* receive:	message = [FLAG | receiver | NULL | sender | NULL | content | NULL]
			*/

			WCHAR* partner;
			partner = message + 1;


			for (auto client : _clientList)
			{
				if (wcscmp(client->username.c_str(), partner) == 0)
				{
					sendMessageClient(client, (WCHAR*)message, iStat / 2);
					break;
				}
			}
			break;
		}
		case MessageType::GROUP_CHAT:
		{
			/*
			 * receive: message = [FLAG | group name | NULL | sender | NULL | content | NULL]
			 * send:	message = [FLAG | group name | NULL | sender | NULL | content | NULL]
			 */
			WCHAR* groupname;
			len = wcslen(message);
			groupname = message + 1;
			
			sendMessageGroup(groupname, (*itl)->username, message, iStat / 2);
			break;
		}
		case MessageType::END_PRIVATE_CHAT:
		{
			/*
			* receive:	message = [FLAG | receiver | NULL | sender | NULL]
			* send:		message = [FLAG | receiver | NULL | sender | NULL]
			*/
			WCHAR* receiver = message + 1;

			for (auto client : _clientList)
			{
				if (wcscmp(client->username.c_str(), receiver) == 0)
				{
					sendMessageClient(client, (WCHAR*)message, iStat / 2);
					break;
				}
			}
			break;
		}
		case MessageType::END_GROUP_CHAT:
		{
			/*
			* receive:	message = [FLAG | group name | NULL | sender | NULL]
			* send:		message = [FLAG | group name | NULL | sender | NULL]
			*/

			WCHAR* groupname;
			GroupChat* gc = NULL;

			groupname = message + 1;
			for (auto group : _groupchatList)
			{
				// Tìm thấy group chat
				if (wcscmp(group->name.c_str(), groupname) == 0)
				{
					for (auto user : group->username)
					{
						// Những user khác user gửi tin nhắn sẽ được chọn
						// Gửi tin nhắn đến hết các user được chọn
						if (user != (*itl)->username)
						{
							// Tìm ClientPacket của user
							for (auto client : _clientList)
							{
								// Tìm thấy ClientPacket
								if (client->username == user)
								{
									// Gửi tin nhắn đến user
									sendMessageClient(client, message, iStat / 2);
									break;
								}
							}
						}
					}
					gc = group;
					break;
				}
			}
			if (!gc)
			{
				break;
			}
			if (gc->username.size() > 0)
			{
				gc->username.remove((*itl)->username);
			}

			if (gc->username.size() == 0)
			{
				if (_groupchatList.size() > 0)
				{
					_groupchatList.remove(gc);
				}
			}
			break;
		}
		case MessageType::SIGNUP:
		{
			/*
			* receive:	message = [FLAG | user name | NULL | password | NULL]
			* send:		message = [FLAG]
			*/

			WCHAR* username;
			WCHAR* password;

			username = message + 1;
			password = message + wcslen(message) + 1;

			auto user = new User;
			user->username = username;
			user->password = password;
			int result = signUp(user);
			if (result == true)
			{
				message[0] = MessageType::SU_SUCCESS;
				SendMessage(_hwnd, WM_COMMAND, ID_RESTORE, 0);
			}
			else
			{
				message[0] = MessageType::SU_FAILURE;
			}
			sendMessageClient((*itl), (WCHAR*)message, 1);
			break;
		}
		case MessageType::LOGIN:
		{
			/*
			* receive:	message = [FLAG | user name | NULL | password | NULL]
			* send:		message = [FLAG]
			*/

			WCHAR* username;
			WCHAR* password;
			username = message + 1;
			password = message + wcslen(message) + 1;
			User user;
			user.username = username;
			user.password = password;
			int result = logIn(recSocket, user);
			if (result == true)
			{
				message[0] = MessageType::LI_SUCCESS;
			}
			else
			{
				message[0] = MessageType::LI_FAILURE;
			}

			sendMessageClient((*itl), (WCHAR*)message, 1);
			break;
		}
		case MessageType::CREATE_PRIVATE_CHAT:
		{
			/*
			* receive:	message = [FLAG | partner | NULL]
			* send:		message = [FLAG | parter | NULL]	- Succeed
			*					  [FLAG]					- Fail
			*/

			WCHAR* partner;
			partner = message + 1;
			bool result = isOnlineUser(partner);
			if (result == true)
			{

				message[0] = MessageType::C_PC_SUCCESS;
				sendMessageClient((*itl), (WCHAR*)message, iStat / 2);
			}
			else
			{
				message[0] = MessageType::C_PC_FAILURE;
				sendMessageClient((*itl), (WCHAR*)message, 1);
			}
			break;
		}
		case MessageType::CREATE_GROUP_CHAT:
		{
			/*
			* receive:	message = [FLAG | group name | NULL]
			* send:		message = [FLAG | group name | NULL]	- Succeed
			*					  [FLAG]					- Fail
			*/

			WCHAR buffer[150];
			buffer[0] = C_GC_SUCCESS;
			buffer[1] = NULL;
			wcscat(buffer, (*itl)->username.c_str());
			wcscat(buffer, L";");
			wcscat(buffer, message + 1);
			bool result = isOnlineGroup(buffer + 1);
			if (result == false)
			{
				auto gc = new (std::nothrow) GroupChat;
				gc->name = buffer + 1;
				gc->username.push_back((*itl)->username);
				_groupchatList.push_back(gc);
				len = wcslen(buffer);
				sendMessageClient((*itl), (WCHAR*)buffer, len);
			}
			else
			{
				buffer[0] = MessageType::C_GC_FAILURE;
				sendMessageClient((*itl), (WCHAR*)buffer, 1);
			}
			break;
		}
		case MessageType::GC_ADD_USER:
		{
			/*
			* receive:	message = [FLAG | group name | NULL | added user | NULL]
			* send:		message = [FLAG | group name | NULL | added user | NULL]
			*/
			WCHAR* username;
			WCHAR* groupname;
			len = wcslen(message);

			groupname = message + 1;
			username = message + len + 1;

			bool result = isOnlineUser(username);
			if (result == true)
			{
				for (auto group : _groupchatList)
				{
					if (wcscmp(group->name.c_str(), groupname) == 0)
					{
						for (auto user : group->username)
						{
							if (wcscmp(user.c_str(), username) == 0)
							{
								message[0] = MessageType::GC_AU_FAILURE;
								len += group->username.size() + 1;
								sendMessageClient((*itl), (WCHAR*)message, len);
								return 0;
							}
						}
						group->username.push_back(username);

						message[0] = MessageType::GC_AU_SUCCESS;

						len += wcslen(username) + 1;

						sendMessageGroup(groupname, L"user", message, len, true);
						//SendMessageClient((*itl), (WCHAR*)buffer, len);
						return 0;
					}
				}
			}
			else
			{
				message[0] = MessageType::GC_AU_FAILURE;
				sendMessageClient((*itl), (WCHAR*)message, len);
			}
			break;
		}
		case MessageType::SF_ACCEPT:
		case MessageType::SF_CANCEL:
		{
			/*
			* receive:	message = [FLAG | file name | NULL | file size | NULL | sender | NULL| receiver | NULL]
			* send:		message = [FLAG | file name | NULL | file size | NULL | sender | NULL| receiver | NULL]
			*/
			WCHAR* partner;
			partner = message + wcslen(message) + 4;
			partner += wcslen(partner) + 1;
			//buffer[len] = NULL;
			for (auto client : _clientList)
			{
				if (wcscmp(client->username.c_str(), partner) == 0)
				{
					sendMessageClient(client, (WCHAR*)message, iStat / 2);
					break;
				}
			}
			break;
		}
		case MessageType::SEND_FILE:
		{
			/*
			* receive:	message = [FLAG | file name | NULL | file size | NULL | receiver | NULL| sender | NULL]
			* send:		message = [FLAG | file name | NULL | file size | NULL | receiver | NULL| sender | NULL]
			*/
			WCHAR* partner;
			partner = message + wcslen(message) + 4;
	
			//buffer[len] = NULL;
			for (auto client : _clientList)
			{
				if (wcscmp(client->username.c_str(), partner) == 0)
				{
					sendMessageClient(client, (WCHAR*)message, iStat / 2);
					break;
				}
			}
			break;
		}
		case MessageType::FILE_DATA:
		{
			/*
			* receive:	message = [FLAG | file size | NULL | receiver | NULL | sender | NULL | content]
			* send:		message = [FLAG | file size | NULL | receiver | NULL | sender | NULL | content]
			*/

			WCHAR* receiver;
			receiver = message + 3;
			
			//buffer[len] = NULL;
			for (auto client : _clientList)
			{
				if (wcscmp(client->username.c_str(), receiver) == 0)
				{
					sendMessageClient(client, (WCHAR*)message, iStat / 2);
					break;
				}
			}
			break;
		}
		case MessageType::STOP:
		case MessageType::CONTINUE:
		{
			/*
			* receive:	message = [FLAG | receiver | NULL | sender | NULL]
			* send:		message = [FLAG | receiver | NULL | sender | NULL]
			*/
			WCHAR* receiver = message + 1;
			for (auto client : _clientList)
			{
				if (wcscmp(client->username.c_str(), receiver) == 0)
				{
					sendMessageClient(client, (WCHAR*)message, iStat / 2);
					break;
				}
			}
			
			break;
		}
		}
		
		return 0;
	}
	return 0;

}

void ChatServer::setHWND(HWND hwnd)
{
	_hwnd = hwnd;
}

bool ChatServer::signUp(User* user)
{
	for (auto userdata : _userData)
	{
		if (user->username == userdata->username)
		{
			return false;
		}
	}
	_userData.push_back(user);
	return true;
}

bool ChatServer::logIn(SOCKET socket, User user)
{
	for (auto userdata : _userData)
	{
		if (user.username == userdata->username && user.password == userdata->password)
		{
			for (auto client : _clientList)
			{
				if (client->socket == socket)
				{
					client->username = user.username;
					return true;
				}
			}
		}
	}
	return false;
}

bool ChatServer::isOnlineUser(wstring username)
{
	for (auto client : _clientList)
	{
		if (username == client->username)
		{
			return true;
		}
	}
	return false;
}

bool ChatServer::isOnlineGroup(wstring groupname)
{
	for (auto group : _groupchatList)
	{
		if (group->name == groupname)
		{
			return true;
		}
	}
	return false;
}

void ChatServer::sendMessageGroup(wstring groupname, wstring sender, WCHAR* message, int len, bool isSendToSender)
{
	// Tìm group chat trong danh sách
	for (auto group : _groupchatList)
	{
		// Tìm thấy group chat
		if (group->name == groupname)
		{
			// Tim user trong danh sách của group tìm thấy
			for (auto user : group->username)
			{
				// Những user khác user gửi tin nhắn sẽ được chọn
				// Gửi tin nhắn đến hết các user được chọn
				if (user != sender || isSendToSender)
				{
					// Tìm ClientPacket của user
					for (auto client : _clientList)
					{
						// Tìm thấy ClientPacket
						if (client->username == user)
						{
							// Gửi tin nhắn đến user
							sendMessageClient(client, (WCHAR*)message, len);
							break;
						}
					}

				}
			}
			// Đã gửi tin nhắn hết các user - thoát! 
			return;
		}
	}
}

void ChatServer::addUser(User* user)
{
	_userData.push_back(user);
}

list<User*>& ChatServer::getUser()
{
	return _userData;
}
