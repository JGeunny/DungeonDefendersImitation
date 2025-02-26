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
	// 애니메이션은 공유가 되어선 안되기 때문에 애초에 복제 함수를 제공한다
	rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(),	// 복제 시 원본 객체에서 제공되고 있는 애니메이션의 개수를 반환
											rhs.m_pAniCtrl->GetMaxNumAnimationSets(),		// 구동 가능한 애니메이션의 최대 개수(첫 번째 인자값과 거의 같음)
											rhs.m_pAniCtrl->GetMaxNumTracks(),				// 트랙 개수(한 동작-애니메이션 셋-을 트랙 위에 올려놓고 재생하는 개념, 정수 형의 값, 트랙은 대부분 한 개를 사용하지만 필요에 따라 두 개 까지도 사용)
											rhs.m_pAniCtrl->GetMaxNumEvents(),				// 현재 메쉬 안에 적용되어 있는 독특한 효과의 개수를 의미, 하지만 우리는 사용하지 못한다.
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
	LPD3DXANIMATIONSET		pAS = nullptr;		// 애니메이션 셋의 정보를 보관하는 객체
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

	LPD3DXANIMATIONSET		pAS = nullptr;		// 애니메이션 셋의 정보를 보관하는 객체
	m_pAniCtrl->GetAnimationSetByName(strAniName.c_str(), &pAS);

	Set_AnimationSet(pAS);
}

void Engine::CAniCtrl::Play_Animation(const _float& fTimeDelta, _int* piCount)
{
	// 애니메이션을 재생하는 함수
	m_pAniCtrl->AdvanceTime(fTimeDelta, NULL);		// 2인자 : 애니메이션 동작에 따른 사운드나 이펙트에 대한 처리를 담당하는 객체의 주소, 이 녀석을 사용하지 않는 것이 오히려 좋은데 이유는 실제로는 코드의 가중치가 커진다.
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

	// 얻어온 애니메이션 셋의 전체 재생 시간을 반환하는 함수
	m_dPeriod = pAS->GetPeriod();

	// 애니메이션 셋을 세팅을 하자.
	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAS);


	// 이벤트(메쉬 내에 있는 이펙트)를 사용하지 않음(이벤트들 때문에 가끔 보간이 안되는 경우 발생)
	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	// 키 프레임 : 애니메이션 동작 중 특정 시점의 동작을 의미, 예를 들면 공격 모션 중 함수가 호출되는 시점의 동작을 키 프레임이라 할 수 있다.
	// 애니메쉬의 경우 모든 키 프레임을 정의 할 수 없다. 몇몇 동작만 정의 되어 있어 선형 보간을 통한 동작 제어를 하게 된다.

	// 현재 지정된 트랙의 재생 유무를 결정하는 함수, 3인자 : 언제부터 트랙의 상태를 결정할 것인가(실수 값으로 double형을 사용)
	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fAccTime + 0.25);

	// 현재 재생하는 트랙의 속도를 결정하는 함수, 속도의 기본 상수값 1.f
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// 애니메이션 합성 시, 어떤 프레임을 기준으로 더 가깝게 보간할 것인지 결정하는 함수(가중치 적용 : 가중치 값은 두 키 프레임의 합이 1이어야만 한다)
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// 트랙을 활성화, 활성화 함수일 뿐 재생하는 함수는 아니다.
	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fAccTime, 0.25, D3DXTRANSITION_LINEAR);

	// 현재 애니메이션이 재생되고 있었던 시간을 초기화하는 함수(advanced 함수 호출 시 내부적으로 시간을 누적하고 있는데 그 시간 값을 초기화하겠다는 의미)
	m_pAniCtrl->ResetTime();
	m_fAccTime = 0.f;

	// 기존의 재생 중이던 트랙에서 새로운 트랙이 등장하게 되었을 때, 0초부터 시작하도록 지시하는 함수
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

