#pragma once

#include <array>
#include <tuple>


//=============================================================================
//		GJK�N���X(���Q�l�T�C�g��)
//			https://winter.dev/
//=============================================================================


//-----------------------------------------------------------------------------
//		�R���C�_�[�\����
//-----------------------------------------------------------------------------
struct Collider 
{
public :
	/****************************************************************
	// @brief	�w�肵�������ɑ΂��čł��������_�����߂�֐�
	// @param	Direction : �T����������x�N�g��
	// @return	�w������ɂ�����ł��������_�̍��W
	****************************************************************/
	virtual D3DXVECTOR3 FindFurthestPoint(D3DXVECTOR3 Direction) const final;


	/****************************************************************
	// @brief			���_�������[���h���W�ɕϊ����ăZ�b�g����
	// @param	obj		: �I�u�W�F�N�g�̏��
	// @param	vertex	: ���[�J�����W�n�̒��_���X�g
	****************************************************************/
	virtual void SetVertex(	const ObjectInfo& object, const std::vector<D3DXVECTOR3>& vertex) final;

	//-----------------------------------------------------------------------------
	// @brief �R���C�_�[�̒��_�̏d�S�i���S�j���擾����֐�
	//-----------------------------------------------------------------------------
	virtual D3DXVECTOR3 GetCenter() const;

protected:
	// �R���C�_�[�̒��_���X�g
	std::vector<D3DXVECTOR3> m_Vertices;
};


//=============================================================================
//		�V���v���b�N�X (Simplex) �N���X
//=============================================================================
struct Simplex
{
public:
	Simplex()
		: m_Size ( 0 )
	{}


	// ���������X�g����l��ݒ�
	Simplex& operator = (std::initializer_list<D3DXVECTOR3> list)
	{
		m_Size = min(static_cast<int>(list.size()), 4);
		std::copy_n(list.begin(), m_Size, m_Points.begin());
		return *this;
	}


	// �擪�ɐV�����_��ǉ�
	void Push_Front(const D3DXVECTOR3& point)
	{
		std::rotate(m_Points.rbegin(), m_Points.rbegin() + 1, m_Points.rend());
		m_Points[0] = point;
		m_Size = min(m_Size + 1, 4);
	}


	// �Y�����Z�q
	D3DXVECTOR3& operator[](int i) { return m_Points[i]; }
	// �V���v���b�N�X�̃T�C�Y���擾
	size_t Size() const { return m_Size; }
	// �C�e���[�^�擾
	std::array<D3DXVECTOR3, 4>::const_iterator Begin() const { return m_Points.begin(); }
	std::array<D3DXVECTOR3, 4>::const_iterator End() const { return m_Points.begin() + m_Size; }


private:
	int m_Size;
	//	���_���W�̕ۑ��p�z��
	std::array<D3DXVECTOR3, 4> m_Points;
};


//=============================================================================
//		�Փ˓_�\����
//=============================================================================
struct CollisionPoints {
	D3DXVECTOR3 Normal	= ZEROVEC3;
	float  Depth	= 0.0f;
	bool   Col		= false;
};


//=============================================================================
//		GJK�N���X
//=============================================================================
class GJK
{
public:
	/****************************************************************
	// @brief	��̃R���C�_�[�ƕ�������~���R�t�X�L�[���̃T�|�[�g�_���v�Z����
	// @param	ColliderA : �R���C�_�[A
	// @param	ColliderB : �R���C�_�[B
	// @param	Direction : �T����������x�N�g��
	// @return	�T�|�[�g�_ (�ł������_�̍��W)
	****************************************************************/
	D3DXVECTOR3 Support(const Collider& ColliderA, const Collider& ColliderB, D3DXVECTOR3 Direction);


	/****************************************************************
	// @brief	GJK�A���S���Y����p���ē�̃R���C�_�[�̏Փ˂𔻒肷��
	// @param	ColliderA : �R���C�_�[A
	// @param	ColliderB : �R���C�_�[B
	// @return	�Փˏ�� (�Փ˂̗L����Փ˓_�Ȃ�)
	****************************************************************/
	CollisionPoints GJKC(const Collider& ColliderA, const Collider& ColliderB);


