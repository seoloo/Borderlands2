#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CSoundMgr :public CBase
{
	DECLARE_SINGLETON(CSoundMgr)
public :
	enum CHANNELID {
		SOUND_BGM,
		SOUND_BGM2,
		SOUND1,
		SOUND2,
		SOUND3,
		SOUND4,
		SOUND5,
		SOUND6,
		SOUND7,
		SOUND8,
		SOUND9,
		SOUND10,
		SOUND11,
		SOUND12,
		SOUND13,
		SOUND14,
		SOUND15,
		SOUND16,
		SOUND17,
		SOUND18,
		SOUND20,
		SOUND21,
		SOUND22,
		SOUND23,
		SOUND24,
		SOUND25,
		SOUND26,
		SOUND27,
		SOUND28,
		SOUND29,
		MAXCHANNEL
	};


private:
	CSoundMgr();
	virtual ~CSoundMgr() = default;

public:
	HRESULT Initialize(); 
	virtual void Free() override; 

public:
	void PlaySoundW(const wstring& pSoundKey, float fVolume);
	void PlaySoundeffect(const wstring& pSoundKey, CHANNELID eID,  float fVolume);
	void PlayBGM(const wstring& pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);
	bool isplaying(CHANNELID eid);
	void SetPitch(_uint eid, _float fPitch);
	

private:
	void LoadSoundFile(); 

	// ���� ���ҽ� ������ ���� ��ü 
	map<wstring, FMOD_SOUND*> m_mapSound; 
	
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD_CHANNEL* m_pChannelArr[MAXCHANNEL]; 

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD_SYSTEM* m_pSystem; 
};

END