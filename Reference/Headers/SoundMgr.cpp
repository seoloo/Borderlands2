#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr()
{
	m_pSystem = nullptr; 
}

HRESULT CSoundMgr::Initialize()
{
	FMOD_RESULT fr = FMOD_OK;
	// 사운드를 담당하는 대표객체를 생성하는 함수
	fr = FMOD_System_Create(&m_pSystem);
	if (fr != FMOD_OK)
		return E_FAIL;

	// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	fr = FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);
	if (fr != FMOD_OK)
		return E_FAIL;

	LoadSoundFile(); 
	return S_OK;
}

void CSoundMgr::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		FMOD_RESULT result = FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear(); 

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}




void CSoundMgr::PlaySound(const wstring& pSoundKey, float fVolume)
{
	map<wstring, FMOD_SOUND*>::iterator iter; 

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), 
		[&](auto& iter)->bool 
	{
			return pSoundKey == iter.first;
	});
	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE; 

	CHANNELID eID = SOUND1;
	while (isplaying(eID))
	{
		eID = (CHANNELID)(eID + 1);
		if (eID >= MAXCHANNEL)
			return;
	}
	
	FMOD_System_PlaySound(m_pSystem, iter->second,nullptr, FALSE, &m_pChannelArr[eID]);
	

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlaySoundeffect(const wstring& pSoundKey, CHANNELID eID, float fVolume)
{
	map<wstring, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return pSoundKey == iter.first;
		});

	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE;

	if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second,nullptr, FALSE, &m_pChannelArr[eID]);
	}

	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlayBGM(const wstring& pSoundKey, float fVolume)
{
	map<wstring, FMOD_SOUND*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return pSoundKey == iter.first;
	});
	
	if (iter == m_mapSound.end())
		return;
	
	if (isplaying(SOUND_BGM))
	{
		FMOD_System_PlaySound(m_pSystem, iter->second, nullptr, FALSE, &m_pChannelArr[SOUND_BGM2]);
		FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM2], FMOD_LOOP_NORMAL);
		FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM2], fVolume);
		FMOD_System_Update(m_pSystem);
		return;
	}
	FMOD_System_PlaySound(m_pSystem, iter->second,nullptr, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound(CHANNELID eID)
{
	FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSoundMgr::StopAll()
{
	for (int i = 0 ; i < MAXCHANNEL ; ++i)
		FMOD_Channel_Stop(m_pChannelArr[i]);
}

void CSoundMgr::SetChannelVolume(CHANNELID eID, float fVolume)
{
	FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	FMOD_System_Update(m_pSystem);
}

bool CSoundMgr::isplaying(CHANNELID eID)
{
	FMOD_BOOL playing = false;

	FMOD_Channel_IsPlaying(m_pChannelArr[eID], &playing);

	return playing;
}

void CSoundMgr::SetPitch(_uint eid, _float fPitch)
{
	FMOD_Channel_SetPitch(m_pChannelArr[eid], fPitch);
}

void CSoundMgr::LoadSoundFile()
{
	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	_finddata_t fd; 

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t  handle = _findfirst("../Bin/AudioClip/*.*", &fd);

	if (handle == -1)
		return; 

	int iResult = 0; 

	char szCurPath[128] = "../Bin/AudioClip/";
	char szFullPath[128] = ""; 

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath); 

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		FMOD_SOUND* pSound = nullptr; 
		
		FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = (int)strlen(fd.name) + 1; 

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
			Safe_Delete_Array(pSoundKey);
			printf("(%d) %s\n", eRes, szFullPath);
		}
		else {
			printf("[ERROR] (%d) %s\n", eRes, szFullPath);
		}
		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴
		iResult = _findnext(handle, &fd);
	}

	FMOD_System_Update(m_pSystem);
	
	_findclose(handle);
}
