#pragma once

#include "Shader.h"
#include "Texture.h"
#include "Renderer.h"
#include "Transform.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Rect.h"
#include "VIBuffer_Terrain.h"
#include "Model.h"
#include "Collider.h"
#include "Navigation.h"
#include "VIBuffer_Rect_Instance.h"
#include "Animation.h"
#include "VIBuffer_Point_Instance.h"

/*컴포넌트들의 레벨별로 원형을 보관한다. */

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag, class CComponent* pPrototype);
	CComponent* Clone_Component(_uint iLevelIndex, const wstring& strPrototypeTag, void* pArg);

private:
	_uint									m_iNumLevels = { 0 };
	map<wstring, class CComponent*>*		m_pPrototypes = { nullptr };
	typedef map<wstring, class CComponent*>	PROTOTYPE;

private:
	class CComponent* Find_Prototype(_uint iLevelIndex, const wstring& strPrototypeTag);

public:
	virtual void Free() override;
};

END