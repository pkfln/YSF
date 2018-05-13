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
*  the Initial Developer. All Rights Reserved. The development was abandobed
*  around 2010, afterwards kurta999 has continued it.
*
*  Contributor(s):
*
*	0x688, balika011, Gamer_Z, iFarbod, karimcambridge, Mellnik, P3ti, Riddick94
*	Slice, sprtik, uint32, Whitetigerswt, Y_Less, ziggi and complete SA-MP community
*
*  Special Thanks to:
*
*	SA:MP Team past, present and future
*	Incognito, maddinat0r, OrMisicL, Zeex
*
*/

#include "main.h"

//#define print_addresses
#ifdef _WIN32
DWORD CAddress::FUNC_Logprintf_03Z = 0x00486CB0;
DWORD CAddress::FUNC_Logprintf_03ZR2_2 = 0x00487310;
DWORD CAddress::FUNC_Logprintf_03ZR3 = 0x00487460;
DWORD CAddress::FUNC_Logprintf_03ZR4 = 0x004875F0;
DWORD CAddress::FUNC_Logprintf_037 = 0x0048A0B0;
DWORD CAddress::FUNC_Logprintf_037_R2_1 = 0x0048C8D0;
DWORD CAddress::FUNC_Logprintf_03DL_R1 = 0x00491FA0;
#else
DWORD CAddress::FUNC_Logprintf_03Z = 0x080A7440;
DWORD CAddress::FUNC_Logprintf_03ZR2_2 = 0x080A77D0;
DWORD CAddress::FUNC_Logprintf_03ZR3 = 0x080A78E0;
DWORD CAddress::FUNC_Logprintf_03ZR4 = 0x80A7A90;
DWORD CAddress::FUNC_Logprintf_037 = 0x080A9000;
DWORD CAddress::FUNC_Logprintf_037_R2_1 = 0x080A91D0;
DWORD CAddress::FUNC_Logprintf_03DL_R1 = 0x080B1CA0;
#endif

// Variables
DWORD CAddress::VAR_pRestartWaitTime = NULL;
DWORD CAddress::VAR_pPosSyncBounds[4];
DWORD CAddress::VAR_wRCONUser = NULL;
DWORD CAddress::ARRAY_ConsoleCommands = NULL;

// Functions
DWORD CAddress::FUNC_CConsole__AddStringVariable = NULL;
DWORD CAddress::FUNC_CConsole__GetStringVariable = NULL;
DWORD CAddress::FUNC_CConsole__SetStringVariable = NULL;
DWORD CAddress::FUNC_CConsole__GetIntVariable = NULL;
DWORD CAddress::FUNC_CConsole__SetIntVariable = NULL;
DWORD CAddress::FUNC_CConsole__GetBoolVariable = NULL;
DWORD CAddress::FUNC_CConsole__ModifyVariableFlags = NULL;
DWORD CAddress::FUNC_CConsole__FindVariable = NULL;
DWORD CAddress::FUNC_CConsole__SendRules = NULL;
DWORD CAddress::FUNC_CConsole__Execute = NULL;

DWORD CAddress::FUNC_CNetGame__SetWeather = NULL;
DWORD CAddress::FUNC_CNetGame__SetGravity = NULL;

DWORD CAddress::FUNC_CFilterscripts__LoadFilterscript = NULL;
DWORD CAddress::FUNC_CFilterscripts__UnLoadFilterscript = NULL;
DWORD CAddress::FUNC_ContainsInvalidChars = NULL;

DWORD CAddress::FUNC_CPlayer__SpawnForWorld = NULL;
DWORD CAddress::FUNC_CVehicle__Respawn = NULL;
DWORD CAddress::FUNC_CPlayerPool__HandleVehicleRespawn = NULL;
DWORD CAddress::FUNC_CObject__SpawnForPlayer = NULL;

