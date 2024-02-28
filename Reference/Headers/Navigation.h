#pragma once

#include "Component.h"

/* 삼각형들의 집합. */

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	enum MOVESTATE { MOVE_FALSE, MOVE_TRUE, MOVE_SLIDING };
public:
	typedef struct tagNavigationDesc
	{
		const _float4x4* pWorldMatrix = { nullptr };
		_int				iCurrentIndex = { -1 };
	}NAVIDESC;
private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pNavigationDataFiles, _int iLevel);
	virtual HRESULT Initialize(void* pArg);

public:
	_bool is_Move(_fvector vPosition);

#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif

private:
	vector<class CCell*>			m_Cells;
	typedef vector<class CCell*>	CELLS;

private:
	const _float4x4* m_pWorldMatrix = { nullptr };
	_int			m_iCurrentIndex = { -1 };

#ifdef _DEBUG
private:
	class CShader* m_pShader = { nullptr };
#endif
public:
	_float3 Get_FirstPos() { return m_vFirstPos; };
	_float3 Get_SecondPos() { return m_vSecondPos; };
	
private:
	MOVESTATE m_MoveState;

	_float3				m_vFirstPos = { 0.f,0.f,0.f };
	_float3				m_vSecondPos = { 0.f,0.f,0.f };

private:
	HRESULT SetUp_Neighbors();
public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFiles, _int iLevel);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;
};

END