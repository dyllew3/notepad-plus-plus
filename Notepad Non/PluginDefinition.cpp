//this file is part of notepad++
//Copyright (C)2003 Don HO <donho@altern.org>
//
//This program is free software; you can redistribute it and/or
//modify it under the terms of the GNU General Public License
//as published by the Free Software Foundation; either
//version 2 of the License, or (at your option) any later version.
//
//This program is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with this program; if not, write to the Free Software
//Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

#include "PluginDefinition.h"
#include "menuCmdID.h"

//
// put the headers you need here
//
#include <stdlib.h>
#include <time.h>
#include <shlwapi.h>
#include "GoToLineDlg.h"

const TCHAR sectionName[] = TEXT("Insert Extesion");
const TCHAR keyName[] = TEXT("doCloseTag");
const TCHAR configFileName[] = TEXT("pluginDemo.ini");

DemoDlg _goToLine;

#ifdef UNICODE 
	#define generic_itoa _itow
#else
	#define generic_itoa itoa
#endif

FuncItem funcItem[nbFunc];

//
// The data of Notepad++ that you can use in your plugin commands
//
NppData nppData;


TCHAR iniFilePath[MAX_PATH];
bool doCloseTag = false;

#define DOCKABLE_DEMO_INDEX 15

//
// Initialize your plugin data here
// It will be called while plugin loading   
void pluginInit(HANDLE hModule)
{
	// Initialize dockable demo dialog
	_goToLine.init((HINSTANCE)hModule, NULL);
}

//
// Here you can do the clean up, save the parameters (if any) for the next session
//
void pluginCleanUp()
{
	::WritePrivateProfileString(sectionName, keyName, doCloseTag?TEXT("1"):TEXT("0"), iniFilePath);
}

//
// Initialization of your plugin commands
// You should fill your plugins commands here
void commandMenuInit()
{
	//
	// Firstly we get the parameters from your plugin config file (if any)
	//

	// get path of plugin configuration
	::SendMessage(nppData._nppHandle, NPPM_GETPLUGINSCONFIGDIR, MAX_PATH, (LPARAM)iniFilePath);

	// if config path doesn't exist, we create it
	if (PathFileExists(iniFilePath) == FALSE)
	{
		::CreateDirectory(iniFilePath, NULL);
	}

	// make your plugin config file full file path name
	PathAppend(iniFilePath, configFileName);

	// get the parameter value from plugin config
	doCloseTag = (::GetPrivateProfileInt(sectionName, keyName, 0, iniFilePath) != 0);




	// Shortcut :
	// Following code makes the first command
	// bind to the shortcut Alt-F
	ShortcutKey *shKey = new ShortcutKey;
	shKey->_isAlt = true;
	shKey->_isCtrl = false;
	shKey->_isShift = false;
	shKey->_key = 0x46; //VK_F



	ShortcutKey *pShKey = new ShortcutKey;
	pShKey->_isAlt = true;
	pShKey->_isCtrl = false;
	pShKey->_isShift = false;
	pShKey->_key = 0x51; //VK_Q
	setCommand(9, TEXT("Close HTML/XML tag automatically"), insertHtmlCloseTag, pShKey, doCloseTag);

}


//
// Here you can do the clean up (especially for the shortcut)
//
void commandMenuCleanUp()
{

}

void hello()
{
    // Open a new document
    ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return;
    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;

    // Say hello now :
    // Scintilla control has no Unicode mode, so we use (char *) here
    ::SendMessage(curScintilla, SCI_SETTEXT, 0, (LPARAM)"Hello, Notepad++!");
}

static DWORD WINAPI threadZoomer(void *)
{
    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return FALSE;

    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
    
    int currentZoomLevel = (int)::SendMessage(curScintilla, SCI_GETZOOM, 0, 0);

    int i = currentZoomLevel;
	for (int j = 0 ; j < 4 ; j++)
	{	
		for ( ; i >= -10 ; i--)
		{
			::SendMessage(curScintilla, SCI_SETZOOM, i, 0);
			Sleep(30);
		}
		Sleep(100);
		for ( ; i <= 20 ; i++)
		{
			Sleep(30);
			::SendMessage(curScintilla, SCI_SETZOOM, i, 0);
		}
		Sleep(100);
	}

    Sleep(100);
    for ( ; i >= currentZoomLevel ; i--)
    {
        Sleep(30);
        ::SendMessage(curScintilla, SCI_SETZOOM, i, 0);
    }
    return TRUE;
};

void helloFX()
{
    hello();
    HANDLE hThread = ::CreateThread(NULL, 0, threadZoomer, 0, 0, NULL);
    ::CloseHandle(hThread);
}


