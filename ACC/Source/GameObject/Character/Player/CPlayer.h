#pragma once
#include "Character/CCharacter.h"

//============================================================================
//		プレイヤークラス.
//============================================================================
class CPlayer
	: public CCharacter
{
public:
	CPlayer();
	virtual ~CPlayer() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

private:
	// キー入力処理.
	void KeyInput();

private:
	float m_TurnSpeed;	 // 回転速度.
	float m_MoveSpeed;	 // 移動速度.
	float m_CamRevision; // カメラ座標の補正値.
	float m_JumpPower;	 // ジャンプ力.
};