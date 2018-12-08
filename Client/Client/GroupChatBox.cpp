#include "stdafx.h"
#include "GroupChatBox.h"
#include "resource.h"

GroupChatBox * GroupChatBox::create(HWND hParent, HINSTANCE hInst, Point pos, Size size, wstring groupname)
{
	auto cb = new (std::nothrow) GroupChatBox();
	cb->_hInst = hInst;
	cb->_hParent = hParent;
	cb->_position = pos;
	cb->_size = size;
	cb->_groupname = groupname;

	groupname.erase(0, groupname.find_first_of(L';') + 1);
	cb->_hWnd = CreateWindowEx(0, L"chatbox", (groupname + L" - Group chat").c_str(), WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE, pos.X, pos.Y, size.Width, size.Height, NULL, NULL, hInst, NULL);

	cb->_hAddUserBox = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER, size.Width*0.05, size.Height*0.03, size.Width*0.65, size.Height*0.05, cb->_hWnd, 0, hInst, 0);
	cb->_hAdd = CreateWindow(L"button", L"Add", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, size.Width*0.75, size.Height*0.03, size.Width*0.15, size.Height*0.05, cb->_hWnd, (HMENU)IDC_ADD, hInst, 0);

	cb->_hMessageBox = CreateWindowEx(WS_EX_RIGHTSCROLLBAR, L"edit", L"", WS_VISIBLE | WS_VSCROLL | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY | WS_BORDER, size.Width*0.05, size.Height*0.1, size.Width*0.9, size.Height*0.55, cb->_hWnd, 0, hInst, 0);
	cb->_hTextBox = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER, size.Width*0.05, size.Height*0.7, size.Width*0.65, size.Height*0.15, cb->_hWnd, 0, hInst, 0);
	cb->_hSend = CreateWindow(L"button", L"Send", WS_VISIBLE | WS_CHILD | BS_OWNERDRAW, size.Width*0.75, size.Height*0.7, size.Width*0.15, size.Height*0.15, cb->_hWnd, (HMENU)IDC_SEND_GROUP, hInst, 0);
	return cb;
}

void GroupChatBox::setFont(HFONT hFont)
{
	SetWindowFont(_hMessageBox, hFont, true);
	SetWindowFont(_hTextBox, hFont, true);
}

wstring GroupChatBox::getGroupName()
{
	return _groupname;
}

void GroupChatBox::receiveMessage(WCHAR * message)
{
	WCHAR buffer[10000];
	GetWindowText(_hMessageBox, buffer, 10000);
	wcscat(buffer, L"\r\n\r\n");
	wcscat(buffer, message);
	SetWindowText(_hMessageBox, buffer);

	SendMessageA(_hMessageBox, EM_SETSEL, 0, -1);
	SendMessageA(_hMessageBox, EM_SETSEL, -1, -1);
	SendMessageA(_hMessageBox, EM_SCROLLCARET, 0, 0);
}

int GroupChatBox::onPressBtnSend(WCHAR* message)
{
	WCHAR bufferText[1000];
	WCHAR bufferMess[10000];
	message[0] = MessageType::GROUP_CHAT;
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
	wcscat(message, _groupname.c_str());

	len = _groupname.size() + 1;
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

	return len;
}

int GroupChatBox::onPressBtnAdd(WCHAR * message)
{
	WCHAR username[100];
	message[0] = MessageType::GC_ADD_USER;
	message[1] = NULL;
	GetWindowText(_hAddUserBox, username, 50);
	SetWindowText(_hAddUserBox, L"");
	if (username == NULL)
	{
		return -1;
	}
	wcscat(message, _groupname.c_str());
	int len = wcslen(message);
	len++;
	int i;
	for (i = 0; username[i] != NULL; i++)
	{
		message[len + i] = username[i];
	}
	len += i;
	message[len] = NULL;
	return len;
}

void GroupChatBox::onUserLeft(WCHAR* user)
{
	WCHAR buffer[10000];
	GetWindowText(_hMessageBox, buffer, 10000);
	wcscat(buffer, L"\r\n\r\n\t\t");
	wcscat(buffer, user);
	wcscat(buffer, L" has left this group chat.\r\n");
	SetWindowText(_hMessageBox, buffer);

	SendMessageA(_hMessageBox, EM_SETSEL, 0, -1);
	SendMessageA(_hMessageBox, EM_SETSEL, -1, -1);
	SendMessageA(_hMessageBox, EM_SCROLLCARET, 0, 0);
}

void GroupChatBox::onUserJoin(WCHAR* user)
{
	WCHAR buffer[10000];
	GetWindowText(_hMessageBox, buffer, 10000);
	wcscat(buffer, L"\r\n\r\n\t\t");
	wcscat(buffer, user);
	wcscat(buffer, L" has joined this group chat.\r\n");
	SetWindowText(_hMessageBox, buffer);

	SendMessageA(_hMessageBox, EM_SETSEL, 0, -1);
	SendMessageA(_hMessageBox, EM_SETSEL, -1, -1);
	SendMessageA(_hMessageBox, EM_SCROLLCARET, 0, 0);
}

void GroupChatBox::setUsername(const wstring & username)
{
	_username = username;
}

HWND GroupChatBox::getHWND()
{
	return _hWnd;
}

GroupChatBox::GroupChatBox()
{
}


GroupChatBox::~GroupChatBox()
{
}