	/****************************************************************
	// @brief	EPA (Expanding Polytope Algorithm) �ɂ��Փˏ���⊮����
	// @param	simplex		: GJK �ɂ���Đ������ꂽ�P�� (�V���v���b�N�X)
	// @param	colliderA	: �R���C�_�[A
	// @param	colliderB	: �R���C�_�[B
	// @return	�Փˏ�� (�Փː[�x��Փ˖@��)
	****************************************************************/
	CollisionPoints EPA(const Simplex& simplex, const Collider& colliderA, const Collider& colliderB);


	/****************************************************************
	// @brief	�|���g�[�v (���ʑ�) ����e�ʂ̖@���x�N�g�����v�Z����
	// @param	Poly	: ���_���X�g
	// @param	Faces	: �ʂ̃C���f�b�N�X���X�g
	// @return	�ʂ̖@���x�N�g�����X�g�ƍł��߂��ʂ̃C���f�b�N�X
	****************************************************************/
	std::tuple<std::vector<D3DXVECTOR4>, size_t> GetFaceNormals(
		const std::vector<D3DXVECTOR3>& Poly,
		const std::vector<size_t>& Faces);


	/****************************************************************
	// @brief	�G�b�W�̋t���������X�g�ɑ��݂��邩�m�F���A�d��������΍폜����
	// @param	Edges	: �G�b�W���X�g
	// @param	Faces	: �ʂ̃C���f�b�N�X���X�g
	// @param	a		: �G�b�W�̎n�_
	// @param	b		: �G�b�W�̏I�_
	****************************************************************/
	void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& Edges,
		const std::vector<size_t>& Faces, size_t a, size_t b);


	/****************************************************************
	// @brief	�^����ꂽ���_�Q�̒��ōł��ႢY���W�̒��_�̈ʒu��Ԃ�
	// @param	obj		: �I�u�W�F�N�g�̏��
	// @param	vertex	: ���_���X�g (���[�J�����W)
	// @return	�ł��Ⴂ���_��Y���W�l
	****************************************************************/
	float SendMinVertexPosY( const ObjectInfo& obje, const std::vector<D3DXVECTOR3>& vertex);


	/****************************************************************
	// @brief	�V���v���b�N�X���g�����A���̒T�����������肷��
	// @param	points		: ���݂̃V���v���b�N�X (���_�W��)
	// @param	Direction	: �X�V��̒T������
	// @return	GJK�̏I�������𖞂������ꍇ true
	****************************************************************/
	bool NextSimplex(Simplex& points, D3DXVECTOR3& Direction);


	/****************************************************************
	// @brief	��̃x�N�g�������������������Ă��邩���肷��
	// @param	Direction	: �����x�N�g��
	// @param	ao			: ���肷��x�N�g��
	// @return	���������Ȃ� true
	****************************************************************/
	bool SameDirection(const D3DXVECTOR3& Direction, const D3DXVECTOR3& ao);


	/****************************************************************
	// @brief	2�_ (����) �̃V���v���b�N�X���������A���̒T�����������肷��
	// @param	points		: ���݂̃V���v���b�N�X (2�_)
	// @param	Direction	: �X�V��̒T������
	// @return	GJK�̏I�������𖞂������ꍇ true
	****************************************************************/
	bool Line(Simplex& points, D3DXVECTOR3& Direction);


	/****************************************************************
	// @brief	3�_ (�O�p�`) �̃V���v���b�N�X���������A���̒T�����������肷��
	// @param	points		: ���݂̃V���v���b�N�X (3�_)
	// @param	Direction	: �X�V��̒T������
	// @return	GJK�̏I�������𖞂������ꍇ true
	****************************************************************/
	bool Triangle(Simplex& points, D3DXVECTOR3& Direction);


	/****************************************************************
	// @brief	4�_ (�l�ʑ�) �̃V���v���b�N�X���������A���̒T�����������肷��
	// @param	points		: ���݂̃V���v���b�N�X (4�_)
	// @param	Direction	: �X�V��̒T������
	// @return	GJK�̏I�������𖞂������ꍇ true
	****************************************************************/
	bool Tetrahedron(Simplex& points, D3DXVECTOR3& Direction);
};