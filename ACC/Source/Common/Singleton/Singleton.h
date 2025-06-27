#pragma once

#include <iostream>
#include <utility>


//============================================================================
//		CRTP���g�����V���O���g���N���X
//============================================================================
template<typename T>
class Singleton
{
public:
	static T* GetInstance() // �C���X�^���X�擾
	{
		static T s_Instance;
		return &s_Instance;
	}

protected:
	Singleton() = default;
	~Singleton() = default;

protected:
	// �R�s�[�֎~
	Singleton(const Singleton& rhs) = delete;
	Singleton& operator = (const Singleton& rhs) = delete;

};