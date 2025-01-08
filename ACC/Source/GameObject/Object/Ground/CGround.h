#pragma once

#include "Mesh/Static/CStaticMeshObject.h"
#include "Character/CCharacter.h"

//============================================================================
//		地面クラス.
//============================================================================
class CGround
	: public CStaticMeshObject
{
public:
	CGround();
	virtual ~CGround();

	virtual void Update() override;

	// プレイヤー情報を設定.
	void SetPlayer(CCharacter& pPlayer) { m_pPlayer = &pPlayer; }

protected:
	CCharacter*	m_pPlayer;
};