#include "stdafx.h"
#include "PrivateChatBox.h"
#include "resource.h"

PrivateChatBox * PrivateChatBox::create(HWND hParent, HINSTANCE hInst, Point pos, Size size, wstring partner)
{
	auto cb = new (std::nothrow) PrivateChatBox();
	cb->_hWnd = CreateWindowEx(0, L"chatbox", (partner + L" - Private Chat").c_str(), WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, pos.X, pos.Y, size.Width, size.Height, NULL, NULL, hInst, NULL);
	cb->_hInst = hInst;
	cb->_hParent = hParent;
	cb->_position = pos;
	cb->_size = size;
	cb->_partnerUsername = partner;
	cb->_hMessageBox = CreateWindowEx(WS_EX_RIGHTSCROLLBAR, L"edit", L"", WS_VISIBLE | WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY | WS_BORDER, size.Width*0.025, size.Height*0.025, size.Width*0.925, size.Height*0.635, cb->_hWnd, (HMENU)1000, hInst, 0);
	cb->_hTextBox = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER, size.Width*0.025, size.Height*0.7, size.Width*0.55, size.Height*0.15, cb->_hWnd, 0, hInst, 0);
	cb->_hSend = CreateWindow(L"button", L"Send", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, size.Width*0.6, size.Height*0.7, size.Width*0.15, size.Height*0.15, cb->_hWnd, (HMENU)IDC_SEND, hInst, 0);
	cb->_hAttach = CreateWindow(L"button", L"Attach", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, size.Width*0.775, size.Height*0.7, size.Width*0.15, size.Height*0.15, cb->_hWnd, (HMENU)IDC_ATTACH, hInst, 0);

	return cb;
}


wstring PrivateChatBox::getPartner()
{
	return _partnerUsername;
}

void PrivateChatBox::setFont(HFONT hFont)
{
	SetWindowFont(_hMessageBox, hFont, true);
	SetWindowFont(_hTextBox, hFont, true);
}

void PrivateChatBox::receiveMessage(WCHAR * message)
{
	WCHAR buffer[10000];
	GetWindowText(_hMessageBox, buffer, 10000);
	wcscat(buffer, L"\r\n\r\n$[");
	wcscat(buffer, _partnerUsername.c_str());
	wcscat(buffer, L"]:\r\n");
	wcscat(buffer, message);
	SetWindowText(_hMessageBox, buffer);

	SendMessageA(_hMessageBox, EM_SETSEL, 0, -1);
	SendMessageA(_hMessageBox, EM_SETSEL, -1, -1);
	SendMessageA(_hMessageBox, EM_SCROLLCARET, 0, 0);
}

void PrivateChatBox::preReceiveFile(WCHAR * filename, DWORD filesize)
{
	_filename = filename;
	_hReceiveFile = CreateFile(filename, // open file
		GENERIC_WRITE, // open for reading
		0, // do not share
		NULL, // no security
		OPEN_ALWAYS, // always open
		FILE_ATTRIBUTE_NORMAL, // normal file
		NULL); // no attr. template

	if (_hReceiveFile == INVALID_HANDLE_VALUE) 
	{
		return;  // process error
	}
	_recFileSize = filesize;
}

BOOL PrivateChatBox::receiveFile(WCHAR * buffer, DWORD num)
{
	DWORD dwBytesWrite;
	BOOL result = WriteFile(_hReceiveFile, buffer, num, &dwBytesWrite, NULL);
	if (!result)
	{
		MessageBox(_hWnd, L"Failed write", L"Error", 0);
	}
	_totalRecSize += dwBytesWrite;

	if (_totalRecSize >= _recFileSize)
	{
		CloseHandle(_hReceiveFile);
		_totalRecSize = 0;
		_recFileSize = 0;
		int result = MessageBox(_hWnd, L"Receive the file successfully.\n\rDo you want to open it?", L"Receive the file successfully", MB_YESNO);
		if (result == IDYES)
		{
			auto result2 = ShellExecute(_hWnd, L"open", _filename.c_str(), NULL, NULL, SW_SHOWNORMAL);
			if ((LONG)result2 == ERROR_FILE_NOT_FOUND || (LONG)result2 == ERROR_PATH_NOT_FOUND)
			{
				MessageBox(_hWnd, L"File not exist", 0, 0);
			}
		}
		_filename.clear();
		return false;
	}
	return true;
}

