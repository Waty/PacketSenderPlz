// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <TlHelp32.h>

#include <nana\gui.hpp>
#include <nana\gui\place.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\textbox.hpp>
#include <nana\gui\widgets\progress.hpp>
#include <nana\threads\pool.hpp>
