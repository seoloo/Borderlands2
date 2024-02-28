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
class CExplosion final : public CGameObject
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
	}POINTINSTANCEDESC;

private:
	CExplosion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosion(const CExplosion& rhs);
	virtual ~CExplosion() = default;

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
	CVIBuffer_Point_Instance* m_pVIBufferCom = { nullptr };

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

private:
	_bool	m_bExplosion = { false };
	_bool	m_bBleeding = { false };
	_bool	m_bDig = { false };
	_bool	m_bFog = { false };
	_bool	m_bGray = { false };
	_bool	m_bBlue = { false };
	_bool	m_bBlack = { false };
	_bool	m_bRed = { false };
	_float4	m_fPos = { 0.f,0.f,0.f,0.f };
	_float4	m_fCreatePos = { 0.f,0.f,0.f,0.f };
	_vector m_vLook;
	POINTINSTANCEDESC m_Desc;

	_float							m_fFrame = { 0.0f };
public:
	static CExplosion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END