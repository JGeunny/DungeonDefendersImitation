#ifndef STATUS_H__
#define STATUS_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CStatus : public CComponent
{
private:
	explicit		CStatus();
	virtual			~CStatus();

private:
	HRESULT			Ready_Status(
		_float			_fAtt = 150,
		_float			_fMaxHp = 1300,
		_float			_fMaxMp = 150,
		_float			_fMaxExp = 50,
		_int			_iLevel = 1,
		_int			_iGold = 0
	);

public:
	_bool			Damage(_float _fAtt);
	_bool			Damage_Mp(_float _fMp);
	_bool			LevelUp();
	_bool			Recovery(_float _fHealHp);
	_bool			Recovery_Mp(_float _fHealMp);

public:
	_float			m_fAtt;
	_float			m_fHp;
	_float			m_fMaxHp;
	_float			m_fMp;
	_float			m_fMaxMp;
	_float			m_fExp;
	_float			m_fMaxExp;
	_int			m_iLevel;
	_int			m_iGold;

public:
	static CStatus* Create(
		_float			_fAtt = 150,
		_float			_fMaxHp = 1300,
		_float			_fMaxMp = 150,
		_float			_fMaxExp = 50,
		_int			_iLevel = 1,
		_int			_iGold = 0
	);
private:
	virtual void		Free(void);
};

END

#endif // !STATUS_H__
