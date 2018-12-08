#pragma once

#include "ChatClient.h"
#include "ThreadFunc.h"
#include "resource.h"
#include <windows.h>
#include <windowsx.h>
#include "PrivateChatBox.h"
#include "GroupChatBox.h"
#include <list>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HWND hTextBox;
HWND hMessageBox;
HWND hSend;

HWND hSignUp;
HWND hLogIn;
HWND hUsername;
HWND hPassword;

HWND hInvitedUsername;
HWND hGroupName;
HWND hPrivateChat;
HWND hGroupChat;
HWND hCreate;
HWND hwnd;
HFONT hFont;

int gCurScene;
list<PrivateChatBox*> gPrivateChatBoxList;
list<GroupChatBox*> gGroupChatBoxList;
enum MessageType gCurMessageType;

GdiplusStartupInput gdiplusStartupInput;
ULONG_PTR gdiplusToken;

LPWSTR convertSize(DWORD size);
bool myRegClass(WNDPROC lpfnWndProc, WCHAR *szClassName, HINSTANCE hInst);
BOOL myCreateOpenFile(HWND hwnd, WCHAR* filename);
BOOL myCreateSaveFile(HWND hwnd, WCHAR* filename);

LRESULT CALLBACK ChatBoxProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void OnPaint(HWND hwnd);
void OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT * lpDrawItem);