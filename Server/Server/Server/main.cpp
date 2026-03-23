#include <iostream>
#include <ws2tcpip.h>
#include <print>
#pragma comment(lib, "ws2_32.lib")

#include "../../../Common/Protocol.h"


int main()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		error_display(L"WSAStartup()", WSAGetLastError());
		return 1;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
		std::print("Winsock.dll의 사용 가능한 버전을 찾을 수 없습니다");
		WSACleanup();
		return 1;
	}

}