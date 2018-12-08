#pragma once

#include "ChatServer.h"
#include "ThreadFunc.h"
#include "resource.h"
#include <windows.h>
#include <iostream>
#include <xlocale>
#include <xstring>
#include <codecvt>
#include <fstream>

HWND hTextBox;

void LoadData();
void RestoreData();