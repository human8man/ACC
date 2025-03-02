#pragma once

#include <iostream>
#include <utility>


//============================================================================
//		CRTP���g�����V���O���g���N���X.
//============================================================================
template<typename T>
class CSingleton
{
public:
	static T* GetInstance() // �C���X�^���X�擾.
	{
		static T s_Instance;
		return &s_Instance;
	}

protected:
	CSingleton() = default;
	~CSingleton() = default;

protected:
	// �R�s�[�֎~.
	CSingleton(const CSingleton& rhs) = delete;
	CSingleton& operator = (const CSingleton& rhs) = delete;

};