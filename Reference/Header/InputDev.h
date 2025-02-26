#ifndef InputDev_h__
#define InputDev_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CInputDev : public CBase
{
	DECLARE_SINGLETON(CInputDev)

private:
	explicit CInputDev(void);
	virtual ~CInputDev(void);

private:
	void	Set_InputDev(void);

public:
	_byte	Get_DIKeyState(_ubyte byKeyID) { return m_byKeyState[byKeyID]; }
	_byte	Get_DIMouseState(MOUSEKEYSTATE eMouse) { return m_tMouseState.rgbButtons[eMouse]; }
	
	_long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
	{
		return *(((_long*)&m_tMouseState) + eMouseState);
	}
	
	HRESULT Ready_InputDev(HINSTANCE hInst, HWND hWnd);
	void	Update_InputDev(void);

	bool	KeyDown(DWORD dwKey);
	bool	KeyUp(DWORD dwKey);
	bool	KeyPressing(DWORD dwKey);
	bool	KeyCombined(DWORD dwFirst, DWORD dwSecond);

private:
	DWORD m_dwKey;
	DWORD m_dwKeyDown;
	DWORD m_dwKeyPressed;

private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;
private:
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;

private:
	_byte					m_byKeyState[256];
	DIMOUSESTATE			m_tMouseState;

public:
	virtual void	Free(void);

};
END



#endif // InputDev_h__
