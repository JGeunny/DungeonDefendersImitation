#include "InputDev.h"

USING(Engine)
IMPLEMENT_SINGLETON(CInputDev)

Engine::CInputDev::CInputDev(void)
	: m_dwKey(0), m_dwKeyDown(0), m_dwKeyPressed(0)
{

}

Engine::CInputDev::~CInputDev(void)
{
	Free();
}

HRESULT Engine::CInputDev::Ready_InputDev(HINSTANCE hInst, HWND hWnd)
{

	// DInput 컴객체를 생성하는 함수
	FAILED_CHECK_RETURN(DirectInput8Create(hInst,
											DIRECTINPUT_VERSION,
											IID_IDirectInput8,
											(void**)&m_pInputSDK,
											NULL), E_FAIL);

	// 키보드 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	// 생성된 키보드 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pKeyBoard->Acquire();


	// 마우스 객체 생성
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

	// 생성된 마우스 객체의 대한 정보를 컴 객체에게 전달하는 함수
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// 장치에 대한 독점권을 설정해주는 함수, 클라이언트가 떠있는 상태에서 키 입력을 받을지 말지를 결정하는 함수
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// 장치에 대한 access 버전을 받아오는 함수
	m_pMouse->Acquire();


	return S_OK;
}

void CInputDev::Update_InputDev()
{
	Set_InputDev();

	m_dwKey = 0;

	if (Get_DIKeyState(DIK_UP) & 0x80)
		m_dwKey |= KEY_UP;
	if (Get_DIKeyState(DIK_DOWN) & 0x80)
		m_dwKey |= KEY_DOWN;
	if (Get_DIKeyState(DIK_LEFT) & 0x80)
		m_dwKey |= KEY_LEFT;
	if (Get_DIKeyState(DIK_RIGHT) & 0x80)
		m_dwKey |= KEY_RIGHT;
	if (Get_DIKeyState(DIK_RETURN) & 0x80)
		m_dwKey |= KEY_RETURN;
	if (Get_DIKeyState(DIK_SPACE) & 0x80)
		m_dwKey |= KEY_SPACE;
	if (Get_DIMouseState(DIM_LB) & 0x80)
		m_dwKey |= KEY_LBUTTON;
	if (Get_DIMouseState(DIM_RB) & 0x80)
		m_dwKey |= KEY_RBUTTON;
	if (Get_DIKeyState(DIK_A) & 0x80)
		m_dwKey |= KEY_A;
	if (Get_DIKeyState(DIK_S) & 0x80)
		m_dwKey |= KEY_S;
	if (Get_DIKeyState(DIK_D) & 0x80)
		m_dwKey |= KEY_D;
	if (Get_DIKeyState(DIK_W) & 0x80)
		m_dwKey |= KEY_W;
	if (Get_DIKeyState(DIK_Q) & 0x80)
		m_dwKey |= KEY_Q;
	if (Get_DIKeyState(DIK_E) & 0x80)
		m_dwKey |= KEY_E;
	if (Get_DIKeyState(DIK_F1) & 0x80)
		m_dwKey |= KEY_F1;
	if (Get_DIKeyState(DIK_F2) & 0x80)
		m_dwKey |= KEY_F2;
	if (Get_DIKeyState(DIK_F12) & 0x80)
		m_dwKey |= KEY_F12;
	if (Get_DIKeyState(DIK_1) & 0x80)
		m_dwKey |= KEY_1;
	if (Get_DIKeyState(DIK_2) & 0x80)
		m_dwKey |= KEY_2;
	if (Get_DIKeyState(DIK_3) & 0x80)
		m_dwKey |= KEY_3;
	if (Get_DIKeyState(DIK_4) & 0x80)
		m_dwKey |= KEY_4;
	if (Get_DIKeyState(DIK_0) & 0x80)
		m_dwKey |= KEY_0;
}

void Engine::CInputDev::Set_InputDev(void)
{
	m_pKeyBoard->GetDeviceState(256, m_byKeyState);
	m_pMouse->GetDeviceState(sizeof(m_tMouseState), &m_tMouseState);
}

bool CInputDev::KeyDown(DWORD dwKey)
{
	// 이 전에 누른적 없고 현재 눌렸으면 TRUE
	if (!(m_dwKeyDown & dwKey) && (m_dwKey & dwKey))
	{
		m_dwKeyDown |= dwKey;
		return true;
	}

	// 이전에 누른적 있고 현재 누르지 않았으면 FALSE
	if ((m_dwKeyDown & dwKey) && !(m_dwKey & dwKey))
	{
		m_dwKeyDown ^= dwKey;
		return false;
	}

	return false;
}

bool CInputDev::KeyUp(DWORD dwKey)
{
	// 이전에 누른적 있고 현재 누르지 않았다면 TRUE
	if ((m_dwKeyPressed & dwKey) && !(m_dwKey & dwKey))
	{
		m_dwKeyPressed ^= dwKey;
		return true;
	}

	// 이전에 누른적 없고 현재 눌렀다면 FALSE
	if (!(m_dwKeyPressed & dwKey) && (m_dwKey & dwKey))
	{
		m_dwKeyPressed |= dwKey;
		return false;
	}

	return false;
}

bool CInputDev::KeyPressing(DWORD dwKey)
{
	if (m_dwKey & dwKey)
		return true;

	return false;
}

bool CInputDev::KeyCombined(DWORD dwFirst, DWORD dwSecond)
{
	if (KeyDown(dwSecond) && KeyPressing(dwFirst))
		return true;

	return false;
}

void Engine::CInputDev::Free(void)
{
	Engine::Safe_Release(m_pKeyBoard);
	Engine::Safe_Release(m_pMouse);
	Engine::Safe_Release(m_pInputSDK);
}

