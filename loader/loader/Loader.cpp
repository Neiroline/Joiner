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
#include <string>
//Несколько макросов представленны Мъщых
#define Is2power(x) (!(x & (x - 1)))
#define ALIGN_DOWN(x, align) (x & ~(align - 1))
#define ALIGN_UP(x, align) ((x & (align - 1)) ? ALIGN_DOWN(x, align) + align : x)



int main(int argc,char* argv[])
{
//если аргумент не передали - выведем пример использования и выйдем
	
	if (argc!=3){
		cout<<"Example: loader file1 file2"<<endl<<"file1-program"<<endl<<"file2-inject program";
		return 0;
	}

//откроем файл формата PE в бинарном режиме
	std::fstream pefile1;
	pefile1.open(argv[1]);
	
	if(!pefile1.is_open())
	{
//если вдруг его открыть не удалось, то выведем ошибку и выйдем
		cout << "Can't open file" << std::endl;
		return 0;
	}
    
//определим размер файла, он нам пригодится дальше
	pefile1.seekg(0, pefile1.end);
//для этого переведем файловый указатель чтения в самый конец файла, получим его позицию
//	long filesize1 = pefile1.tellg();
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
	
	char* buffer=NULL;
	
	std::fstream  pefile2;
	pefile2.open(argv[2]);
	if (pefile2.is_open())
  {
	//определим размер файла
		pefile2.seekg(0,pefile2.end);
	//для этого переведем файловый указатель чтения в самый конец файла, получим его позицию
		std::streamoff filesize2 = pefile2.tellg();//это и будет размер файла в байтах
		cout<<"filesize= "<<filesize2<<endl;
		pefile2.seekg(0);
		buffer=new char[filesize2]; //выделяю память под массив		
		pefile2.read(buffer,filesize2);//читаем файл в буфер 
		cout<<"buffer= "<<buffer<<endl;//DEBUG!	

		//**************************проверяю не склеивался ли файл ранее**********************************************
		int position=0;//позиция для DEBUG пригодится
		pefile1.seekg( 100 , ios::beg );//поместим файловый за пределы первого заголовка MZ(отбалды написал 100 - главно чтобы не попасть в первый заголовок)
		position=pefile1.tellg();
		char flag1[]="MZ"; //эталон поиска
		char word[]="00"; //то с чем будет сравниваться
		
		while (! pefile1.eof())// не достигает конца не понимаю почему ----смотреть ниже причину

	   {
		  
		  pefile1>>word; //читаем по 2 символа -Как то черта первый раз записывает слово run!! -должен записать ru
		  if (word==flag1)//сравниваем на наличие MZ и если он есть то выходим из программы
			  {	   
			  cout << "Including is alredy ok";//если подключен то все завершаем и закрываем
			  pefile2.close();
			  pefile1.close();
			  delete[] buffer;
			  system("pause");
			  return 0;
			  }
		  else
			  {
				pefile1.seekg(position+1,ios::beg);//смещение от текущего положения(просто смещение на 1 - не играет я думаю роли)
				position=pefile1.tellg();//DEBUG смотрю движуху в памяти по файлу, после шапки DOS перестает двигаться и впадает в бесконечный цикл (ПРОБЛЕМА! ЗДЕСЬ!)
										 // проблема может в том что я не правильно организовал перебор по файлу(все таки бинарный) - хотя ему поидее должно быть пофиг

			  }
	   }
		//***********************************************продолжение склейки******************************************************************
		
		pefile1.seekg(0,pefile1.end);//ставим в конец указатель
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