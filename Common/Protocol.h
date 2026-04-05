

constexpr uint16_t SERVER_PORT = 3500;
constexpr int WORLD_WIDTH = 8;
constexpr int WORLD_HEIGHT = 8;
constexpr int MAX_PLAYERS = 10;
constexpr int MAX_NAME_LEN = 20;


enum PACKET_TYPE : uint8_t
{ 
	CS_LOGIN,
	CS_MOVE,
	SC_LOGIN_RESULT,
	SC_AVATAR_INFO,
	SC_ADD_PLAYER,
	SC_REMOVE_PLAYER,
	SC_MOVE_PLAYER
};
enum DIRECTION : uint8_t
{ 
	UP, 
	DOWN, 
	LEFT, 
	RIGHT 
};


enum ERROR_CODE : uint8_t
{
	NONE,
	
	// LOGIN
	LOGIN_ALREADY_IN_USE,
};

// Packet
#pragma pack(push, 1)

struct CS_Login {
	uint8_t size;
	PACKET_TYPE   type;
	char username[MAX_NAME_LEN];
};
constexpr uint8_t CS_LOGIN_SIZE = sizeof(CS_Login);

struct CS_Move {
	uint8_t size;
	PACKET_TYPE   type;
	DIRECTION    dir;
};
constexpr uint8_t CS_MOVE_SIZE = sizeof(CS_Move);

struct SC_LoginResult {
	uint8_t size;
	PACKET_TYPE   type;
	ERROR_CODE errCode;
	char message[50];
};
constexpr uint8_t SC_LOGIN_RESULT_SIZE = sizeof(SC_LoginResult);

struct SC_AvatarInfo {
	uint8_t size;
	PACKET_TYPE   type;
	int playerId;
	short x;
	short y;
};
constexpr uint8_t SC_AVATAR_INFO_SIZE = sizeof(SC_AvatarInfo);

struct SC_AddPlayer {
	uint8_t size;
	PACKET_TYPE   type;
	int playerId;
	char username[MAX_NAME_LEN];
	short x;
	short y;
};
constexpr uint8_t SC_ADD_PLAYER_SIZE = sizeof(SC_AddPlayer);

struct SC_RemovePlayer {
	uint8_t size;
	PACKET_TYPE   type;
	int playerId;
};
constexpr uint8_t SC_REMOVE_PLAYER_SIZE = sizeof(SC_RemovePlayer);

struct SC_MovePlayer {
	uint8_t size;
	PACKET_TYPE   type;
	int playerId;
	short x;
	short y;
}; 
constexpr uint8_t SC_MOVE_PLAYER_SIZE = sizeof(SC_MovePlayer);

#pragma pack()

// error

inline void error_display(const wchar_t* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::wcout << msg;
	std::wcout << L"에러" << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}