DWORD CAddress::FUNC_ProcessQueryPacket = NULL;
DWORD CAddress::FUNC_Packet_WeaponsUpdate = NULL;
DWORD CAddress::FUNC_Packet_StatsUpdate = NULL;
DWORD CAddress::FUNC_format_amxstring = NULL;

// Others
DWORD CAddress::ADDR_GetNetworkStats_VerbosityLevel = NULL;
DWORD CAddress::ADDR_GetPlayerNetworkStats_VerbosityLevel = NULL;

DWORD CAddress::ADDR_RecordingDirectory = NULL;

// Callback hooks
DWORD CAddress::FUNC_CGameMode__OnPlayerConnect = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerDisconnect = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerSpawn = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerStreamIn = NULL;
DWORD CAddress::FUNC_CGameMode__OnPlayerStreamOut = NULL;
DWORD CAddress::FUNC_CGameMode__OnDialogResponse = NULL;

void CAddress::Initialize(SAMPVersion sampVersion)
{
	#ifdef _WIN32
		VAR_pRestartWaitTime = 0x0051C188;
		VAR_pPosSyncBounds[0] = 0x004BEEC8;
		VAR_pPosSyncBounds[1] = 0x004BEECC;
		VAR_pPosSyncBounds[2] = 0x004BEE84;
		VAR_pPosSyncBounds[3] = 0x004BF1FC;
	
		VAR_wRCONUser = 0x004EBCE4;
		ARRAY_ConsoleCommands = 0x004EA848;
	
		FUNC_CConsole__AddStringVariable = 0x00491C40;
		FUNC_CConsole__GetStringVariable = 0x00490BA0;
		FUNC_CConsole__SetStringVariable = 0x00490BC0;
		FUNC_CConsole__GetIntVariable = 0x00490C80;
		FUNC_CConsole__SetIntVariable = 0x00490CA0;
		FUNC_CConsole__GetBoolVariable = 0x00490CC0;
		FUNC_CConsole__ModifyVariableFlags = 0x00490B80;
		FUNC_CConsole__FindVariable = 0x00490AA0;
		FUNC_CConsole__SendRules = 0x00490770;
		FUNC_CConsole__Execute = 0x00490CE0;
	
		FUNC_CNetGame__SetWeather = 0x00496240;
		FUNC_CNetGame__SetGravity = 0x00496310;
	
		FUNC_CFilterscripts__LoadFilterscript = 0x0046A9D0;
		FUNC_CFilterscripts__UnLoadFilterscript = 0x0046D1C0;
	
		FUNC_ContainsInvalidChars = 0x00468EE0;
	
		FUNC_CPlayer__SpawnForWorld = 0x00487730;
		FUNC_CVehicle__Respawn = 0x00489790;
		FUNC_CPlayerPool__HandleVehicleRespawn = 0x00465E40;
		FUNC_CObject__SpawnForPlayer = 0x0049DFC0;
	
		FUNC_ProcessQueryPacket = 0x0049E480;
		FUNC_Packet_WeaponsUpdate = 0x00495940;
		FUNC_Packet_StatsUpdate = 0x00495860;
		FUNC_format_amxstring = 0x0046FD00;
	
		ADDR_GetNetworkStats_VerbosityLevel = 0x00473BEB;
		ADDR_GetPlayerNetworkStats_VerbosityLevel = 0x0047367A;
	
		ADDR_RecordingDirectory = 0x00483FC6;
	
		FUNC_CGameMode__OnPlayerConnect = 0x0046AE10;
		FUNC_CGameMode__OnPlayerDisconnect = 0x0046AE90;
		FUNC_CGameMode__OnPlayerSpawn = 0x0046DCB0;
		FUNC_CGameMode__OnPlayerStreamIn = 0x0046EBC0;
		FUNC_CGameMode__OnPlayerStreamOut = 0x0046EC30;
		FUNC_CGameMode__OnDialogResponse = 0x0046EE60;
	#else
		VAR_pRestartWaitTime = 0x08166610;
		VAR_pPosSyncBounds[0] = 0x0815C670;
		VAR_pPosSyncBounds[1] = 0x0815C66C;
		VAR_pPosSyncBounds[2] = 0x0815C678;
		VAR_pPosSyncBounds[3] = 0x0815C674;
	
		VAR_wRCONUser = 0x081A5224;
		ARRAY_ConsoleCommands = 0x081A3940;
	
		FUNC_CConsole__AddStringVariable = 0x080A8E00;
		FUNC_CConsole__GetStringVariable = 0x080A88F0;
		FUNC_CConsole__SetStringVariable = 0x080A8870;
		FUNC_CConsole__GetIntVariable = 0x080A87D0;
		FUNC_CConsole__SetIntVariable = 0x080A87A0;
		FUNC_CConsole__GetBoolVariable = 0x080A8770;
		FUNC_CConsole__ModifyVariableFlags = 0x080A8920;
		FUNC_CConsole__FindVariable = 0x080A8250;
		FUNC_CConsole__SendRules = 0x080A7CB0;
		FUNC_CConsole__Execute = 0x080A8330;
	
		FUNC_CNetGame__SetWeather = 0x080B7380;
		FUNC_CNetGame__SetGravity = 0x080B7480;
	
		FUNC_CFilterscripts__LoadFilterscript = 0x080A9980;
		FUNC_CFilterscripts__UnLoadFilterscript = 0x080A9DB0;
	
		FUNC_ContainsInvalidChars = 0x080E1220;
	
		FUNC_CPlayer__SpawnForWorld = 0x080D7870;
		FUNC_CVehicle__Respawn = 0x08157430;
		FUNC_CPlayerPool__HandleVehicleRespawn = 0x080DCAA0;
		FUNC_CObject__SpawnForPlayer = 0x080D2880;
	
		FUNC_ProcessQueryPacket = 0x080DE960;
		FUNC_Packet_WeaponsUpdate = 0x080B52A0;
		FUNC_Packet_StatsUpdate = 0x080B6080;
		FUNC_format_amxstring = 0x080E1B00;
	
		ADDR_GetNetworkStats_VerbosityLevel = 0x80E975B;
		ADDR_GetPlayerNetworkStats_VerbosityLevel = 0x80E910B;
	
		ADDR_RecordingDirectory = 0x080D7082;
	
		FUNC_CGameMode__OnPlayerConnect = 0x080ADB20;
		FUNC_CGameMode__OnPlayerDisconnect = 0x080ADB90;
		FUNC_CGameMode__OnPlayerSpawn = 0x080ADC10;
		FUNC_CGameMode__OnPlayerStreamIn = 0x080AEE10;
		FUNC_CGameMode__OnPlayerStreamOut = 0x080AEE90;
		FUNC_CGameMode__OnDialogResponse = 0x080AF110;
	#endif
/*	
	logprintf("VAR_pRestartWaitTime: %X", VAR_pRestartWaitTime);

	logprintf("FUNC_CConsole__AddStringVariable: %X", FUNC_CConsole__AddStringVariable);
	logprintf("FUNC_CConsole__GetStringVariable: %X", FUNC_CConsole__GetStringVariable);
	logprintf("FUNC_CConsole__SetStringVariable: %X", FUNC_CConsole__SetStringVariable);
	logprintf("FUNC_CConsole__GetIntVariable: %X", FUNC_CConsole__GetIntVariable);
	logprintf("FUNC_CConsole__SetIntVariable: %X", FUNC_CConsole__SetIntVariable);
	logprintf("FUNC_CConsole__GetBoolVariable: %X", FUNC_CConsole__GetBoolVariable);
	logprintf("FUNC_CConsole__FindVariable: %X", FUNC_CConsole__FindVariable);
	logprintf("FUNC_CConsole__SendRules: %X", FUNC_CConsole__SendRules);
	logprintf("FUNC_CConsole__Execute: %X", FUNC_CConsole__Execute);

	logprintf("FUNC_CNetGame__SetWeather: %X", FUNC_CNetGame__SetWeather);
	logprintf("FUNC_CNetGame__SetGravity: %X", FUNC_CNetGame__SetGravity);

	logprintf("FUNC_CFilterscripts__LoadFilterscript: %X", FUNC_CFilterscripts__LoadFilterscript);
	logprintf("FUNC_CFilterscripts__UnLoadFilterscript: %X", FUNC_CFilterscripts__UnLoadFilterscript);

	logprintf("FUNC_ContainsInvalidChars: %X", FUNC_ContainsInvalidChars);

	logprintf("FUNC_CPlayer__SpawnForWorld: %X", FUNC_CPlayer__SpawnForWorld);
	logprintf("FUNC_CVehicle__Respawn: %X", FUNC_CVehicle__Respawn);
	logprintf("FUNC_CPlayerPool__HandleVehicleRespawn: %X", FUNC_CPlayerPool__HandleVehicleRespawn);

	logprintf("FUNC_ProcessQueryPacket: %X", FUNC_ProcessQueryPacket);
	logprintf("FUNC_Packet_WeaponsUpdate: %X", FUNC_Packet_WeaponsUpdate);
	logprintf("FUNC_Packet_StatsUpdate: %X", FUNC_Packet_StatsUpdate);
	logprintf("FUNC_format_amxstring: %X", FUNC_format_amxstring);

	logprintf("ADDR_GetNetworkStats_VerbosityLevel: %X", ADDR_GetNetworkStats_VerbosityLevel);
	logprintf("ADDR_GetPlayerNetworkStats_VerbosityLevel: %X", ADDR_GetPlayerNetworkStats_VerbosityLevel);

	logprintf("FUNC_CGameMode__OnPlayerConnect: %X", FUNC_CGameMode__OnPlayerConnect);
	logprintf("FUNC_CGameMode__OnPlayerDisconnect: %X", FUNC_CGameMode__OnPlayerDisconnect);
	logprintf("FUNC_CGameMode__OnPlayerSpawn: %X", FUNC_CGameMode__OnPlayerSpawn);
	logprintf("FUNC_CGameMode__OnPlayerStreamIn: %X", FUNC_CGameMode__OnPlayerStreamIn);
	logprintf("FUNC_CGameMode__OnPlayerStreamOut: %X", FUNC_CGameMode__OnPlayerStreamOut);
	logprintf("FUNC_CGameMode__OnDialogResponse: %X", FUNC_CGameMode__OnDialogResponse);
*/

	logprintf("ARRAY_ConsoleCommands: %x", ARRAY_ConsoleCommands);
	// Unlock restart wait time
	if (VAR_pRestartWaitTime)
		Unlock((void*)VAR_pRestartWaitTime, 4);

	for (BYTE i = 0; i < 4; i++)
	{
		if (VAR_pPosSyncBounds[i])
			Unlock((void*)VAR_pPosSyncBounds[i], 4);
	}

	// Patch GetNetworkStats to get more advanced stats than default
	if(ADDR_GetNetworkStats_VerbosityLevel)
	{
#ifdef __WIN32
		Unlock((void*)ADDR_GetNetworkStats_VerbosityLevel, 2);
#else
		Unlock((void*)ADDR_GetNetworkStats_VerbosityLevel, 5);
#endif
	}

	// Patch GetPlayerNetworkStats to get more advanced stats than default
	if(ADDR_GetPlayerNetworkStats_VerbosityLevel)
	{
#ifdef __WIN32
		Unlock((void*)ADDR_GetPlayerNetworkStats_VerbosityLevel, 2);
#else
		Unlock((void*)ADDR_GetPlayerNetworkStats_VerbosityLevel, 5);
#endif
	}
}