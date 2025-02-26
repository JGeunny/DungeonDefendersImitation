#include "Status.h"

USING(Engine)

CStatus::CStatus()
{
}

CStatus::~CStatus()
{
}

HRESULT CStatus::Ready_Status(_float _fAtt, _float _fMaxHp, _float _fMaxMp, _float _fMaxExp, _int _iLevel, _int _iGold)
{
	m_fAtt = _fAtt;
	m_fHp = m_fMaxHp = _fMaxHp;
	m_fMp = m_fMaxMp = _fMaxMp;
	m_fExp = 0.f;
	m_fMaxExp = _fMaxExp;
	m_iLevel = _iLevel;
	m_iGold = _iGold;

	return S_OK;
}

_bool CStatus::Damage(_float _fAtt)
{
	m_fHp -= _fAtt;
	if (m_fHp <= 0.f)
	{
		m_fHp = 0.f;
		return false;
	}
	return true;
}

_bool CStatus::Damage_Mp(_float _fMp)
{
	m_fMp -= _fMp;
	if (m_fMp <= 0.f)
	{
		m_fMp = 0.f;
		return false;
	}
	return true;
}

_bool CStatus::LevelUp()
{
	if (m_fExp < m_fMaxExp)
		return false;
	
	m_fExp -= m_fMaxExp;
	m_fMaxExp += m_fMaxExp * 0.5f;

	m_fAtt += m_fAtt * 0.5f;
	m_fMaxHp += 100.f;
	m_fMaxMp += 100.f;

	Recovery(m_fMaxHp);
	Recovery_Mp(m_fMaxMp);

	return true;
}

_bool CStatus::Recovery(_float _fHealHp)
{
	if (m_fHp == m_fMaxHp) return false;
	m_fHp += _fHealHp;
	if (m_fHp > m_fMaxHp)
		m_fHp = m_fMaxHp;
	return true;
}

_bool CStatus::Recovery_Mp(_float _fHealMp)
{
	if (m_fMp == m_fMaxMp) return false;
	m_fMp += _fHealMp;
	if (m_fMp > m_fMaxMp)
		m_fMp = m_fMaxMp;
	return true;
}

CStatus * CStatus::Create(_float _fAtt, _float _fMaxHp, _float _fMaxMp, _float _fMaxExp, _int _iLevel, _int _iGold)
{
	CStatus* pInstance = new CStatus();

	if (FAILED(pInstance->Ready_Status(_fAtt, _fMaxHp, _fMaxMp, _fMaxExp, _iLevel, _iGold)))
		Engine::Safe_Release(pInstance);

	return pInstance;
}

void CStatus::Free(void)
{
}
