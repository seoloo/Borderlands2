#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)

class CMuzzle final : public CGameObject
{
public:
	typedef struct tagEffectDesc
	{
		const _float4x4* pBoneMatrix = nullptr;
		_float4x4				PivotMatrix;
		const _float4x4* pParentMatrix = nullptr;
	}EFFECTDESC;

	typedef struct tagPointInstanceDesc
	{
		_int		iNumInstance = { 0 };
		//_float2		fSize = { 0.f,0.f };
		_float4		vPivotPos = { 0.f,0.f,0.f,0.f };
		_float3		vRange = { 0.f,0.f,0.f };
		_float		fLifeTime = { 0.f };
		_float		fMinSpeed, fMaxSpeed = (0.f, 0.f);
		_bool		bFalling = { false };
		_bool		bRising = { false };
		_bool		bExplosion = { false };
		_bool		bBleeding = { false };
		_bool		bDig = { false };
		_bool		bGreen = { false };
	}POINTINSTANCEDESC;

private:
	CMuzzle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMuzzle(const CMuzzle& rhs);
	virtual ~CMuzzle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader*						m_pShaderCom = { nullptr };
	CTexture*						m_pTextureCom = { nullptr };
	CRenderer*						m_pRendererCom = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	CTransform*						m_pPlayerTransformCom = { nullptr };
	CVIBuffer_Point_Instance*		m_pVIBufferCom = { nullptr };

private:
	_float	m_fFrame = { 0.0f };
	_vector	m_vPlayerPos = { XMVectorSet(0.f,0.f,0.f,0.f) };
	_vector	m_vPlayerLook = { XMVectorSet(0.f,0.f,0.f,0.f) };
	_vector	m_vPlayerUp = { XMVectorSet(0.f,0.f,0.f,0.f) };
	_vector	m_vPlayerRight = { XMVectorSet(0.f,0.f,0.f,0.f) };

	_bool	m_bFire = { false };
private:
	_float4x4						m_WorldMatrix;
	const _float4x4* m_pBoneMatrix = { nullptr };
	_float4x4						m_PivotMatrix;

	const _float4x4* m_pParentMatrix = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	POINTINSTANCEDESC m_Desc;

public:
	static CMuzzle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END