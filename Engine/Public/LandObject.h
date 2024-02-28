#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CLandObject abstract : public CGameObject
{
protected:
	CLandObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLandObject(const CLandObject& rhs);
	virtual ~CLandObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

public:
	_vector SetUp_OnTerrain(_fvector vWorldPos, class CVIBuffer_Terrain* pTerrainVIBuffer, class CTransform* pTerrainTransform);

public:
	virtual CGameObject* Clone(void* pArg) = 0;
	virtual void Free() override;
};

END