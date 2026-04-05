#pragma once
#include "../../Common/Protocol.h"


constexpr int BUF_SIZE = 200;

enum IOType { IO_SEND, IO_RECV, IO_ACCEPT };

class EXP_OVER {
public:
	WSAOVERLAPPED m_over{};
	IOType m_iotype = IO_RECV;
	WSABUF	m_wsa;
	char m_buff[BUF_SIZE];

	EXP_OVER()
	{
		m_wsa.buf = m_buff;
		m_wsa.len = BUF_SIZE;
	}
	EXP_OVER(IOType iotype)
		: m_iotype(iotype)
	{
		m_wsa.buf = m_buff;
		m_wsa.len = BUF_SIZE;
	}
};


class Session
{
public:
	Session() = delete;
	Session(uint8_t id, SOCKET s);
	~Session();

	void DoRecv();
	void DoSend(uint8_t sender_id, int x, int y);

	
	// Application
	void MoveUpdate();

	bool IsConnected()
	{
		return m_isConnected;
	}
private:
	SOCKET m_socket = INVALID_SOCKET;
	uint32_t m_id = 999;
	bool m_isConnected = false;
	
	EXP_OVER m_recvOver;
	int m_prev_recv = 0;
	char m_userName[MAX_NAME_LEN];
	int16_t m_x = 0, m_y = 0;

	WSABUF m_recvWsaBuf;
};
