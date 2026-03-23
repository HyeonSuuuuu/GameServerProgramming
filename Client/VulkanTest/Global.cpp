#include "pch.h"
#include "Global.h"
#include "../../Common/Protocol.h"


// atomic한 moveflag
std::atomic<uint8_t> g_moveFlag{};


// x, y 그냥 global로?
std::atomic<int8_t> g_x{};
std::atomic<int8_t> g_y{};