#pragma once

/**************************************************
*	ゲームオブジェクトクラス.
**/
class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	virtual void Update() = 0;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj,
		LIGHT& Light, CAMERA& Camera ) = 0;

	//座標設定関数.
	void SetPosition( float x, float y, float z ){
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};
	void SetPosition( const D3DXVECTOR3& pos ){
		m_vPosition = pos;
	}
	//座標取得関数.
	const D3DXVECTOR3& GetPosition() const {
		return m_vPosition;
	}

	//回転設定関数.
	void SetRotation( float x, float y, float z ){
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;
	};
	void SetRotation( const D3DXVECTOR3& rot ){
		m_vRotation = rot;
	}
	//回転取得関数.
	const D3DXVECTOR3& GetRotation() const {
		return m_vRotation;
	}

	//拡縮設定関数.
	void SetScale( float x, float y, float z ){
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
	}
	void SetScale( float xyz ){
		m_vScale = D3DXVECTOR3( xyz, xyz, xyz );
	}
	//拡縮取得関数.
	const D3DXVECTOR3& GetScale() const {
		return m_vScale;
	}
	//α値設関数
	void SetAlpha(float alpha) { m_Alpha = alpha; }

protected://protectedは子クラスのみアクセス可能.
	D3DXVECTOR3	m_vPosition;
	D3DXVECTOR3	m_vRotation;
	D3DXVECTOR3	m_vScale;
	float		m_Alpha;
};