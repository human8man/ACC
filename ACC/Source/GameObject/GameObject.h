#pragma once


class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Update() = 0;
	virtual void Draw( D3DXMATRIX& View, D3DXMATRIX& Proj, LIGHT& Light ) = 0;

	// ���W�ݒ�
	void SetPos(const D3DXVECTOR3& pos) { m_vPosition = pos; }
	void SetPos(float x, float y, float z) { m_vPosition = { x, y, z }; }

	// ����W�ݒ�
	void SetPivot(const D3DXVECTOR3& pivot) { m_vPivot = pivot; }
	void SetPivot(float x, float y, float z) { m_vPivot = { x, y, z }; }

	// ��]�ݒ�
	void SetRot(const D3DXVECTOR3& rot) { m_vRotation = rot; }
	void SetRot(float x, float y, float z) { m_vRotation = { x, y, z }; }

	// �g�k�ݒ�
	void SetScale(const D3DXVECTOR3& scale) { m_vScale = scale; }
	void SetScale(float x, float y, float z) { m_vScale = { x, y, z }; }

	// �J���[�ݒ�
	void SetColor(const D3DXVECTOR3& color) { m_vColor = color; }
	void SetColor(float r, float g, float b) { m_vColor = { r, g, b }; }

	// ���ߓx
	void SetAlpha(float alpha) { m_Alpha = alpha; }
	const float& GetAlpha() const { return m_Alpha; }

	const D3DXVECTOR3& GetPos() const { return m_vPosition; }
	const D3DXVECTOR3& GetRot() const { return m_vRotation; }
	const D3DXVECTOR3& GetPivot() const { return m_vPivot; }
	const D3DXVECTOR3& GetScale() const { return m_vScale; }
	const D3DXVECTOR3& GetColor() const { return m_vColor; }

	// �I�u�W�F�N�g���(���W���,��]���,�T�C�Y)���擾
	ObjectInfo GetObjectInfo() const {
		return { m_vPosition, m_vRotation, m_vScale };
	}
protected:
	mutable ObjectInfo m_ObjectInfo;	// �I�u�W�F�N�g���
	D3DXVECTOR3	m_vPosition;	// ���W���
	D3DXVECTOR3	m_vPivot;		// ����W
	D3DXVECTOR3	m_vRotation;	// ��]���
	D3DXVECTOR3	m_vScale;		// �T�C�Y
	D3DXVECTOR3	m_vColor;		// �J���[
	float		m_Alpha;		// ���ߓx
};