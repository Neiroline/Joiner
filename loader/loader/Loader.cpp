//make NT



//для работы с данным приложением необходимо задать в консоли- loader file1 file2
//библиотека ввода-вывода для вывода информации в консоль
#include <iostream>
//библиотека для работа с файлами
#include <fstream>
//вспомогательная библиотека для выравнивания, форматирования вывода и т.д.
#include <iomanip>
//конечно, нам потребуются структуры из Windows.h
//но ничто, в общем-то, не мешает их перенести прямо в код и скомпилировать это под линукс 
#include <Windows.h>
#include <string>

using namespace std;

//Несколько макросов представленны Мъщых
#define Is2power(x) (!(x & (x - 1)))
#define ALIGN_DOWN(x, align) (x & ~(align - 1))
#define ALIGN_UP(x, align) ((x & (align - 1)) ? ALIGN_DOWN(x, align) + align : x)



int main(int argc,char* argv[])
{
//если аргумент не передали - выведем пример использования и выйдем
	if (argc==4)
	{
		goto jump;//Не надо кричать и говорить ФУУУ!))) Деревенский выход
	}
	if (!argc==3){
		cout<<"Example: loader file1 file2"<<endl<<"file1-program"<<endl<<"file2-inject program"<<endl<<--------------------------OR------------------------<<endl<<"Example2:loader file1 file2 return";
		return 0;
	}
jump:

//откроем файл формата PE в бинарном режиме
	std::fstream pefile1;
	pefile1.open(argv[1],std::ios::binary| std::ios_base::out | std::ios_base::in);
	
	if(!pefile1.is_open())
	{
//если вдруг его открыть не удалось, то выведем ошибку и выйдем
		cout << "Can't open file" << std::endl;
		return 0;
	}
    
//определим размер файла, он нам пригодится дальше
	pefile1.seekg(0, std::ios::end);
//для этого переведем файловый указатель чтения в самый конец файла, получим его позицию
std::streamoff filesize1 = pefile1.tellg();
//это и будет размер файла в байтах
//затем вернем файловый указатель в начало файла
	pefile1.seekg(0);


	IMAGE_DOS_HEADER dos_header;
	pefile1.read(reinterpret_cast<char*>(&dos_header), sizeof(IMAGE_DOS_HEADER));
	if(pefile1.bad() || pefile1.eof())
	{
//если вдруг считать не удалось...
		std::cout << "Unable to read IMAGE_DOS_HEADER" << std::endl;
		return 0;
	}
 
//Первые два байта структуры должны быть MZ, но, так как в x86 у нас обратный порядок следования байтов,
//мы сравниваем эти байты со значением 'ZM'
	if(dos_header.e_magic != 'ZM')
	{
		std::cout << "IMAGE_DOS_HEADER signature is incorrect" << std::endl;
		return 0;
	}
 
//Начало заголовка самого PE-файла (IMAGE_NT_HEADERS) должно быть
//выровнено на величину двойного слова (DWORD)
//убедимся, что это так
	if((dos_header.e_lfanew % sizeof(DWORD)) != 0)
	{
//а иначе наш PE-файл некорректен
		std::cout << "PE header is not DWORD-aligned" << std::endl;
		return 0;
	}



	//************************************2 файл***************************************************************************
	
	char* buffer=NULL;
	char* buffer1=NULL;
	

	
	if (argc==4 && !strcmp(argv[3], "return")) //введем третии аргумент(можно сделать красивее-вопрос эстетики)
		{  
			
			std::streamoff filesize2 = 37376;//размер нужного нам файла
			buffer1=new char[filesize2];//память под буффер1
			pefile1.seekg(-filesize2, ios_base::end);//установим указатель от конца на размер 2 файла
			pefile1.read(buffer1,filesize2);//читаем файл2 что тут скажешь))
			ofstream ("include.exe");//создаем файл для заполнения(восстанавливаем)
			std::fstream pefile3;
			pefile3.open("include.exe",std::ios::binary | std::ios_base::out | std::ios_base::in);//откроем только что созданный файл
			pefile3.seekg(0,std::ios::beg);
			pefile3.write(buffer1,filesize2) ;//запишем в него содержимое буффера 1(вырезка из нашей программы)
			pefile3.close();//удалим весь мусор
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
			//определим размер файла
			pefile2.seekg(0,std::ios::end);
			//для этого переведем файловый указатель чтения в самый конец файла, получим его позицию
			std::streamoff filesize2 = pefile2.tellg();//это и будет размер файла в байтах
			cout<<"filesize2= "<<filesize2<<endl;
			if (pefile2.is_open())
		  {
			
				pefile2.seekg(0);
				buffer=new char[filesize2]; //выделяю память под массив		
				pefile2.read(buffer,filesize2);//читаем файл в буфер 
				cout<<"buffer= "<<buffer<<endl;//DEBUG!	

				
				
				pefile1.seekg(0,std::ios::end);//ставим в конец указатель
				pefile1.write(buffer,filesize2) ;//запишем из буфера в файл 1
				
				pefile2.close();
				std::remove("include.exe");//удалим с жесткого диска
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