//
// This function help you to initialize your plugin commands
//
bool setCommand(size_t index, TCHAR *cmdName, PFUNCPLUGINCMD pFunc, ShortcutKey *sk, bool check0nInit) 
{
    if (index >= nbFunc)
        return false;

    if (!pFunc)
        return false;

    lstrcpy(funcItem[index]._itemName, cmdName);
    funcItem[index]._pFunc = pFunc;
    funcItem[index]._init2Check = check0nInit;
    funcItem[index]._pShKey = sk;

    return true;
}

static DWORD WINAPI threadTextPlayer(void *text2display)
{
	// Open a new document
    ::SendMessage(nppData._nppHandle, NPPM_MENUCOMMAND, 0, IDM_FILE_NEW);

    // Get the current scintilla
    int which = -1;
    ::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&which);
    if (which == -1)
        return FALSE;

    HWND curScintilla = (which == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
    srand((unsigned int)time(NULL));
    int rangeMin = 0;
    int rangeMax = 250;
    for (size_t i = 0 ; i < strlen((char *)text2display) ; i++)
    {
		char charToShow[2] = {((char *)text2display)[i], '\0'};

        int ranNum = rangeMin + (int)((double)rand() / ((double)RAND_MAX + 1) * rangeMax);
        Sleep(ranNum + 30);

        ::SendMessage(curScintilla, SCI_APPENDTEXT, 1, (LPARAM)charToShow);
		::SendMessage(curScintilla, SCI_GOTOPOS, ::SendMessage(curScintilla, SCI_GETLENGTH, 0, 0), 0);
    }

    return TRUE;
};

void WhatIsNpp()
{
	char *text2display = "Notepad++ is a free (as in \"free speech\" and also as in \"free beer\") source code editor and Notepad replacement that supports several languages.\n\
		Running in the MS Windows environment, its use is governed by GPL License.\n\
\n\
Based on a powerful editing component Scintilla, Notepad++ is written in C++ and uses pure Win32 API and STL which ensures a higher execution speed and smaller program size.\n\
By optimizing as many routines as possible without losing user friendliness, Notepad++ is trying to reduce the world carbon dioxide emissions. When using less CPU power, the PC can throttle down and reduce power consumption, resulting in a greener environment.";
    HANDLE hThread = ::CreateThread(NULL, 0, threadTextPlayer, text2display, 0, NULL);
    ::CloseHandle(hThread);
}


void insertCurrentPath(int which)
{
	int msg = NPPM_GETFULLCURRENTPATH;
	if (which == FILE_NAME)
		msg = NPPM_GETFILENAME;
	else if (which == CURRENT_DIRECTORY)
		msg = NPPM_GETCURRENTDIRECTORY;

	int currentEdit;
	TCHAR path[MAX_PATH];
	
	// A message to Notepad++ to get a multibyte string (if ANSI mode) or a wide char string (if Unicode mode)
	::SendMessage(nppData._nppHandle, msg, 0, (LPARAM)path);

	//
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
	HWND curScint = (currentEdit == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
#ifdef UNICODE
	int encoding = (int)::SendMessage(curScint, SCI_GETCODEPAGE, 0, 0);
	char pathA[MAX_PATH];
	WideCharToMultiByte(encoding, 0, path, -1, pathA, MAX_PATH, NULL, NULL);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)pathA);
#else
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)path);
#endif
}

void insertCurrentFullPath()
{
	insertCurrentPath(FULL_CURRENT_PATH);
}
void insertCurrentFileName()
{
	insertCurrentPath(FILE_NAME);
}
void insertCurrentDirectory()
{
	insertCurrentPath(CURRENT_DIRECTORY);
}

const bool shortDate = true;
const bool longDate = false;

void insertShortDateTime()
{
	insertDateTime(shortDate);
}

void insertLongDateTime()
{
	insertDateTime(longDate);
}

void insertDateTime(bool format)
{
	TCHAR date[128];
    TCHAR time[128];
    TCHAR dateTime[256];

    SYSTEMTIME st;
	::GetLocalTime(&st);
	::GetDateFormat(LOCALE_USER_DEFAULT, (format == shortDate)?DATE_SHORTDATE:DATE_LONGDATE, &st, NULL, date, 128);
	::GetTimeFormat(LOCALE_USER_DEFAULT, TIME_NOSECONDS, &st, NULL, time, 128);

    wsprintf(dateTime, TEXT("%s %s"), time, date);

	int currentEdit;
	::SendMessage(nppData._nppHandle, NPPM_GETCURRENTSCINTILLA, 0, (LPARAM)&currentEdit);
	HWND curScint = (currentEdit == 0)?nppData._scintillaMainHandle:nppData._scintillaSecondHandle;
#ifdef UNICODE
	int encoding = (int)::SendMessage(curScint, SCI_GETCODEPAGE, 0, 0);
	char dateTimeA[MAX_PATH];
	WideCharToMultiByte(encoding, 0, dateTime, -1, dateTimeA, MAX_PATH, NULL, NULL);
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)dateTimeA);
#else
	::SendMessage(curScint, SCI_REPLACESEL, 0, (LPARAM)dateTime);
