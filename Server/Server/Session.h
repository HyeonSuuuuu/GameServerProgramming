#pragma once
#include "../../Common/Protocol.h"



class EXP_OVER {
public:
	EXP_OVER() = delete;
	EXP_OVER(uint8_t client_id, int x, int y);

	WSAOVERLAPPED m_over{};
	uint8_t m_id = UINT8_MAX;
	WSABUF	m_wsaBuf[1] = { SC_MOVE_PKT_SIZE, reinterpret_cast<char*>(&m_movePkt) };
	SC_MovePkt m_movePkt;
};


class Session
{
public:
	Session()
	{
		std::print("Session 기본 생성자 호출 ");
		exit(-1);
	}
	Session(uint8_t id, SOCKET s);
	~Session();

	void DoRecv();
	void DoSend(uint8_t sender_id, int x, int y);

	
	// Application
	void MoveUpdate();
	int x;
	int y;
	CS_MovePkt m_recvMovePkt;
private:
	SOCKET m_socket;
	WSAOVERLAPPED m_recvOver;
	WSABUF m_recvWsaBuf;
	uint8_t m_id;
};

void CALLBACK RecvCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK SendCallback(DWORD err, DWORD numBytes, LPWSAOVERLAPPED over, DWORD flags);