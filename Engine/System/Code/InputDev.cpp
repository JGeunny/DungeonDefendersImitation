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

	// DInput �İ�ü�� �����ϴ� �Լ�
	FAILED_CHECK_RETURN(DirectInput8Create(hInst,
											DIRECTINPUT_VERSION,
											IID_IDirectInput8,
											(void**)&m_pInputSDK,
											NULL), E_FAIL);

	// Ű���� ��ü ����
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr), E_FAIL);

	// ������ Ű���� ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
	m_pKeyBoard->Acquire();


	// ���콺 ��ü ����
	FAILED_CHECK_RETURN(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr), E_FAIL);

	// ������ ���콺 ��ü�� ���� ������ �� ��ü���� �����ϴ� �Լ�
	m_pMouse->SetDataFormat(&c_dfDIMouse);

	// ��ġ�� ���� �������� �������ִ� �Լ�, Ŭ���̾�Ʈ�� ���ִ� ���¿��� Ű �Է��� ������ ������ �����ϴ� �Լ�
	m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);

	// ��ġ�� ���� access ������ �޾ƿ��� �Լ�
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
	// �� ���� ������ ���� ���� �������� TRUE
	if (!(m_dwKeyDown & dwKey) && (m_dwKey & dwKey))
	{
		m_dwKeyDown |= dwKey;
		return true;
	}

	// ������ ������ �ְ� ���� ������ �ʾ����� FALSE
	if ((m_dwKeyDown & dwKey) && !(m_dwKey & dwKey))
	{
		m_dwKeyDown ^= dwKey;
		return false;
	}

	return false;
}

bool CInputDev::KeyUp(DWORD dwKey)
{
	// ������ ������ �ְ� ���� ������ �ʾҴٸ� TRUE
	if ((m_dwKeyPressed & dwKey) && !(m_dwKey & dwKey))
	{
		m_dwKeyPressed ^= dwKey;
		return true;
	}

	// ������ ������ ���� ���� �����ٸ� FALSE
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

