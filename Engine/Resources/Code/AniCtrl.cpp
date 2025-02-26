#include "AniCtrl.h"
#include <iostream>
USING(Engine)

Engine::CAniCtrl::CAniCtrl(LPD3DXANIMATIONCONTROLLER pAniCtrl)
	: m_pAniCtrl(pAniCtrl)
	, m_iCurrentTrack(0)
	, m_iNewTrack(1)
	, m_fAccTime(0.f)
	, m_strOldAniName("")
	, m_dPeriod(0.0)
{
	m_pAniCtrl->AddRef();
}

Engine::CAniCtrl::CAniCtrl(const CAniCtrl& rhs)
	: m_iCurrentTrack(rhs.m_iCurrentTrack)
	, m_iNewTrack(rhs.m_iNewTrack)
	, m_fAccTime(rhs.m_fAccTime)
	, m_strOldAniName("")
	, m_dPeriod(0.0)
{
	// �ִϸ��̼��� ������ �Ǿ �ȵǱ� ������ ���ʿ� ���� �Լ��� �����Ѵ�
	rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(),	// ���� �� ���� ��ü���� �����ǰ� �ִ� �ִϸ��̼��� ������ ��ȯ
											rhs.m_pAniCtrl->GetMaxNumAnimationSets(),		// ���� ������ �ִϸ��̼��� �ִ� ����(ù ��° ���ڰ��� ���� ����)
											rhs.m_pAniCtrl->GetMaxNumTracks(),				// Ʈ�� ����(�� ����-�ִϸ��̼� ��-�� Ʈ�� ���� �÷����� ����ϴ� ����, ���� ���� ��, Ʈ���� ��κ� �� ���� ��������� �ʿ信 ���� �� �� ������ ���)
											rhs.m_pAniCtrl->GetMaxNumEvents(),				// ���� �޽� �ȿ� ����Ǿ� �ִ� ��Ư�� ȿ���� ������ �ǹ�, ������ �츮�� ������� ���Ѵ�.
											&m_pAniCtrl);
}

Engine::CAniCtrl::~CAniCtrl(void)
{

}

HRESULT Engine::CAniCtrl::Ready_AnimationCtrl(void)
{
	return S_OK;
}

void Engine::CAniCtrl::Set_AnimationSet(const _uint& iIndex)
{
	LPD3DXANIMATIONSET		pAS = nullptr;		// �ִϸ��̼� ���� ������ �����ϴ� ��ü
	m_pAniCtrl->GetAnimationSet(iIndex, &pAS);

	if (m_strOldAniName == pAS->GetName())
		return;

	Set_AnimationSet(pAS);
	
}

void CAniCtrl::Set_AnimationSet(std::string strAniName)
{
	if (strAniName == "")
	{
		CAniCtrl::Set_AnimationSet((_uint)0);
		return;
	}
	if (m_strOldAniName == strAniName)
		return;

	LPD3DXANIMATIONSET		pAS = nullptr;		// �ִϸ��̼� ���� ������ �����ϴ� ��ü
	m_pAniCtrl->GetAnimationSetByName(strAniName.c_str(), &pAS);

	Set_AnimationSet(pAS);
}

void Engine::CAniCtrl::Play_Animation(const _float& fTimeDelta, _int* piCount)
{
	// �ִϸ��̼��� ����ϴ� �Լ�
	m_pAniCtrl->AdvanceTime(fTimeDelta, NULL);		// 2���� : �ִϸ��̼� ���ۿ� ���� ���峪 ����Ʈ�� ���� ó���� ����ϴ� ��ü�� �ּ�, �� �༮�� ������� �ʴ� ���� ������ ������ ������ �����δ� �ڵ��� ����ġ�� Ŀ����.
	m_fAccTime += fTimeDelta;

	D3DXTRACK_DESC		tTrackInfo;
	ZeroMemory(&tTrackInfo, sizeof(D3DXTRACK_DESC));
	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrackInfo);
	if (tTrackInfo.Position >= m_dPeriod)
	{
		m_pAniCtrl->SetTrackPosition(m_iCurrentTrack, 0.0);
		if (nullptr != piCount)
			++(*piCount);
	}
}

