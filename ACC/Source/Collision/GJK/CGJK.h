#pragma once
#include <array>
#include <tuple>

//===========================================================
//	GJK �Q�l�ɂ����T�C�g
//  https://winter.dev/
//===========================================================


//�R���C�_�[�\����
struct Collider {
	//����̕����̃T�|�[�g�|�C���g��������֐�
	virtual D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const = 0;
};

//���b�V���̏Փ˔���
struct MeshCollider : Collider
{
public:
	D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const override;

	// ���_�������[���h���W�ɕϊ����ăZ�b�g
	void SetVertex(
		D3DXVECTOR3 pos,
		D3DXVECTOR3 rot, 
		D3DXVECTOR3 scale, 
		std::vector<D3DXVECTOR3> vertex);

private:
	//���_
	std::vector<D3DXVECTOR3> m_Vertices;

};

//��
struct BoxCollider : Collider
{
public:
	D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const override;

	// ���_�������[���h���W�ɕϊ����ăZ�b�g
	void SetVertex(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, std::vector<D3DXVECTOR3> vertex);
private:
	//���_
	std::vector<D3DXVECTOR3> m_Vertices;
};

//�V���v���b�N�X
struct Simplex
{
public:
	Simplex()
		: m_Size ( 0 )
	{}

	Simplex& operator = (std::initializer_list<D3DXVECTOR3> list)
	{
		m_Size = 0;

		for (D3DXVECTOR3 Point : list)
		{
			m_Points[m_Size++] = Point;
		}

		return *this;
	}

	void Push_Front(D3DXVECTOR3 point)
	{
		m_Points = { point,m_Points[0],m_Points[1],m_Points[2] };
		m_Size = min(m_Size + 1,4);
	}

	D3DXVECTOR3& operator[](int i) { return m_Points[i]; }
	size_t Size() const { return m_Size; }

	auto Begin() const { return m_Points.begin(); }
	auto End() const { return m_Points.end() - (4 - m_Size); }

private:
	std::array<D3DXVECTOR3, 4> m_Points;
	int m_Size;
};

//�Փ˓_�\����
struct CollisionPoints {
	D3DXVECTOR3 Normal	= ZEROVEC3;
	float  Depth	= 0.0f;
	bool   Col		= false;
};

//================================================================
//		GJK�N���X.
//================================================================
class CGJK
{
public:
	//��̃R���C�_�[�ƕ����Ń~���R�t�X�L�[���̒��_��Ԃ�
	D3DXVECTOR3 Support(const Collider& ColliderA, const Collider& ColliderB, D3DXVECTOR3 Direction);

	CollisionPoints GJK(const Collider& ColliderA, const Collider& ColliderB);

	CollisionPoints EPA(
		const Simplex& simplex,
		const Collider& colliderA,
		const Collider& colliderB);

	//�C���f�b�N�X���璸�_���擾
	std::tuple<std::vector<D3DXVECTOR4>, size_t> GetFaceNormals(
		const std::vector<D3DXVECTOR3>& Poly, 
		const std::vector<size_t>& Faces);

	//�G�b�W�̋t���������X�g�ɑ��݂��邩�m�F���A���݂���ꍇ�폜
	void AddIfUniqueEdge(
		std::vector<std::pair<size_t,size_t>>& Edges, //�G�b�W
		const std::vector<size_t>& Faces,			  //�C���f�b�N�X
		size_t a,									  
		size_t b);									  

	//����ꂽ���_���ň�ԒႢ�ʒu�ɂ��钸�_�̈ʒu���𑗂�
	float SendMinVertexPosY(D3DXVECTOR3 _Pos, D3DXVECTOR3 _Rot, D3DXVECTOR3 _Scale, std::vector<D3DXVECTOR3> vertex);

	bool NextSimplex(Simplex& points, D3DXVECTOR3& Direction);

	bool SameDirection(const D3DXVECTOR3& Direction,const D3DXVECTOR3& ao);

	bool Line(Simplex& points, D3DXVECTOR3& Direction);

	bool Triangle(Simplex& points, D3DXVECTOR3& Direction);

	bool Tetrahedron(Simplex& points, D3DXVECTOR3& Direction);
};