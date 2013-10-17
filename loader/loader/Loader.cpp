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

using namespace std;

//Несколько макросов представленны Мъщых
#define Is2power(x) (!(x & (x - 1)))
#define ALIGN_DOWN(x, align) (x & ~(align - 1))
#define ALIGN_UP(x, align) ((x & (align - 1)) ? ALIGN_DOWN(x, align) + align : x)



int main(int argc,char* argv[])
{
	argv[1]="target.exe";
	argv[2]="include.exe";
//если аргумент не передали - выведем пример использования и выйдем
	
	if (argc!=3){
		cout<<"Example: loader file1 file2"<<endl<<"file1-program"<<endl<<"file2-inject program";
		return 0;
	}

//откроем файл формата PE в бинарном режиме
	std::fstream pefile1;
	pefile1.open(argv[1], std::ios::in | std::ios::binary);
	
	if(!pefile1.is_open())
	{
//если вдруг его открыть не удалось, то выведем ошибку и выйдем
		cout << "Can't open file" << std::endl;
		return 0;
	}
    
//определим размер файла, он нам пригодится дальше
	pefile1.seekg(0, pefile1.end);
//для этого переведем файловый указатель чтения в самый конец файла, получим его позицию
	long filesize1 = pefile1.tellg();
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

	//Переходим на структуру IMAGE_NT_HEADERS и готовимся считать ее
	pefile1.seekg(dos_header.e_lfanew);
	if(pefile1.bad() || pefile1.fail())
	{
		std::cout << "Cannot reach IMAGE_NT_HEADERS" << std::endl;
		return 0;
	}
 
//Читаем
//читать будем только часть структуры IMAGE_NT_HEADERS
//без дата директорий
//они нам и не понадобятся сейчас
	IMAGE_NT_HEADERS32 nt_headers;
	pefile1.read(reinterpret_cast<char*>(&nt_headers), sizeof(IMAGE_NT_HEADERS32) - sizeof(IMAGE_DATA_DIRECTORY) * 16);
	if(pefile1.bad() || pefile1.eof())
	{
		std::cout << "Error reading IMAGE_NT_HEADERS32" << std::endl;
		return 0;
	}
//Проверяем, что наш файл - PE
//сигнатура у него должна быть "PE\0\0"
//помним про обратный порядок байтов и проверяем...
	if(nt_headers.Signature != 'EP')
	{
		std::cout << "Incorrect PE signature" << std::endl;
		return 0;
	}
 
//Проверяем, что это PE32
	if(nt_headers.OptionalHeader.Magic != 0x10B)
	{
		std::cout << "This PE is not PE32" << std::endl;
		return 0;
	}

	//************************************2 файл***************************************************************************
	
	
	
	std::fstream pefile2;
	pefile2.open(argv[2], std::ios::in | std::ios::binary);
	if (pefile2.is_open())
  {
	//определим размер файла
		pefile2.seekg(0,pefile2.end);
	//для этого переведем файловый указатель чтения в самый конец файла, получим его позицию
		long filesize2 = pefile2.tellg();//это и будет размер файла в байтах
		pefile2.seekg(0);//затем вернем файловый указатель в начало файла
		char* buffer=new char[filesize2]; //выделяю память под массив		
		pefile2.read(buffer,filesize2);//читаем файл в буфер 
		cout<<"buffer= "<<buffer<<endl;//DEBUG!	
		pefile1.write(buffer,filesize2) ;//запишем из буфера в файл 1
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