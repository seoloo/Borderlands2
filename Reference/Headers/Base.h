#pragma once

#include "Engine_Defines.h"

/* 모든 클래스들의 부모가 되는 Super클래스다 */
/* 레퍼런스 카운트를 부여하고 증가, 감소 or 삭제의 역활을 부여한다. */

/* abstract  : 추상클래스 */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	CBase();
	virtual ~CBase() = default;

public:
	/* AddRef : 레퍼런스카운트를 증가시키낟. */
	/* unsigned long : 증가시키고 난 이후의값. */
	_ulong AddRef();

	/* Release : 레퍼런스카운트를 감소시키낟. or 삭제한다.  */
	/* unsigned long : 감소시키기 이전의값. */
	_ulong Release();

private:
	_ulong		m_dwRefCnt = { 0 };

public:	
	virtual void Free() = 0;
};

END

