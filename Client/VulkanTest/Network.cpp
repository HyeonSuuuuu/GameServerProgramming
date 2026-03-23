#include "pch.h"
#include "Network.hpp"
#include "Global.h"



#include "../../../Common/Protocol.h"


void Networker::Run()
{
	int result{};

	// 시작위치 받기
	WSABUF recvWsaBuf;
	SC_MovePkt recvMovePkt;
	recvWsaBuf.buf = reinterpret_cast<char*>(&recvMovePkt);
	recvWsaBuf.len = SC_MOVE_PKT_SIZE;
	DWORD recvSize{};
	DWORD recvFlag{};
	result = WSARecv(m_socket, &recvWsaBuf, 1, &recvSize, &recvFlag, nullptr, nullptr);
	if (result == SOCKET_ERROR)
	{
		error_display(L"recv 실패", WSAGetLastError());
		return;
	}
	if (recvSize > 0)
	{
		std::println("데이터 받음: x:{} y:{}", recvMovePkt.x, recvMovePkt.y);
	}
	else
	{
		std::println("서버의 연결이 종료되었습니다.");
		return;
	}
	// 처리
	g_x.store(recvMovePkt.x);
	g_y.store(recvMovePkt.y);
	// 잘안찍히면 이벤트로 동기화

	while (m_isRunning.load())
	{
		uint8_t currentFlag = g_moveFlag.exchange(0);
		if (currentFlag != 0)
		{
			CS_MovePkt movePkt;
			movePkt.moveFlag = currentFlag;

			WSABUF wsaBuf;
			wsaBuf.buf = reinterpret_cast<char*>(&movePkt);
			wsaBuf.len = CS_MOVE_PKT_SIZE;

			DWORD sentSize{};
			result = WSASend(m_socket, &wsaBuf, 1, &sentSize, 0, nullptr, nullptr);
			if (result == SOCKET_ERROR)
			{
				error_display(L"데이터 전송 실패", WSAGetLastError());
			}

			//Recv
			WSABUF recvWsaBuf;
			SC_MovePkt recvMovePkt;
			recvWsaBuf.buf = reinterpret_cast<char*>(&recvMovePkt);
			recvWsaBuf.len = SC_MOVE_PKT_SIZE;
			DWORD recvSize{};
			DWORD recvFlag{};
			result = WSARecv(m_socket, &recvWsaBuf, 1, &recvSize, &recvFlag, nullptr, nullptr);
			if (result == SOCKET_ERROR)
			{
				error_display(L"recv 실패", WSAGetLastError());
				break;
			}
			if (recvSize > 0)
			{
				std::println("데이터 받음: x:{} y:{}", recvMovePkt.x, recvMovePkt.y);
			}
			else
			{
				std::println("서버의 연결이 종료되었습니다.");
				break;
			}
			// 처리
			g_x.store(recvMovePkt.x);
			g_y.store(recvMovePkt.y);
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

Networker::~Networker()
{
	Stop();
	WSACleanup();
}

bool Networker::Init()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int result;

	wVersionRequested = MAKEWORD(2, 2);
	result = WSAStartup(wVersionRequested, &wsaData);
	if (result != 0)
	{
		error_display(L"WSAStartup()", WSAGetLastError());
		return false;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		std::print("Winsock.dll의 사용 가능한 버전을 찾을 수 없습니다");
		return false;
	}
	return true;
}

bool Networker::Connect(const char* ip, uint16_t port)
{
	m_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	if (m_socket == INVALID_SOCKET)
	{
		error_display(L"listenSock = WSASocket()", WSAGetLastError());
	}
	
	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);

	int32_t result = inet_pton(AF_INET, ip, &serverAddr.sin_addr);
	if (result == 0)
	{
		std::println("잘못된 IP 주소 형식입니다: {}", ip);
		return false;
	}
	else if (result == -1)
	{
		error_display(L"inet_pton()", WSAGetLastError());
		return false;
	}

	result = WSAConnect(m_socket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr), nullptr, nullptr, nullptr, nullptr);
	if (result == SOCKET_ERROR)
	{
		error_display(L"서버 연결 실패", WSAGetLastError());
		return false;
	}
	return true;
}

void Networker::Start()
{
	m_isRunning.store(true);
	m_thread = std::thread([this]() {
		Run();
		});
	return;
}

void Networker::Stop()
{
	m_isRunning.store(false);

	if (m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	
	if (m_thread.joinable())
		m_thread.join();
}


