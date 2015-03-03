#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinSock.h>

#include <atomic>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>

#include <nana\gui.hpp>
#include <nana\gui\place.hpp>
#include <nana\gui\widgets\button.hpp>
#include <nana\gui\widgets\textbox.hpp>
#include <nana\gui\widgets\progress.hpp>
#include <nana\threads\pool.hpp>