int PrivateChatBox::onPressBtnSend(WCHAR* message)
{
	WCHAR bufferText[1000];
	WCHAR bufferMess[10000];
	message[0] = MessageType::PRIVATE_CHAT;
	message[1] = NULL;
	GetWindowText(_hTextBox, bufferText, 1000);
	SetWindowText(_hTextBox, L"");
	if (bufferText[0] == NULL)
	{
		return -1;
	}

	WCHAR* content;
	WCHAR* sender;
	int len;
	wcscat(message, _partnerUsername.c_str());

	len = _partnerUsername.size() + 1;
	sender = message + ++len;
	wcscpy(sender, _username.c_str());
	len += _username.size() + 1;

	content = message + len;
	wcscpy(content, bufferText);
	len += wcslen(bufferText);

	GetWindowText(_hMessageBox, bufferMess, 10000);
	wcscat(bufferMess, L"\r\n\r\n[You]:\r\n");
	wcscat(bufferMess, bufferText);
	SetWindowText(_hMessageBox, bufferMess);

	SendMessageA(_hMessageBox, EM_SETSEL, 0, -1);
	SendMessageA(_hMessageBox, EM_SETSEL, -1, -1);
	SendMessageA(_hMessageBox, EM_SCROLLCARET, 0, 0);

	if (_isEndChat)
	{

		return -1;
	}

	return len;
}

void PrivateChatBox::onPressBtnAttach(WCHAR * filename)
{
	if (_hSentFile != NULL)
	{
		MessageBox(_hWnd, L"You are sending a file. Wait it's done!", 0, 0);
	}
}

void PrivateChatBox::onRefuseReceiveFile()
{
	CloseHandle(_hSentFile);
	_hSentFile = NULL;
}

void PrivateChatBox::onAcceptReceiveFile()
{

}

DWORD PrivateChatBox::onSendFile(WCHAR * message)
{

	/*
	* send:		message = [FLAG | file size | NULL | receive | NULL | sender | NULL | content]
	*/
	int total = 0;
	DWORD dwBytesRead = 0;

	WCHAR buffer[600];
	BOOL result = ReadFile(_hSentFile, buffer, 1024, &dwBytesRead, NULL);
	if (dwBytesRead == 0)
	{
		return 0;
	}
	message[0] = MessageType::FILE_DATA;
	int len = 1;
	message[len++] = dwBytesRead;
	message[len++] = NULL;
	WCHAR* partner;
	WCHAR* own;
	partner = message + len;
	wcscpy(partner, _partnerUsername.c_str());
	len += _partnerUsername.size() + 2;
	own = message + len;
	wcscpy(own, _username.c_str());
	len += _username.size() + 2;

	int i;
	if (dwBytesRead % 2 != 0)
	{
		dwBytesRead++;
	}
	int wsize = dwBytesRead / 2;

	for (i = 0; i < wsize; i++)
	{
		message[len + i] = buffer[i];
	}
	len += wsize;
	return len;
}

void PrivateChatBox::onStop()
{
	onRefuseReceiveFile();
}

void PrivateChatBox::onEndChat()
{
	WCHAR buffer[10000];
	GetWindowText(_hMessageBox, buffer, 10000);
	wcscat(buffer, L"\r\n\r\n\tThis conversation was ended by your partner.\r\n");
	SetWindowText(_hMessageBox, buffer);

	SendMessageA(_hMessageBox, EM_SETSEL, 0, -1);
	SendMessageA(_hMessageBox, EM_SETSEL, -1, -1);
	SendMessageA(_hMessageBox, EM_SCROLLCARET, 0, 0);
	_isEndChat = true;
}

void PrivateChatBox::setUsername(const wstring& username)
{
	_username = username;
}

HWND PrivateChatBox::getHWND()
{
	return _hWnd;
}


DWORD PrivateChatBox::getSizeAndOpenFile(WCHAR* filename)
{
	_hSentFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (_hSentFile == INVALID_HANDLE_VALUE)
	{
		return 0;
	}

	DWORD highSize;
	_sentFileSize = GetFileSize(_hSentFile, &highSize);
	//CloseHandle(hFile);
	return _sentFileSize;
}

DWORD PrivateChatBox::getSentFileSize()
{
	return _sentFileSize;
}

DWORD PrivateChatBox::getRecFileSize()
{
	return _recFileSize;
}

HANDLE PrivateChatBox::getHandleRecFile()
{
	return _hReceiveFile;
}

HANDLE PrivateChatBox::getHandleSendFile()
{
	return _hSentFile;
}

wstring PrivateChatBox::getUsername()
{
	return _username;
}

PrivateChatBox::PrivateChatBox()
{
	_isEndChat = false;
	_hSentFile = NULL;
	_hReceiveFile = NULL;
	_totalRecSize = 0;
	_recFileSize = 0;
}


PrivateChatBox::~PrivateChatBox()
{
}
