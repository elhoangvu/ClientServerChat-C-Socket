#pragma once
#include <string>
#include <new>
#include <ObjIdl.h>
#include <gdiplus.h>
#include <Windows.h>
#include "Definition.h"
#include <windowsx.h>

#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
using namespace std;

class GroupChatBox
{
private:
	HWND _hParent;
	HINSTANCE _hInst;
	HWND _hWnd;
	HWND _hAddUserBox;
	HWND _hAdd;
	HWND _hMessageBox;
	HWND _hTextBox;
	HWND _hSend;
	wstring _username;
	wstring _groupname;
	Point _position;
	Size _size;
public:
	static GroupChatBox* create(HWND hParent, HINSTANCE hInst, Point pos, Size size, wstring groupname);
	void setFont(HFONT hFont);
	wstring getGroupName();
	void receiveMessage(WCHAR* message);
	int onPressBtnSend(WCHAR* messageBuf);
	int onPressBtnAdd(WCHAR* addeduserBuf);
	void onUserLeft(WCHAR* user);
	void onUserJoin(WCHAR* user);
	void setUsername(const wstring& username);
	HWND getHWND();
	GroupChatBox();
	~GroupChatBox();
};

