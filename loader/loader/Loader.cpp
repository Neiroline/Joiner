//make NT



//��� ������ � ������ ����������� ���������� ������ � �������- loader file1 file2
//���������� �����-������ ��� ������ ���������� � �������
#include <iostream>
//���������� ��� ������ � �������
#include <fstream>
//��������������� ���������� ��� ������������, �������������� ������ � �.�.
#include <iomanip>
//�������, ��� ����������� ��������� �� Windows.h
//�� �����, � �����-��, �� ������ �� ��������� ����� � ��� � �������������� ��� ��� ������ 
#include <Windows.h>

using namespace std;

//��������� �������� ������������� �����
#define Is2power(x) (!(x & (x - 1)))
#define ALIGN_DOWN(x, align) (x & ~(align - 1))
#define ALIGN_UP(x, align) ((x & (align - 1)) ? ALIGN_DOWN(x, align) + align : x)



int main(int argc,char* argv[])
{
	argv[1]="target.exe";
	argv[2]="include.exe";
//���� �������� �� �������� - ������� ������ ������������� � ������
	
	if (argc!=3){
		cout<<"Example: loader file1 file2"<<endl<<"file1-program"<<endl<<"file2-inject program";
		return 0;
	}

//������� ���� ������� PE � �������� ������
	std::fstream pefile1;
	pefile1.open(argv[1], std::ios::in | std::ios::binary);
	
	if(!pefile1.is_open())
	{
//���� ����� ��� ������� �� �������, �� ������� ������ � ������
		cout << "Can't open file" << std::endl;
		return 0;
	}
    
//��������� ������ �����, �� ��� ���������� ������
	pefile1.seekg(0, pefile1.end);
//��� ����� ��������� �������� ��������� ������ � ����� ����� �����, ������� ��� �������
	long filesize1 = pefile1.tellg();
//��� � ����� ������ ����� � ������
//����� ������ �������� ��������� � ������ �����
	pefile1.seekg(0);


	IMAGE_DOS_HEADER dos_header;
	pefile1.read(reinterpret_cast<char*>(&dos_header), sizeof(IMAGE_DOS_HEADER));
	if(pefile1.bad() || pefile1.eof())
	{
//���� ����� ������� �� �������...
		std::cout << "Unable to read IMAGE_DOS_HEADER" << std::endl;
		return 0;
	}
 
//������ ��� ����� ��������� ������ ���� MZ, ��, ��� ��� � x86 � ��� �������� ������� ���������� ������,
//�� ���������� ��� ����� �� ��������� 'ZM'
	if(dos_header.e_magic != 'ZM')
	{
		std::cout << "IMAGE_DOS_HEADER signature is incorrect" << std::endl;
		return 0;
	}
 
//������ ��������� ������ PE-����� (IMAGE_NT_HEADERS) ������ ����
//��������� �� �������� �������� ����� (DWORD)
//��������, ��� ��� ���
	if((dos_header.e_lfanew % sizeof(DWORD)) != 0)
	{
//� ����� ��� PE-���� �����������
		std::cout << "PE header is not DWORD-aligned" << std::endl;
		return 0;
	}

	//��������� �� ��������� IMAGE_NT_HEADERS � ��������� ������� ��
	pefile1.seekg(dos_header.e_lfanew);
	if(pefile1.bad() || pefile1.fail())
	{
		std::cout << "Cannot reach IMAGE_NT_HEADERS" << std::endl;
		return 0;
	}
 
//������
//������ ����� ������ ����� ��������� IMAGE_NT_HEADERS
//��� ���� ����������
//��� ��� � �� ����������� ������
	IMAGE_NT_HEADERS32 nt_headers;
	pefile1.read(reinterpret_cast<char*>(&nt_headers), sizeof(IMAGE_NT_HEADERS32) - sizeof(IMAGE_DATA_DIRECTORY) * 16);
	if(pefile1.bad() || pefile1.eof())
	{
		std::cout << "Error reading IMAGE_NT_HEADERS32" << std::endl;
		return 0;
	}
//���������, ��� ��� ���� - PE
//��������� � ���� ������ ���� "PE\0\0"
//������ ��� �������� ������� ������ � ���������...
	if(nt_headers.Signature != 'EP')
	{
		std::cout << "Incorrect PE signature" << std::endl;
		return 0;
	}
 
//���������, ��� ��� PE32
	if(nt_headers.OptionalHeader.Magic != 0x10B)
	{
		std::cout << "This PE is not PE32" << std::endl;
		return 0;
	}

	//************************************2 ����***************************************************************************
	
	
	
	std::fstream pefile2;
	pefile2.open(argv[2], std::ios::in | std::ios::binary);
	if (pefile2.is_open())
  {
	//��������� ������ �����
		pefile2.seekg(0,pefile2.end);
	//��� ����� ��������� �������� ��������� ������ � ����� ����� �����, ������� ��� �������
		long filesize2 = pefile2.tellg();//��� � ����� ������ ����� � ������
		pefile2.seekg(0);//����� ������ �������� ��������� � ������ �����
		char* buffer=new char[filesize2]; //������� ������ ��� ������		
		pefile2.read(buffer,filesize2);//������ ���� � ����� 
		cout<<"buffer= "<<buffer<<endl;//DEBUG!	
		pefile1.write(buffer,filesize2) ;//������� �� ������ � ���� 1
		pefile2.close();
		delete[] buffer;
		cout<<"including is OK"<<endl;		
	}
	else
	{
		cout<<"Failure to open pefile2";		
	}
	pefile1.close();
	system("pause");
	return 0;
}