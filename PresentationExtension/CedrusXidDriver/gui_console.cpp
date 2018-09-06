#include "stdafx.h"

#ifdef _DEBUG

#include <Windows.h>
#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <io.h>

static const WORD max_console_lines = 500;

void redirect_io_to_console()
{
    int console_handle;
    long std_handle;

    CONSOLE_SCREEN_BUFFER_INFO coninfo;

    FILE *fp;

    // allocate the console for this application
    AllocConsole();

    // set the screen buffer to be big enough to let us scroll text
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &coninfo);
    coninfo.dwSize.Y =  max_console_lines;
    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), coninfo.dwSize);

    // redirect unbffered STDOUT to the console
    std_handle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    console_handle = _open_osfhandle(std_handle, _O_TEXT);
    fp = _fdopen(console_handle, "w");
    *stdout = *fp;
    setvbuf(stdout, NULL, _IONBF, 0);

    // redirect unbuffered STDIN to the console
    std_handle = (long)GetStdHandle(STD_INPUT_HANDLE);
    console_handle = _open_osfhandle(std_handle, _O_TEXT);
    fp = _fdopen(console_handle, "r");
    *stdin = *fp;
    setvbuf(stdin, NULL, _IONBF, 0);

    // redirect unbuffered STDERR to the console
    std_handle = (long)GetStdHandle(STD_ERROR_HANDLE);
    console_handle = _open_osfhandle(std_handle, _O_TEXT);
    fp = _fdopen(console_handle, "w");
    *stderr = *fp;
    setvbuf(stderr, NULL, _IONBF, 0);

    // make cout, wcout, cin, wcin, wcerr, wclog and clog point to the console as well
    std::ios::sync_with_stdio();
}

#endif // _DEBUG