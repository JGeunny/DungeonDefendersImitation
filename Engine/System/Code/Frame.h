#ifndef Frame_h__
#define Frame_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFrame : public CBase
{
private:
	explicit CFrame(void);
	virtual ~CFrame(void);

public:
	_bool		IsPermit_Call(const _float& fTimeDelta);

public:
	HRESULT		Ready_Frame(const _float& fCallLimit);
	_int		Get_FPS(void) { return m_iFPS_Render; }

private:
	_float		m_fCallLimit = 0.f;
	_float		m_fAccTimeDelta = 0.f;
	_float		m_fFpsTime = 0.f;
	_int		m_iFPS = 0;
	_int		m_iFPS_Render = 0;

public:
	static CFrame*		Create(const _float& fCallLimit);
	virtual void		Free(void);

};

END
#endif // Frame_h__
