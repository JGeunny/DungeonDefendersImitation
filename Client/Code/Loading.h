#ifndef Loading_h__
#define Loading_h__

#include "Defines.h"
#include "Base.h"

class CLoading : public CBase
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphicDev);
	virtual ~CLoading(void);

public:
	LOADINGID			Get_LoadingID(void) const { return m_LoadingID; }
	CRITICAL_SECTION*	Get_Crt(void) { return &m_Crt; }
	_bool				Get_Finish(void) { return m_bFinish; }
	std::wstring		Get_String(void);

	HRESULT				LoadingCount(HRESULT hResult);

public:
	static _uint	APIENTRY	Thread_Main(void* pArg);

public:
	HRESULT			Ready_Loading(LOADINGID eLoadingID);
	_uint			Loading_ForStage(void);
	HRESULT			Loading_Sound();

	int				m_iCurLoading = 0;
	int				m_iMaxLoading = 0;
private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;
	LOADINGID			m_LoadingID;
	LPDIRECT3DDEVICE9	m_pGraphicDev;
	_bool				m_bFinish = false;
	_tchar				m_szLoading[128];

	HRESULT			Load_NaviMesh(std::wstring strPath);
public:	
	static CLoading*		Create(LPDIRECT3DDEVICE9 pGraphicDev, LOADINGID eLoadingID);
private:
	virtual void Free(void);


};

#endif // Loading_h__
