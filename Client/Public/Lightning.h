#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CCollider;
class CTransform;
class CVIBuffer_Point_Instance;
END

BEGIN(Client)
class CLightning final : public CGameObject
{
public:
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
		_bool		bFog = { false };
		_bool		bBlue = { false };
	}POINTINSTANCEDESC;

private:
	CLightning(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightning(const CLightning& rhs);
	virtual ~CLightning() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CTransform* m_pMonsterFireTransformCom = { nullptr };
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };
	CCollider* m_pColliderCom[COLLIDER_END] = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();
	HRESULT Collision_Object();

private:
	_bool	m_bExplosion = { false };
	_bool	m_bBleeding = { false };
	_bool	m_bDig = { false };
	_bool	m_bFog = { false };
	_bool	m_bGray = { false };
	_bool	m_bBlue = { false };
	_bool	m_bRed = { false };

	_bool	m_bCollided = { false };
	_float4	m_fPos = { 0.f,0.f,0.f,0.f };
	_float4	m_fCreatePos = { 0.f,0.f,0.f,0.f };
	_vector m_vLook;
	POINTINSTANCEDESC m_Desc;

	_float							m_fFrame = { 0.0f };
public:
	static CLightning* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END