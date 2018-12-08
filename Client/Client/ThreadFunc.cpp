#include "stdafx.h"
#include "ChatClient.h"
#include "ThreadFunc.h"

ChatClient gClientObj;

UINT  recMessageThread(LPVOID lParam)
{
	while (1)
	{
		if (gClientObj.recMessagePort())
			break;
	}
	return 0;
}