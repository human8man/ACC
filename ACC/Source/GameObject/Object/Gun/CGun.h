#pragma once
#include "GameObject/Mesh/Static/CStaticMeshObject.h"


//============================================================================
//		�e�N���X.
//============================================================================
class CGun
	: public CStaticMeshObject
{
public:
	CGun();
	virtual ~CGun() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

private:
};