void CAniCtrl::Set_AnimationSet(LPD3DXANIMATIONSET pAS)
{
	m_strOldAniName = pAS->GetName();

	m_iNewTrack = (m_iCurrentTrack == 0 ? 1 : 0);

	// ���� �ִϸ��̼� ���� ��ü ��� �ð��� ��ȯ�ϴ� �Լ�
	m_dPeriod = pAS->GetPeriod();

	// �ִϸ��̼� ���� ������ ����.
	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAS);


	// �̺�Ʈ(�޽� ���� �ִ� ����Ʈ)�� ������� ����(�̺�Ʈ�� ������ ���� ������ �ȵǴ� ��� �߻�)
	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	// Ű ������ : �ִϸ��̼� ���� �� Ư�� ������ ������ �ǹ�, ���� ��� ���� ��� �� �Լ��� ȣ��Ǵ� ������ ������ Ű �������̶� �� �� �ִ�.
	// �ִϸ޽��� ��� ��� Ű �������� ���� �� �� ����. ��� ���۸� ���� �Ǿ� �־� ���� ������ ���� ���� ��� �ϰ� �ȴ�.

	// ���� ������ Ʈ���� ��� ������ �����ϴ� �Լ�, 3���� : �������� Ʈ���� ���¸� ������ ���ΰ�(�Ǽ� ������ double���� ���)
	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.25);

	// ���� ����ϴ� Ʈ���� �ӵ��� �����ϴ� �Լ�, �ӵ��� �⺻ ����� 1.f
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// �ִϸ��̼� �ռ� ��, � �������� �������� �� ������ ������ ������ �����ϴ� �Լ�(����ġ ���� : ����ġ ���� �� Ű �������� ���� 1�̾�߸� �Ѵ�)
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// Ʈ���� Ȱ��ȭ, Ȱ��ȭ �Լ��� �� ����ϴ� �Լ��� �ƴϴ�.
	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// ���� �ִϸ��̼��� ����ǰ� �־��� �ð��� �ʱ�ȭ�ϴ� �Լ�(advanced �Լ� ȣ�� �� ���������� �ð��� �����ϰ� �ִµ� �� �ð� ���� �ʱ�ȭ�ϰڴٴ� �ǹ�)
	m_pAniCtrl->ResetTime();
	m_fAccTime = 0.f;

	// ������ ��� ���̴� Ʈ������ ���ο� Ʈ���� �����ϰ� �Ǿ��� ��, 0�ʺ��� �����ϵ��� �����ϴ� �Լ�
	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);

	m_iCurrentTrack = m_iNewTrack;
}

CAniCtrl* Engine::CAniCtrl::Create(LPD3DXANIMATIONCONTROLLER pAniCtrl)
{
	CAniCtrl*	pInstance = new CAniCtrl(pAniCtrl);

	if (FAILED(pInstance->Ready_AnimationCtrl()))
		Safe_Release(pInstance);

	return pInstance;
}

CAniCtrl* Engine::CAniCtrl::Create(const CAniCtrl& rhs)
{
	CAniCtrl*	pInstance = new CAniCtrl(rhs);

	if (FAILED(pInstance->Ready_AnimationCtrl()))
		Safe_Release(pInstance);

	return pInstance;
}

void Engine::CAniCtrl::Free(void)
{
	Safe_Release(m_pAniCtrl);
}

_bool Engine::CAniCtrl::Is_AnimationSetEnd(RANGEFLOAT* pRangeFloat)
{
	D3DXTRACK_DESC		tTrackInfo;
	ZeroMemory(&tTrackInfo, sizeof(D3DXTRACK_DESC));

	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrackInfo);

	if (nullptr == pRangeFloat)
	{
		if (tTrackInfo.Position >= m_dPeriod - 0.1f)
			return true;
	}
	else
	{
		if(tTrackInfo.Position >= m_dPeriod * pRangeFloat->fMin && tTrackInfo.Position <= m_dPeriod * pRangeFloat->fMax)
			return true;
	}
	return false;
}

