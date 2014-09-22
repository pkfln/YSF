
/*  
 *  Version: MPL 1.1
 *  
 *  The contents of this file are subject to the Mozilla Public License Version 
 *  1.1 (the "License"); you may not use this file except in compliance with 
 *  the License. You may obtain a copy of the License at 
 *  http://www.mozilla.org/MPL/
 *  
 *  Software distributed under the License is distributed on an "AS IS" basis,
 *  WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 *  for the specific language governing rights and limitations under the
 *  License.
 *  
 *  The Original Code is the YSI 2.0 SA:MP plugin.
 *  
 *  The Initial Developer of the Original Code is Alex "Y_Less" Cole.
 *  Portions created by the Initial Developer are Copyright (C) 2008
 *  the Initial Developer. All Rights Reserved.
 *  
 *  Contributor(s):
 *  
 *  Peter Beverloo
 *  Marcus Bauer
 *  MaVe;
 *  Sammy91
 *  Incognito
 *  
 *  Special Thanks to:
 *  
 *  SA:MP Team past, present and future
 */

#include "main.h"
#include "Utils.h"

#ifdef WIN32
	#define WIN32_LEAN_AND_MEAN
	#define VC_EXTRALEAN
	#include <Windows.h>
	#include <Psapi.h>
#else
	#include <stdio.h>
	#include <sys/mman.h>
	#include <limits.h>
	#include <string.h>
	#include <algorithm>
	#include <unistd.h>
#endif

#ifndef PAGESIZE
	#define PAGESIZE (4096)
#endif

// Y_Less - original YSF
bool Unlock(void *address, size_t len)
{
	#ifdef WIN32
		DWORD
			oldp;
		// Shut up the warnings :D
		return !!VirtualProtect(address, len, PAGE_EXECUTE_READWRITE, &oldp);
	#else
		size_t
			iPageSize = getpagesize(),
			iAddr = ((reinterpret_cast <uint32_t>(address) / iPageSize) * iPageSize);
		return !mprotect(reinterpret_cast <void*>(iAddr), len, PROT_READ | PROT_WRITE | PROT_EXEC);
	#endif
}

// Y_Less - fixes2
void AssemblySwap(char * addr, char * dat, int len)
{
	char
		swp;
	while (len--)
	{
		swp = addr[len];
		addr[len] = dat[len];
		dat[len] = swp;
	}
}

void AssemblyRedirect(void * from, void * to, char * ret)
{
	#ifdef LINUX
		size_t
			iPageSize = getpagesize(),
			iAddr = ((reinterpret_cast <uint32_t>(from) / iPageSize) * iPageSize),
			iCount = (5 / iPageSize) * iPageSize + iPageSize * 2;
		mprotect(reinterpret_cast <void*>(iAddr), iCount, PROT_READ | PROT_WRITE | PROT_EXEC);
		//mprotect(from, 5, PROT_READ | PROT_WRITE | PROT_EXEC);
	#else
		DWORD
			old;
		VirtualProtect(from, 5, PAGE_EXECUTE_READWRITE, &old);
	#endif
	*((unsigned char *)ret) = 0xE9;
	*((char **)(ret + 1)) = (char *)((char *)to - (char *)from) - 5;
	AssemblySwap((char *)from, ret, 5);
}

bool memory_compare(const BYTE *data, const BYTE *pattern, const char *mask)
{
	for(; *mask; ++mask, ++data, ++pattern)
	{
		if(*mask == 'x' && *data != *pattern)
			return false;
	}
	return (*mask) == NULL;
}

DWORD FindPattern(char *pattern, char *mask)
{
	DWORD i;
	DWORD size;
	DWORD address;
#ifdef WIN32
	MODULEINFO info = { 0 };

	address = (DWORD)GetModuleHandle(NULL);
	GetModuleInformation(GetCurrentProcess(), GetModuleHandle(NULL), &info, sizeof(MODULEINFO));
	size = (DWORD)info.SizeOfImage;
#else
	address = 0x804b480; // around the elf base
	size = 0x8128B80 - address;
#endif
	for(i = 0; i < size; ++i)
	{
		if(memory_compare((BYTE *)(address + i), (BYTE *)pattern, mask))
			return (DWORD)(address + i);
	}
	return 0;
}

///////////////////////////////////////////////////////////////
// Hooks //
///////////////////////////////////////////////////////////////
char gContainsInvalidCharsAssembly[5];
std::vector <char> gValidNameCharacters;

bool YSF_ContainsInvalidChars(char * szString)
{
	int i = 0;
	bool bIllegal = false;

	while(*szString) 
	{
		if( (*szString >= '0' && *szString <= '9') || (*szString >= 'A' && *szString <= 'Z') || (*szString >= 'a' && *szString <= 'z')  ||
			*szString == ']' || *szString == '[' || *szString == '_'  || *szString == '$' || *szString == ':' || *szString == '=' || 
			*szString == '(' || *szString == ')' || *szString == '@' ||  *szString == '.' ) 
		{

			szString++;
		} 
		else 
		{
			if(Contains(gValidNameCharacters, *szString)) 
			{
				szString++;
			}
			else
			{
				return true;
			}

		}
	}
	return false;
}

void GetAddresses()
{	 
	// Unlock restart wait time
	Unlock((void*)CAddress::VAR_pRestartWaitTime, 4);

	// Custom name check
	AssemblyRedirect((void *)CAddress::FUNC_ContainsInvalidChars, (void *)YSF_ContainsInvalidChars, gContainsInvalidCharsAssembly);
}

unsigned long rakNet_receive_hook_return;
Packet *rakNet_receive_hook_pktptr;
unsigned char rakNet_receive_hook_packetid;

