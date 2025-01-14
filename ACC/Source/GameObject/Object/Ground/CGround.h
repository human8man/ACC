#pragma once

#include "Mesh/Static/CStaticMeshObject.h"
#include "Character/CCharacter.h"

//============================================================================
//		’n–ÊƒNƒ‰ƒX.
//============================================================================
class CGround
	: public CStaticMeshObject
{
public:
	CGround();
	virtual ~CGround();

	virtual void Update() override;

protected:


};