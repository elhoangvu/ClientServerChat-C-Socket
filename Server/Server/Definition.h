#pragma once
#include <Afxwin.h>
#include <stdio.h>
#include <winsock2.h>
#include <conio.h> 
#include <list>
#include <iostream>
#include <windows.h>
using namespace std;

struct ClientPacket
{
	SOCKET socket;
	wstring username;
};

struct User
{
	wstring username;
	wstring password;
};

struct GroupChat
{
	wstring name;
	list<wstring> username;
};

enum MessageType
{
	PRIVATE_CHAT = 1,
	END_PRIVATE_CHAT,
	GROUP_CHAT,
	END_GROUP_CHAT,
	SIGNUP,
	SU_SUCCESS,
	SU_FAILURE,
	LOGIN,
	LI_SUCCESS,
	LI_FAILURE,
	CREATE_PRIVATE_CHAT,
	C_PC_SUCCESS,
	C_PC_FAILURE,
	CREATE_GROUP_CHAT,
	C_GC_SUCCESS,
	C_GC_FAILURE,
	GC_ADD_USER,
	GC_AU_SUCCESS,
	GC_AU_FAILURE,
	SEND_FILE,
	SF_ACCEPT,
	SF_CANCEL,
	FILE_DATA,
	CONTINUE,
	STOP,
};
