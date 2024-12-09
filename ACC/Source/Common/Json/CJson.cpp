#include "CJson.h"
#include <sstream>

CJson::CJson()
{
}

CJson::~CJson()
{
}

//�w�肵���t�@�C����ǂݍ���ŕԂ�
Json CJson::LoadjsonFile(std::string FileName)
{
	//�f�[�^�̓ǂݍ��ݐ�
	std::ifstream LoadData;

	//�ǂݍ��ݏ�ԂŃt�@�C�����J��
	std::string a = FileName + ".json";
	LoadData.open(a, std::ios::in);
	
	//�ǂݍ��񂾃f�[�^��n��
	Json outdata;
	
	outdata = Json::parse(LoadData);

	//�t�@�C�������
	LoadData.close();

	return outdata;
}

//�w�肳�ꂽ�t�@�C���Ƀf�[�^��ۑ�����
void CJson::SavejsonData(std::string FileName, Json& SaveData)
{
	std::ofstream WriteData(FileName + ".json");
	
	//ostringstream��json�f�[�^�𕶎���Ŏ擾
	std::ostringstream os;
	os << std::setw(4) << SaveData; //4�C���f���g���J����json�f�[�^���Z�b�g

	std::string OutData = os.str(); //json�f�[�^�𕶎���Ƃ��Ď擾

	//�f�[�^��ۑ�
	WriteData << OutData;

	//�t�@�C�������
	WriteData.close();
}

//�V����json�t�@�C�����쐬
void CJson::Createjson(std::string FileName)
{
	std::string Text = "New Data";
	std::ofstream OutFile;

	//json�t�@�C�����쐬
	OutFile.open(FileName + ".json", std::ios::app);
	OutFile.write(Text.data(),Text.size());
	OutFile.close();
}