void installJump(unsigned long addr, void *func)
{
#ifdef WIN32
	unsigned long dwOld;
	VirtualProtect((LPVOID)addr, 5, PAGE_EXECUTE_READWRITE, &dwOld);
#else
	int pagesize = sysconf(_SC_PAGE_SIZE);
	void *unpraddr = (void *)(((int)addr + pagesize - 1) & ~(pagesize - 1)) - pagesize;
	mprotect(unpraddr, pagesize, PROT_WRITE);
#endif
	*(unsigned char *)addr = 0xE9;
	*(unsigned long *)((unsigned long)addr + 0x1) = (unsigned long)func - (unsigned long)addr - 5;
#ifdef WIN32
	VirtualProtect((LPVOID)addr, 5, dwOld, &dwOld);
#else
	mprotect(unpraddr, pagesize, PROT_READ | PROT_EXEC);
#endif
}

bool IsPlayerUpdatePacket(unsigned char packetId)
{
	if(packetId == ID_PLAYER_SYNC || packetId == ID_VEHICLE_SYNC || packetId == ID_PASSENGER_SYNC || packetId == ID_SPECTATOR_SYNC)
		return true;
	else
		return false;
}

void ProcessPacket()
{
#ifndef WIN32
	logprintf("ProcessPacket %x, %x", rakNet_receive_hook_pktptr, rakNet_receive_hook_packetid);
#endif
	PlayerIndex playerIndex = rakNet_receive_hook_pktptr->playerIndex;
	unsigned char packetId = rakNet_receive_hook_packetid;

	// invalid
	if(playerIndex < 0 || playerIndex > MAX_PLAYERS)
		return;

	//SYSTEMTIME time;
	//GetLocalTime(&time);
	/*
	if(packetId == ID_PLAYER_SYNC)
	{
		logprintf("ID_PLAYER_SYNC");
		RakNet::BitStream bsData( rakNet_receive_hook_pktptr->data, rakNet_receive_hook_pktptr->length / 8, false );
		BYTE bytePacketID;
		CSyncData pSyncData;

		bsData.Read(bytePacketID);
		bsData.Read((char*)&pSyncData, sizeof(pSyncData));

		if(pSyncData.byteWeapon == 44 || pSyncData.byteWeapon == 45)
		{
			pSyncData.byteWeapon = 0;
			logprintf("nightvision");
		}
	}
	*/

	//AFK
	if(IsPlayerUpdatePacket(packetId) && pPlayerData[playerIndex])
	{
		if(pNetGame->pPlayerPool->bIsPlayerConnected[playerIndex] && pNetGame->pPlayerPool->pPlayer[playerIndex]->byteState != 0 && pNetGame->pPlayerPool->pPlayer[playerIndex]->byteState != 7)
		{
			pPlayerData[playerIndex]->dwLastUpdateTick = GetTickCount();
			pPlayerData[playerIndex]->bEverUpdated = true;
		}
	}

	//logprintf("[%02d:%02d:%02d.%03d] Incoming packet - playerIndex: %d | packetId: %d", time.wHour, time.wMinute, time.wSecond, time.wMilliseconds, playerIndex, packetId);
}

#ifdef WIN32
unsigned char _declspec(naked) RakNet_Receive_Hook(void)
#else
unsigned char /*__attribute__((naked))*/ RakNet_Receive_Hook ( void )
#endif
{
#ifdef WIN32
	_asm
	{
		mov ecx, dword ptr [esi+0x10]
		mov al, byte ptr [ecx]
		/*
		push eax
		mov eax, dword ptr [edi]
		add eax, 8
		mov pInternelRakServer, eax
		pop eax
		*/
		mov rakNet_receive_hook_packetid, al
		mov rakNet_receive_hook_pktptr, esi

		pushad
	}
#else

	// hax cos the naked doesn't work :s
	__asm(
	".intel_syntax noprefix\n"
		"add esp, 8\n"
		"pop ebp\n"
	".att_syntax\n");
	/*
	__asm(
	".intel_syntax noprefix\n"
		"push eax\n"
		"mov eax, dword ptr [ebp+8]\n"
		"mov eax, dword ptr [eax]\n"
		"mov pInternelRakServer, eax\n"
		"pop eax\n"
	".att_syntax\n");
	*/
	__asm(
	".intel_syntax noprefix\n"
		"mov rakNet_receive_hook_packetid, al\n"
		"mov rakNet_receive_hook_pktptr, ebx\n"
		"pushad\n"
	".att_syntax\n");

#endif

	ProcessPacket();

#ifdef WIN32
	_asm
	{
		popad

		push eax
		mov eax, CAddress::ADDR_RECEIVE_HOOKPOS
		add eax, 5
		mov rakNet_receive_hook_return, eax
		pop eax

		jmp rakNet_receive_hook_return
	}
#else
	__asm(
	".intel_syntax noprefix\n"
		"popad\n"

		"push eax\n"
	".att_syntax\n"
	);

	__asm(
	".intel_syntax noprefix\n"
		"mov eax, %0\n"
	".att_syntax\n"
		:
		: "r"(CAddress::ADDR_RECEIVE_HOOKPOS)
	);

	__asm(
	".intel_syntax noprefix\n"
		"add eax, 8\n"
		"mov rakNet_receive_hook_return, eax\n"
		"pop eax\n"

		"movzx eax, al\n"
		"cmp eax, 0x0D8\n"

		"jmp dword ptr [rakNet_receive_hook_return]\n"
	".att_syntax\n"
	);
#endif
}

void InstallRakNetReceiveHook()
{
	installJump(CAddress::ADDR_RECEIVE_HOOKPOS, (void*)RakNet_Receive_Hook);
}

void InstallPreHooks()
{
	GetAddresses();
#ifdef WIN32
	InstallRakNetReceiveHook();
#endif
}