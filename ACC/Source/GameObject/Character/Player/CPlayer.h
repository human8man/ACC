#pragma once
#include "Character/CCharacter.h"

//============================================================================
//		プレイヤークラス.
//============================================================================
class CPlayer
	: public CCharacter
{
public:
	// 移動状態列挙型.
	enum enMoveState
	{
		Stop	= 0, // 停止.
		Forward,	 // 前進.
		Backward,	 // 後退.
		TurnLeft,	 // 左回転.
		TurnRight,	 // 右回転.
	};

public:
	CPlayer();
	virtual ~CPlayer() override;

	virtual void Update() override;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) override;

private:
	// キー入力処理.
	void KeyInput();

private:
	float		m_TurnSpeed;	// 回転速度.
	float		m_MoveSpeed;	// 移動速度.
	float		m_CamRevision;	// カメラ座標の補正値.
	enMoveState	m_MoveState;	// 移動状態.
};