#ifndef SOUNDMGR_H__
#define SOUNDMGR_H__

#include "Engine_Defines.h"

// 사운드 매니저를 사용하기 위해 추가한 해더들. 
#include <io.h>
#include "fmod.h"

BEGIN(Engine)

// SoundMgr
// 사운드 파일 읽고 관리

class ENGINE_DLL CSoundMgr
{
	DECLARE_SINGLETON(CSoundMgr)

	// 생성자 , 소멸자, 생성함수
private:
	CSoundMgr();
	~CSoundMgr();

public:
	enum SOUNDID
	{
		BGM,
		SE,
		SE_SUB,
		ENV,
		BGM_SUB,
		End
	};

	///////////////////////////////////////////////

	// 게임루프 함수,변수
public:
	void Initialize();
	void Update(_vec3 vCameraPos = _vec3(0, 0, 0));
	void Release();

	void SetVolume(SOUNDID eSoundID, _vec3* pvSoundPos);

protected:


private:


	///////////////////////////////////////////////

	// 기타 함수,변수
public:
	void			StartSound(wstring wstrSoundKey, SOUNDID eSoundID, bool bLoop = false, _vec3* pvPos = nullptr);	// 소리 재생
	void			PauseSound(SOUNDID eSoundID);											// 채널 일시정지
	void			ResumeSound(SOUNDID eSoundID);											// 채널 재개
	void			StopSound(SOUNDID eSoundID);											// 채널 정지
	void			StopAll();																// 모든 채널 정지

	void			SetVolume(SOUNDID eSoundID, float _fVolume);							// 볼륨 설정
	int				GetIsPlay(FMOD_CHANNEL** pChannel);										// 채널 재생중인지 판별
	FMOD_CHANNEL**	GetPlayableChannel(SOUNDID eSoundID);									// 재생가능한 채널 반환

	HRESULT			AddSoundFile(wstring wstrFilePath, wstring wstrSoundKey);				// 사운드 읽고 추가
	HRESULT			LoadSoundFromTxt(const wstring & wstrFilePath);							// 사운드 정보 로드

protected:
	_vec3			m_vCameraPos = _vec3(0.f, 0.f, 0.f);

private:
	FMOD_SYSTEM*				m_pSystem;											// FMOD 관리 객체
	std::map<std::wstring, FMOD_SOUND*>	m_mapSound;											// 사운드 정보
	std::vector<FMOD_CHANNEL*>		m_vecChannel[static_cast<int>(SOUNDID::End)];	// 채널 정보
	float						m_fVolume[static_cast<int>(SOUNDID::End)];	// 채널별 사운드 정보
};

END

#endif // !SOUNDMGR_H__
