#include <stdio.h>
#include <stdlib.h>
#pragma comment (lib, "BeaEngine.lib")
#pragma comment (lib, "BeaEngineCheetah.lib")
#define BEA_ENGINE_STATIC
#define BEA_USE_STDCALL
#include "BeaEngine.h"
//[DllImport("BeaEngine.dll")]
/* ============================= Init datas */
DISASM MyDisasm;
int len;
bool Error = 0;
void *pBuffer;
int  (*pSourceCode) (void);     /* function pointer */
 
/* ===============================================================================*/
/*                                  
/*  Disassemble code in the specified buffer using the correct VA       
/*                                  
/* ===============================================================================*/
 
void DisassembleCode(char *StartCodeSection, 
                     char *EndCodeSection, 
                     int Virtual_Address)
{
 
/* ============================= Init the Disasm structure (important !)*/
(void) memset (&MyDisasm, 0, sizeof(DISASM));
 
/* ============================= Init EIP */
MyDisasm.EIP = (int) StartCodeSection;
/* ============================= Init VirtualAddr */
MyDisasm.VirtualAddr = (long long) Virtual_Address;//взял исходники одной программы по вычислению виртуальных адресов(и не только) всех секции в проект не включал
 
/* ============================= set IA-32 architecture */
MyDisasm.Archi = 0;
/* ============================= Loop for Disasm */
while ( !Error){
    /* ============================= Fix SecurityBlock */
    MyDisasm.SecurityBlock = (int) EndCodeSection - MyDisasm.EIP;
 
    len = Disasm(&MyDisasm);
    if (len == OUT_OF_BLOCK) {
        (void) printf("disasm engine is not allowed to read more memory \n");
        Error = 1;
    }
    else if (len == UNKNOWN_OPCODE) {
        (void) printf("unknown opcode \n");
		//MyDisasm.EIP = MyDisasm.EIP + len; //останавливается на MZP поэтому иключение убрал(просто перешагнул его)
        //MyDisasm.VirtualAddr = MyDisasm.VirtualAddr + len;
        Error = 1;
    }
    else {
            (void) printf("%.8X %s\n",(int) MyDisasm.VirtualAddr, 
                                                         &MyDisasm.CompleteInstr);
        MyDisasm.EIP = MyDisasm.EIP + len;
        MyDisasm.VirtualAddr = MyDisasm.VirtualAddr + len;
        if (MyDisasm.EIP >= (int) EndCodeSection) {
            (void) printf("End of buffer reached ! \n");
            Error = 1;
        }
    }
};
return;
}
 

