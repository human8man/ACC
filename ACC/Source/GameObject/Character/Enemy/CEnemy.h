#pragma once

#include "Character/CCharacter.h"

/**************************************************
*	エネミークラス.
**/
class CEnemy
	: public CCharacter
{
public:
	CEnemy();
	virtual ~CEnemy();

	virtual void Update() override;

protected:

};


