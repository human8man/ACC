#pragma once


//===================================================================
//		ゲームオブジェクトクラス.
//===================================================================
class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

	virtual void Update() = 0;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) = 0;

	// 座標設定関数.
	void SetPos(const D3DXVECTOR3& pos) { m_vPosition = pos; }
	void SetPos( float x, float y, float z ){
		m_vPosition.x = x;
		m_vPosition.y = y;
		m_vPosition.z = z;
	};

	// 回転設定関数.
	void SetRot(const D3DXVECTOR3& rot) { m_vRotation = rot; }
	void SetRot( float x, float y, float z ){
		m_vRotation.x = x;
		m_vRotation.y = y;
		m_vRotation.z = z;
	};

	// 拡縮設定関数.
	void SetScale( float xyz ) { m_vScale = D3DXVECTOR3( xyz, xyz, xyz ); }
	void SetScale( float x, float y, float z ){
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
	}

	// 透過度設定関数.
	void SetAlpha( float alpha ) { m_Alpha = alpha; }


	const float& GetAlpha() const { return m_Alpha; }				// 透過度を取得.
	const D3DXVECTOR3& GetPos()		 const { return m_vPosition; }	// 座標情報を取得.
	const D3DXVECTOR3& GetRot()		 const { return m_vRotation; }	// 回転情報をを取得.
	const D3DXVECTOR3& GetScale()	 const { return m_vScale;	 }	// サイズを取得.
	// オブジェクト情報(座標情報,回転情報,サイズ)を取得.
	const ObjectInfo& GetObjeInfo()	 const { return ObjectInfo{ m_vPosition, m_vRotation, m_vScale }; }

protected:
	D3DXVECTOR3	m_vPosition;	// 座標情報.
	D3DXVECTOR3	m_vRotation;	// 回転情報.
	D3DXVECTOR3	m_vScale;		// サイズ.
	float		m_Alpha;		// 透過度.
};