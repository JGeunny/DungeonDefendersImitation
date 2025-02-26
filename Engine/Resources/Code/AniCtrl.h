#ifndef AniCtrl_h__
#define AniCtrl_h__

#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CAniCtrl : public CBase
{
private:
	explicit CAniCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	explicit CAniCtrl(const CAniCtrl& rhs);
	virtual ~CAniCtrl(void);

public:
	LPD3DXANIMATIONCONTROLLER		Get_AniCtrl(void) { return m_pAniCtrl; }
	_bool							Is_AnimationSetEnd(RANGEFLOAT* pRangeFloat = nullptr);

public:
	HRESULT		Ready_AnimationCtrl(void);
	void		Set_AnimationSet(const _uint& iIndex);
	void		Set_AnimationSet(std::string strAniName);
	std::string	Get_AnimationSet() { return m_strOldAniName; }
	void		Play_Animation(const _float& fTimeDelta, _int* piCount = nullptr);

private:
	void		Set_AnimationSet(LPD3DXANIMATIONSET pAS);

	LPD3DXANIMATIONCONTROLLER			m_pAniCtrl;
	_uint								m_iCurrentTrack;

	_uint								m_iNewTrack;
	_float								m_fAccTime;
	std::string							m_strOldAniName;

	_double								m_dPeriod;

public:
	static	CAniCtrl*	Create(LPD3DXANIMATIONCONTROLLER pAniCtrl);
	static  CAniCtrl*	Create(const CAniCtrl& rhs);
	virtual void		Free(void);

};

END
#endif // AniCtrl_h__
