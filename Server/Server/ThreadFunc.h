#pragma once
#include <windows.h>
extern ChatServer gServerObj;

UINT  listenServerThread(LPVOID lParam);
UINT  recServerThread(LPVOID lParam);