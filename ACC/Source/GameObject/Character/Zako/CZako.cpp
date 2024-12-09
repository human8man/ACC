#include "CZako.h"

CZako::CZako()
	: m_AnimNo		()
	, m_AnimTime	()
	, m_AnimSpeed	( 0.0001 )
	, m_BonePos		()
{
}

CZako::~CZako()
{
}

void CZako::Update()
{
	m_pMesh->SetAnimSpeed( m_AnimSpeed );

	//アニメーション切り替え.
	if( GetAsyncKeyState( 'N' ) & 0x8000 )
	{
		m_AnimNo	= 2;	//登場アニメーションへ.
		m_AnimTime	= 0.0;	//アニメーション経過時間初期化.
		m_pMesh->ChangeAnimSet( m_AnimNo, m_pAnimCtrl );
	}
	if( m_AnimNo == 2 )
	{
		//アニメーション経過時間を加算.
		m_AnimTime += m_AnimSpeed;

		//登場アニメーションの終了時間を超えた？.
		if( m_pMesh->GetAnimPeriod( m_AnimNo ) < m_AnimTime )
		{
			m_AnimNo	= 0;	//待機アニメーションへ.
			m_AnimTime	= 0.0;	//アニメーション経過時間初期化.
			m_pMesh->ChangeAnimSet( m_AnimNo, m_pAnimCtrl );
		}
	}
	if( m_AnimNo == 0 )
	{
		//アニメーション経過時間を加算.
		m_AnimTime += m_AnimSpeed;

		//登場アニメーションの終了時間を超えた？.
		if( m_pMesh->GetAnimPeriod( m_AnimNo ) < m_AnimTime )
		{
			m_AnimNo	= 1;	//歩きアニメーションへ.
			m_AnimTime	= 0.0;	//アニメーション経過時間初期化.
			m_pMesh->ChangeAnimSet( m_AnimNo, m_pAnimCtrl );
		}
	}
	//ボーン座標の取得.
	m_pMesh->GetPosFromBone( "entotu_Joint", &m_BonePos );
}

void CZako::Draw(
	D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light, CAMERA& Camera )
{
	m_pMesh->SetAnimSpeed( m_AnimSpeed );
	CSkinMeshObject::Draw( View, Proj, Light, Camera );
}
