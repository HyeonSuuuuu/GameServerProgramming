
namespace lve { class Coordinator; }

class Networker
{
public:
	Networker() = default;
	~Networker();
	bool Init();
	bool Connect(const char* ip, uint16_t port);
	void Run(lve::Coordinator& coordinator);


	// 입장 및 퇴장처리
	std::queue<uint8_t> m_enterQueue;
	std::queue<uint8_t> m_leaveQueue;

private:

	SOCKET m_socket = INVALID_SOCKET;
};