#include "Random.h"


Random::Random()
	: m_RandomEngine (std::random_device{}())
{
}

Random::~Random()
{
}


//=============================================================================
//		�w�肵���͈͂̃����_���Ȑ����𐶐�����
//=============================================================================
int Random::GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> dist(min, max); // �����̈�l���z���`
	return dist(m_RandomEngine); // �����𐶐����ĕԂ�
}


//=============================================================================
//		�w�肵���͈͂̃����_���ȕ��������_���𐶐�����
//=============================================================================
float Random::GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> dist(min, max); // ���������̈�l���z���`
	return dist(m_RandomEngine); // �����𐶐����ĕԂ�
}


//=============================================================================
//		50%�̊m���� true �܂��� false ��Ԃ�
//=============================================================================
bool Random::GetRandomBool()
{
	// 0����1�܂ł̐����̈�l���z���` (0��1�̂ǂ��炩)
	std::uniform_int_distribution<int> dist(0, 1);

	// �����𐶐�. 0���o���� false, 1���o���� true ��Ԃ�
	return (dist(m_RandomEngine) == 1);
}