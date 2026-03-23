#pragma once


// core


// network
#include <winSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <ws2tcpip.h>

// libs


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <GLM/gtc/constants.hpp>


// std
#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <stdexcept>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <set>
#include <fstream>
#include <cassert>
#include <unordered_set>
#include <chrono>
#include <print>
#include <thread>
#include <atomic>
#include <print>
#include <mutex>
#include <queue>