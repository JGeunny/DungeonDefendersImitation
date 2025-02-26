#ifndef Engine_Defines_h__
#define Engine_Defines_h__

#include <d3d9.h>
#include <d3dx9.h>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <functional>
#include <ctime>
#include <fstream>
#include <deque>
#include <chrono>
#include <process.h>


#pragma warning(disable : 4251)

#define	DIRECTINPUT_VERSION		0x0800
#include <dinput.h>

#include "Engine_Typedef.h"
#include "Engine_Macro.h"

#include "Engine_Function.h"
#include "Engine_Struct.h"
#include "Engine_Enum.h"

extern	HWND		g_hWnd;
extern HINSTANCE	g_hInst;

#define DEBUG_RENDER_NONE		0x00000000
#define DEBUG_RENDER_COLLIDER	0x00000001
#define DEBUG_RENDER_NAVI		0x00000002
#define DEBUG_RENDER_CAMERA		0x00000004
#define DEBUG_RENDER_DIRECTION	0x00000008
#define DEBUG_RENDER_TARGET		0x00000010
#define DEBUG_RENDER_FPS		0x00000020
#define DEBUG_RENDER_ALL		DEBUG_RENDER_COLLIDER + DEBUG_RENDER_NAVI + DEBUG_RENDER_CAMERA + DEBUG_RENDER_DIRECTION + DEBUG_RENDER_TARGET + DEBUG_RENDER_FPS
//static unsigned long g_dwDebugRender = DEBUG_RENDER_ALL;

#define DIRECTION_LEFT_LEFT		11
#define DIRECTION_LEFT_RIGHT	12
#define DIRECTION_LEFT_BOTTOM	13
#define DIRECTION_RIGHT_LEFT	21
#define DIRECTION_RIGHT_RIGHT	22
#define DIRECTION_RIGHT_BOTTOM	23
#define DIRECTION_FRONT_LEFT	31
#define DIRECTION_FRONT_RIGHT	32

inline std::wstring Get_Direction(_int iDrectionString)
{
	switch (iDrectionString)
	{
	case DIRECTION_LEFT_LEFT:
		return L"Direction_Left_Left";
	case DIRECTION_LEFT_RIGHT:
		return L"Direction_Left_Right";
	case DIRECTION_LEFT_BOTTOM:
		return L"Direction_Left_Bottom";
	case DIRECTION_RIGHT_LEFT:
		return L"Direction_Right_Left";
	case DIRECTION_RIGHT_RIGHT:
		return L"Direction_Right_Right";
	case DIRECTION_RIGHT_BOTTOM:
		return L"Direction_Right_Bottom";
	case DIRECTION_FRONT_LEFT:
		return L"Direction_Front_Left";
	case DIRECTION_FRONT_RIGHT:
		return L"Direction_Front_Right";
	}
	return L"";
}

using namespace std;

#endif // Engine_Defines_h__
