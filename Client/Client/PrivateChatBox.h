#pragma once
#include <string>
#include <new>
#include <ObjIdl.h>
#include <gdiplus.h>
#include <Windows.h>
#include "Definition.h"
#include <Shellapi.h>
#include <windowsx.h>
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;
using namespace std;

class PrivateChatBox
{
private:
	HWND _hParent;
	HINSTANCE _hInst;
	HWND _hWnd;
	HWND _hMessageBox;
	HWND _hTextBox;
	HWND _hSend;
	HWND _hAttach;
	HANDLE _hSentFile;
	HANDLE _hReceiveFile;
	wstring _username;
	wstring _partnerUsername;
	wstring _filename;
	Point _position;
	Size _size;
	bool _isEndChat;
	DWORD _sentFileSize;
	DWORD _recFileSize;
	DWORD _totalRecSize;
public:
	static PrivateChatBox* create(HWND hParent, HINSTANCE hInst, Point pos, Size size, wstring partner);
	wstring getPartner();
	void setFont(HFONT hFont);
	void receiveMessage(WCHAR* message);
	int onPressBtnSend(WCHAR* messageBuf);
	void onPressBtnAttach(WCHAR* filename);
	void onRefuseReceiveFile();
	void onAcceptReceiveFile();
	DWORD onSendFile(WCHAR* message);
	void onStop();
	void preReceiveFile(WCHAR* filename, DWORD filesize);
	BOOL receiveFile(WCHAR* buffer, DWORD num);
	void onEndChat();
	void setUsername(const wstring& username);
	HWND getHWND();
	DWORD getSizeAndOpenFile(WCHAR* filename);
	DWORD getSentFileSize();
	DWORD getRecFileSize();
	HANDLE getHandleRecFile();
	HANDLE getHandleSendFile();
	wstring getUsername();
	PrivateChatBox();
	~PrivateChatBox();
};