#endif

}

void insertHtmlCloseTag()
{
	doCloseTag = !doCloseTag;
	::CheckMenuItem(::GetMenu(nppData._nppHandle), funcItem[5]._cmdID, MF_BYCOMMAND | (doCloseTag?MF_CHECKED:MF_UNCHECKED));
}

void getFileNamesDemo()
{
	int nbFile = (int)::SendMessage(nppData._nppHandle, NPPM_GETNBOPENFILES, 0, 0);
	TCHAR toto[10];
	::MessageBox(nppData._nppHandle, generic_itoa(nbFile, toto, 10), TEXT("nb opened files"), MB_OK);
	
	TCHAR **fileNames = (TCHAR **)new TCHAR*[nbFile];
	for (int i = 0 ; i < nbFile ; i++)
	{
		fileNames[i] = new TCHAR[MAX_PATH];
	}

	if (::SendMessage(nppData._nppHandle, NPPM_GETOPENFILENAMES, (WPARAM)fileNames, (LPARAM)nbFile))
	{
		for (int i = 0 ; i < nbFile ; i++)
		::MessageBox(nppData._nppHandle, fileNames[i], TEXT(""), MB_OK);
	}

	for (int i = 0 ; i < nbFile ; i++)
	{
		delete [] fileNames[i];
	}
	delete [] fileNames;
}

void getSessionFileNamesDemo()
{
	const TCHAR *sessionFullPath = TEXT("c:\\test.session");
	int nbFile = (int)::SendMessage(nppData._nppHandle, NPPM_GETNBSESSIONFILES, 0, (LPARAM)sessionFullPath);

	if (!nbFile)
	{
		::MessageBox(nppData._nppHandle, TEXT("Please modify \"sessionFullPath\" in \"NppInsertPlugin.cpp\" in order to point to a valide session file"), TEXT("Error :"), MB_OK);
		return;
	}
	TCHAR toto[10];
	::MessageBox(nppData._nppHandle, generic_itoa(nbFile, toto, 10), TEXT("nb session files"), MB_OK);
	
	TCHAR **fileNames = (TCHAR **)new TCHAR*[nbFile];
	for (int i = 0 ; i < nbFile ; i++)
	{
		fileNames[i] = new TCHAR[MAX_PATH];
	}

	if (::SendMessage(nppData._nppHandle, NPPM_GETSESSIONFILES, (WPARAM)fileNames, (LPARAM)sessionFullPath))
	{
		for (int i = 0 ; i < nbFile ; i++)
			::MessageBox(nppData._nppHandle, fileNames[i], TEXT("session file name :"), MB_OK);
	}

	for (int i = 0 ; i < nbFile ; i++)
	{
		delete [] fileNames[i];
	}
	delete [] fileNames;
}

void saveCurrentSessionDemo()
{
	TCHAR *sessionPath = (TCHAR *)::SendMessage(nppData._nppHandle, NPPM_SAVECURRENTSESSION, 0, 0);
	if (sessionPath)
		::MessageBox(nppData._nppHandle, sessionPath, TEXT("Saved Session File :"), MB_OK);
}

// Dockable Dialog Demo
// 
// This demonstration shows you how to do a dockable dialog.
// You can create your own non dockable dialog - in this case you don't nedd this demonstration.
// You have to create your dialog by inherented DockingDlgInterface class in order to make your dialog dockable
// - please see DemoDlg.h and DemoDlg.cpp to have more informations.
void DockableDlgDemo()
{
	_goToLine.setParent(nppData._nppHandle);
	tTbData	data = {0};

	if (!_goToLine.isCreated())
	{
		_goToLine.create(&data);

		// define the default docking behaviour
		data.uMask = DWS_DF_CONT_RIGHT;

		data.pszModuleName = _goToLine.getPluginFileName();

		// the dlgDlg should be the index of funcItem where the current function pointer is
		// in this case is DOCKABLE_DEMO_INDEX
		data.dlgID = DOCKABLE_DEMO_INDEX;
		::SendMessage(nppData._nppHandle, NPPM_DMMREGASDCKDLG, 0, (LPARAM)&data);
	}
	_goToLine.display();
}
