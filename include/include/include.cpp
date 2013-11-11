

#include <windows.h>
#pragma code_seg(push, ".text$aaa")
/*
функция GetBase, которая берет адрес где-то в kernel32.dll, 
листает страницы назад, пока не наткнется на MZ и PE-заголовки,
что будет означать, что она, наконец, дошла до базового адреса загрузки kernel32.dll.
*/
void* GetBase(DWORD lpAddr)//поиск KERNELBASE32.DLL для использования всех возможностей внедряемого кода
{
	for(lpAddr &= 0xFFFF0000 /* гранулярность выделения памяти */ ; ; lpAddr -= 0x10000 /* листаем назад страницами */)
	{
		if( ( ((IMAGE_DOS_HEADER*)lpAddr)->e_magic == 0x5a4d ) // MZ проверяем
		 && ( ((IMAGE_NT_HEADERS*) ((DWORD)lpAddr + ((IMAGE_DOS_HEADER*)lpAddr)->e_lfanew) )->Signature == 0x00004550 ) // PE проверяем
		 )
		 return (void*)lpAddr; // если все да то это это заветная база
	}
	// Ни фига =(
	return NULL;
}

DWORD __stdcall ShellCode_Start( void GetBase(DWORD lpAddr))//входные данные - вызов kernel
{
	
	const TCHAR name[] = {'I', 'T', 'i', 's', 'H', 'A', 'C', 'K',0};
	HANDLE h = GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD Count=0;
 	int MsgLen = 9;
	
	return WriteConsole(h,name,MsgLen,&Count,NULL);//использую как аналог printf для отказа от CRT
	
}
#pragma code_seg(pop)

// код будет в .text секции, но будет иметь метку aab
#pragma code_seg(push, ".text$aab")

#pragma code_seg(pop)
