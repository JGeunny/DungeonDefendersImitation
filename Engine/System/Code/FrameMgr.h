#ifndef FrameMgr_h__
#define FrameMgr_h__

#include "Engine_Defines.h"
#include "Base.h"
#include "Frame.h"

BEGIN(Engine)

class ENGINE_DLL CFrameMgr : public CBase
{
	DECLARE_SINGLETON(CFrameMgr)

private:
	explicit CFrameMgr(void);
	virtual ~CFrameMgr(void);

public:
	_bool		IsPermit_Call(const _tchar* pFrameTag, const _float& fTimeDelta);

public:
	HRESULT		Ready_Frame(const _tchar* pFrameTag, const _float& fCallLimit);
	_int		Get_FPS(const _tchar* pFrameTag);

	_bool		Is_DebugRender(DWORD dwDebugRender) {
									return ((m_dwDebugRender & dwDebugRender) == 0 ? false : true);
								}
	DWORD		Get_DebugRender() { return m_dwDebugRender; }
	void		Set_DebugRender(DWORD dwDebugRender) { m_dwDebugRender = dwDebugRender; }

	void		Set_GayScale(bool _bGayScale) { m_bGayScale = _bGayScale; }
	bool		Get_GayScale() { return m_bGayScale; }
private:
	CFrame*		Find_Frame(const _tchar* pFrameTag);

private:
	map<const _tchar*, CFrame*>		m_mapFrame;

private:
	bool	m_bGayScale = false;
	DWORD m_dwDebugRender = DEBUG_RENDER_ALL;
public:
	virtual void Free(void);
};

END
#endif // FrameMgr_h__
