

#include <windows.h>
#pragma code_seg(push, ".text$aaa")
/*
������� GetBase, ������� ����� ����� ���-�� � kernel32.dll, 
������� �������� �����, ���� �� ��������� �� MZ � PE-���������,
��� ����� ��������, ��� ���, �������, ����� �� �������� ������ �������� kernel32.dll.
*/
void* GetBase(DWORD lpAddr)//����� KERNELBASE32.DLL ��� ������������� ���� ������������ ����������� ����
{
	for(lpAddr &= 0xFFFF0000 /* ������������� ��������� ������ */ ; ; lpAddr -= 0x10000 /* ������� ����� ���������� */)
	{
		if( ( ((IMAGE_DOS_HEADER*)lpAddr)->e_magic == 0x5a4d ) // MZ ���������
		 && ( ((IMAGE_NT_HEADERS*) ((DWORD)lpAddr + ((IMAGE_DOS_HEADER*)lpAddr)->e_lfanew) )->Signature == 0x00004550 ) // PE ���������
		 )
		 return (void*)lpAddr; // ���� ��� �� �� ��� ��� �������� ����
	}
	// �� ���� =(
	return NULL;
}

DWORD __stdcall ShellCode_Start( void GetBase(DWORD lpAddr))//������� ������ - ����� kernel
{
	
	const TCHAR name[] = {'I', 'T', 'i', 's', 'H', 'A', 'C', 'K',0};
	HANDLE h = GetStdHandle( STD_OUTPUT_HANDLE );
	DWORD Count=0;
 	int MsgLen = 9;
	
	return WriteConsole(h,name,MsgLen,&Count,NULL);//��������� ��� ������ printf ��� ������ �� CRT
	
}
#pragma code_seg(pop)

// ��� ����� � .text ������, �� ����� ����� ����� aab
#pragma code_seg(push, ".text$aab")

#pragma code_seg(pop)
