//Sony
#include <sys/prx.h>
#include <sys/syscall.h>
#include <sys/ppu_thread.h>
#include <sys/sys_time.h>
#include <sys/time_util.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/timer.h>
#include <sys/return_code.h>
#include <sys/prx.h>
#include <sys/paths.h>
#include <cellstatus.h>
#include <cell/error.h>
#include <string.h>

//C-Headers (converted)
#include <cstdio>
#include <cstdlib>
#include <cwchar>
#include <cstdarg>
#include <cassert>
#include <cstddef>
#include <cmath>
#include <ctime>

//C++ Std Lib
#include <typeinfo>
#include <vector>

//C-Headers
#include <pthread.h>
#include <locale.h>


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