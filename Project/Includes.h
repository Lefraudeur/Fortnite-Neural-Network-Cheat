#pragma once

#define _CRT_SECURE_WARNINGS
#define WIN32_LEAN_AND_MEAN

// BetterXor Implementation (For real this time)
#include "Utilities/FNXor.h"

// Include Windows General Library
#include <Windows.h>

// Define name and version

// Include Linked Libraries
#include <filesystem>
#include <shellapi.h>
#include <TlHelp32.h>
#include <WinUser.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <assert.h>
#include <iostream>
#include <signal.h>
#include <Dwmapi.h> 
#include <Urlmon.h>
#include <iostream>
#include <fstream>
#include <Psapi.h>
#include <sstream>
#include <excpt.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <format>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <ctime>

// Include OpenCV
#include "Utilities/Includes/IncludeOpenCV.h"

// Add DirectX Libraries
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

// Definitions
#include "Utilities/Definitions.h"

#define m_chrAppTitle xor("Fortnite Neural Network Cheat")

#define m_chrAppVersion xor("1.0")
#define m_idAppVersion 100

// Include Namespaces
using namespace std;