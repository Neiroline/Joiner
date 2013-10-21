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
#include <string>

using namespace std;

//��������� �������� ������������� �����
#define Is2power(x) (!(x & (x - 1)))
#define ALIGN_DOWN(x, align) (x & ~(align - 1))
#define ALIGN_UP(x, align) ((x & (align - 1)) ? ALIGN_DOWN(x, align) + align : x)



int main(int argc,char* argv[])
{
//���� �������� �� �������� - ������� ������ ������������� � ������
	if (argc==4)
	{
		goto jump;//�� ���� ������� � �������� ����!))) ����������� �����
	}
	if (!argc==3){
		cout<<"Example: loader file1 file2"<<endl<<"file1-program"<<endl<<"file2-inject program"<<endl<<--------------------------OR------------------------<<endl<<"Example2:loader file1 file2 return";
		return 0;
	}
jump:

//������� ���� ������� PE � �������� ������
	std::fstream pefile1;
	pefile1.open(argv[1],std::ios::binary| std::ios_base::out | std::ios_base::in);
	
	if(!pefile1.is_open())
	{
//���� ����� ��� ������� �� �������, �� ������� ������ � ������
		cout << "Can't open file" << std::endl;
		return 0;
	}
    
//��������� ������ �����, �� ��� ���������� ������
	pefile1.seekg(0, std::ios::end);
//��� ����� ��������� �������� ��������� ������ � ����� ����� �����, ������� ��� �������
std::streamoff filesize1 = pefile1.tellg();
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



	//************************************2 ����***************************************************************************
	
	char* buffer=NULL;
	char* buffer1=NULL;
	

	
	if (argc==4 && !strcmp(argv[3], "return")) //������ ������ ��������(����� ������� ��������-������ ��������)
		{  
			
			std::streamoff filesize2 = 37376;//������ ������� ��� �����
			buffer1=new char[filesize2];//������ ��� ������1
			pefile1.seekg(-filesize2, ios_base::end);//��������� ��������� �� ����� �� ������ 2 �����
			pefile1.read(buffer1,filesize2);//������ ����2 ��� ��� �������))
			ofstream ("include.exe");//������� ���� ��� ����������(���������������)
			std::fstream pefile3;
			pefile3.open("include.exe",std::ios::binary | std::ios_base::out | std::ios_base::in);//������� ������ ��� ��������� ����
			pefile3.seekg(0,std::ios::beg);
			pefile3.write(buffer1,filesize2) ;//������� � ���� ���������� ������� 1(������� �� ����� ���������)
			pefile3.close();//������ ���� �����
			delete[] buffer1;
			pefile1.close();
			
			delete[] buffer;
			cout<<"Secret is created"<<endl;
			system ("pause");
			return 0;
		}
		else
		{
			std::fstream  pefile2;
			pefile2.open(argv[2],std::ios::binary | std::ios_base::out | std::ios_base::in);
			//��������� ������ �����
			pefile2.seekg(0,std::ios::end);
			//��� ����� ��������� �������� ��������� ������ � ����� ����� �����, ������� ��� �������
			std::streamoff filesize2 = pefile2.tellg();//��� � ����� ������ ����� � ������
			cout<<"filesize2= "<<filesize2<<endl;
			if (pefile2.is_open())
		  {
			
				pefile2.seekg(0);
				buffer=new char[filesize2]; //������� ������ ��� ������		
				pefile2.read(buffer,filesize2);//������ ���� � ����� 
				cout<<"buffer= "<<buffer<<endl;//DEBUG!	

				
				
				pefile1.seekg(0,std::ios::end);//������ � ����� ���������
				pefile1.write(buffer,filesize2) ;//������� �� ������ � ���� 1
				
				pefile2.close();
				std::remove("include.exe");//������ � �������� �����
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
}