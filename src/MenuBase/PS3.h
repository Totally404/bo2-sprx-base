#include <cellstatus.h>
#include <sys/prx.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <sys/prx.h>
#include <sys/syscall.h>
#include <sys/ppu_thread.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/types.h>
#include <math.h>
#include <fastmath.h>
#include <cellstatus.h>
#include <sys/timer.h>
#include <cell/sysmodule.h>
#include <sys/random_number.h>
#include <ppu_intrinsics.h>
#include <spu_printf.h>
#include <ctype.h>


struct opd_s
{
	uint32_t sub;
	uint32_t toc;
};

int sys_ppu_thread_exit()
{
	system_call_1(41, 0);
	return_to_user_prog(int);
}

int cstrcmp(const char* s1, const char* s2)//Please note if its equal to zero the strings are equal
{
	while(*s1 && (*s1==*s2))
		s1++,s2++;
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int console_write(const char * s)
{
	uint32_t len;
	system_call_4(403, 0, (uint64_t) s, std::strlen(s), (uint64_t) &len);
	return_to_user_prog(int);
}

int32_t write_process(uint64_t ea, const void * data, uint32_t size)
{
	system_call_4(905, (uint64_t)sys_process_getpid(), ea, size, (uint64_t)data);
	return_to_user_prog(int32_t);
}

sys_ppu_thread_t create_thread(void (*entry)(uint64_t), int priority, size_t stacksize, const char* threadname, sys_ppu_thread_t tid)
{	
	if(sys_ppu_thread_create(&tid, entry, 0, priority , stacksize, 0, threadname) != CELL_OK)
	{
		console_write("Thread creation failed\n");	
	}
	else
	{
		console_write("Thread created\n");	
	}	
    return tid;
}

void sleep(usecond_t time)  //1 second = 1000
{
	sys_timer_usleep(time * 1000);
}

namespace PS3
{
	float floatArray[100];
    float* ReadFloat(int address, int length)
    {
        for (int i = 0; i < length; i++)
        {
            floatArray[i] =  *(float*)(address + (i * 0x04));
        }
        return floatArray;
    }

	char byteArray[100];
	char* ReadBytes(int address, int length)
    {
        for (int i = 0; i < length; i++)
        {
            byteArray[i] =  *(char*)(address + (i));
        }
        return byteArray;
    }

	void WriteFloat(int address, float* input, int length)
	{
		for (int i = 0; i < length; i++)
		{
			*(float*)(address + (i * 4)) = input[i];
		}
	}

	void WriteBytes(int address, char* input, int length)
    {
        for (int i = 0; i < length; i++)
        {
            *(char*)(address + (i)) = input[i];
        }
    }

	float intArray[100];
    float* ReadInt(int address, int length)
    {
        for (int i = 0; i < length; i++)
        {
            intArray[i] =  *(int*)(address + (i * 0x04));
        }
        return intArray;
    }

	void WriteInt(int address, int* input, int length)
    {
        for (int i = 0; i < length; i++)
        {
            *(int*)(intArray + (i * 0x04)) = input[i];
        }
    }

	void WriteString(int address, char* string)
	{
		int FreeMem = 0x1D00000;
		int strlength = std::strlen(string);
		*(char**)FreeMem = string;
		char* StrBytes = PS3::ReadBytes(*(int*)FreeMem, strlength);
		PS3::WriteBytes(address, StrBytes, strlength);
	}

	struct readstr
	{
		char returnRead[100];
	}ReturnRead[1000];
	int strcount;

	char* ReadString(int address)
	{
		strcount ++;
		memset(&ReturnRead[strcount].returnRead[0], 0, sizeof(ReturnRead[strcount].returnRead));
		int strlength = 100;
		char* StrBytes = ReadBytes(address, strlength);
		for (int i = 0; i < strlength; i++)
		{
			if (StrBytes[i] != 0x00)
				ReturnRead[strcount].returnRead[i] = StrBytes[i];
			else
				break;
		}
		return ReturnRead[strcount].returnRead;
	}
};

extern "C" {
	int _sys_printf(const char *fmt, ...);
	int _sys_snprintf(char *, size_t, const char*, ...);
	int _sys_sprintf(char *, const char*, ...);
	void* _sys_malloc(size_t size);
	void _sys_free(void *ptr);
}

#define printf _sys_printf
#define snprintf _sys_snprintf
#define sprintf _sys_sprintf
#define malloc _sys_malloc
#define free _sys_free


//Hook Functions
int Memcpy(void* destination, const void* source, size_t size)
{
	system_call_4(905, (uint64_t)sys_process_getpid(), (uint64_t)destination, size, (uint64_t)source);
	__dcbst(destination);
	__sync();
	__isync();
	return_to_user_prog(int);
}
void PatchInJump(int Address, int Destination)
{
	int FuncBytes[4];
	Destination = *(int*)Destination;
	FuncBytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF);
	if (Destination & 0x8000) FuncBytes[0] += 1;
	FuncBytes[1] = 0x396B0000 + (Destination & 0xFFFF);
	FuncBytes[2] = 0x7D6903A6;
	FuncBytes[3] = 0x4E800420;
	Memcpy((void*)Address, FuncBytes, 4 * 4);
}
void patcher(int Address, int Destination, bool Linked)
{
	int FuncBytes[4];
	Destination = *(int *)Destination;
	FuncBytes[0] = 0x3D600000 + ((Destination >> 16) & 0xFFFF);
	if (Destination & 0x8000)
		FuncBytes[0] += 1;
	FuncBytes[1] = 0x396B0000 + (Destination & 0xFFFF); // addi    %r11, %r11, dest&0xFFFF
	FuncBytes[2] = 0x7D6903A6; // mtctr    %r11
	FuncBytes[3] = 0x4E800420; // bctr
	if (Linked)
		FuncBytes[3] += 1; // bctrl
	Memcpy((void*)Address, FuncBytes, 4 * 4);
}
void hookFunctionStart(int Address, int saveStub, int Destination)
{
	saveStub = *(int*)saveStub;
	int BranchtoAddress = Address + (4 * 4);
	int StubData[8];
	StubData[0] = 0x3D600000 + ((BranchtoAddress >> 16) & 0xFFFF);
	if (BranchtoAddress & 0x8000) StubData[0] += 1;
	StubData[1] = 0x396B0000 + (BranchtoAddress & 0xFFFF);
	StubData[2] = 0x7D6903A6;
	Memcpy(&StubData[3], (void*)Address, 4 * 4);
	StubData[7] = 0x4E800420;
	Memcpy((void*)saveStub, StubData, 8 * 4);
	PatchInJump(Address, Destination);
}

//Create Thread
sys_ppu_thread_t id;
sys_ppu_thread_t create_thread(void(*entry)(uint64_t), int priority, size_t stacksize, const char* threadname)
{
	if (sys_ppu_thread_create(&id, entry, 0, priority, stacksize, 0, threadname) != CELL_OK)
	{
		console_write("Thread creation failed\n");
	}
	else
	{
		console_write("Thread created\n");
	}
	return id;
}
