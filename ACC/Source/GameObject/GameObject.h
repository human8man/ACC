#pragma once


class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Update() = 0;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) = 0;

	// 座標設定
	void SetPos(const D3DXVECTOR3& pos) { m_vPosition = pos; }
	void SetPos(float x, float y, float z) { m_vPosition = { x, y, z }; }

	// 基準座標設定
	void SetPivot(const D3DXVECTOR3& pivot) { m_vPivot = pivot; }
	void SetPivot(float x, float y, float z) { m_vPivot = { x, y, z }; }

	// 回転設定
	void SetRot(const D3DXVECTOR3& rot) { m_vRotation = rot; }
	void SetRot(float x, float y, float z) { m_vRotation = { x, y, z }; }

	// 拡縮設定
	void SetScale(const D3DXVECTOR3& scale) { m_vScale = scale; }
	void SetScale(float x, float y, float z) { m_vScale = { x, y, z }; }

	// カラー設定
	void SetColor(const D3DXVECTOR3& color) { m_vColor = color; }
	void SetColor(float r, float g, float b) { m_vColor = { r, g, b }; }

	// 透過度
	void SetAlpha(float alpha) { m_Alpha = alpha; }
	const float& GetAlpha() const { return m_Alpha; }

	const D3DXVECTOR3& GetPos() const { return m_vPosition; }
	const D3DXVECTOR3& GetRot() const { return m_vRotation; }
	const D3DXVECTOR3& GetPivot() const { return m_vPivot; }
	const D3DXVECTOR3& GetScale() const { return m_vScale; }
	const D3DXVECTOR3& GetColor() const { return m_vColor; }

	// オブジェクト情報(座標情報,回転情報,サイズ)を取得
	ObjectInfo GetObjectInfo() const {
		return { m_vPosition, m_vRotation, m_vScale };
	}
protected:
	mutable ObjectInfo m_ObjectInfo;	// オブジェクト情報
	D3DXVECTOR3	m_vPosition;	// 座標情報
	D3DXVECTOR3	m_vPivot;		// 基準座標
	D3DXVECTOR3	m_vRotation;	// 回転情報
	D3DXVECTOR3	m_vScale;		// サイズ
	D3DXVECTOR3	m_vColor;		// カラー
	float		m_Alpha;		// 透過度
};