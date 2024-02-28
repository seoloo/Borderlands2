#pragma once

#include "Client_Defines.h"
#include "Base.h"

/* ���������� ���� �ڿ��� �����ϴ� ��Ȱ�� ��Ƽ�����带 �����Ͽ� �����Ѵ�. */

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public: /* Getter */
	const _tchar* Get_LoadingText() const {
		return m_szLoading;
	}

public:
	HRESULT Initialize(LEVEL eNextLevel);
	HRESULT LoadingForNextLevel();

public:
	_bool is_Finished() const {
		return m_isFinished;
	}

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	HANDLE					m_hThread = { 0 };
	CRITICAL_SECTION		m_CriticalSection;
	LEVEL					m_eNextLevel = { LEVEL_END };

	_bool					m_isFinished = { false };
	_tchar					m_szLoading[MAX_PATH] = TEXT("");

private:
	HRESULT Loading_ForLogo();
	HRESULT Loading_ForTown();
	HRESULT Loading_ForGamePlay();
	HRESULT Loading_ForEnding();

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevel);
	virtual void Free() override;
};

END