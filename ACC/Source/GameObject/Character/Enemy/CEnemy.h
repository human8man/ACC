#pragma once

#include "../GameObject/Character/CCharacter.h"

/**************************************************
*	�G�l�~�[�N���X.
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


