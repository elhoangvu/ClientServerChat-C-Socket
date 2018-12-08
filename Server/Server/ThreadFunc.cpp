#include "stdafx.h"
#include "ChatServer.h"
#include "ThreadFunc.h"
ChatServer gServerObj;

UINT  recServerThread(LPVOID lParam)
{
	SOCKET recSocket = (SOCKET)lParam;
	while (1)
	{
		if (gServerObj.recClient(recSocket))
			break;
	}
	return 0;
}

UINT  listenServerThread(LPVOID lParam)
{
	while (1)
		gServerObj.startListenClient();
	return 0;
}