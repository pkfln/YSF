#include "../Natives.h"
#include "../includes/platform.h"
#include "../CPlugin.h"
#include "../CScriptParams.h"
#include "../Hooks.h"
#include "../Globals.h"
#include "../Utils.h"
#include "../RPCs.h"

namespace Natives
{
	// This function based on maddinat0r's function - Thanks (MySQL plugin/CCallback.cpp)
	// native CallFunctionInScript(const scriptname[], const function[], const format[], {Float,_}:...);
	AMX_DECLARE_NATIVE(CallFunctionInScript)
	{
		if (CScriptParams::Get()->Setup(3, "CallFunctionInScript", CScriptParams::Flags::MORE_PARAMETER_ALLOWED, amx, params)) return CScriptParams::Get()->HandleError();

		std::string scriptname, function, formatparams;
		CScriptParams::Get()->Read(scriptname, function, formatparams);

		AMX* pAMX = nullptr;
		if (scriptname == "GameMode")
		{
			pAMX = &pNetGame->pGameModePool->amx;
		}
		else
		{
			for (BYTE i = 0; i != 16; ++i)
			{
				if (scriptname == pNetGame->pFilterScriptPool->szFilterScriptName[i])
				{
					pAMX = pNetGame->pFilterScriptPool->pFilterScripts[i];
					break;
				}
			}
		}

		if (pAMX == nullptr)
		{
			logprintf("script \"%s\" does not exist", scriptname.c_str());
			return 0;
		}

		int cb_idx = -1;
		if (amx_FindPublic(pAMX, function.c_str(), &cb_idx) != AMX_ERR_NONE)
		{
			logprintf("callback \"%s\" does not exist", function.c_str());
			return 0;
		}

		size_t len = formatparams.length();
		char* format = new char[len + 1];
		strcpy(format, formatparams.c_str());

		const size_t param_offset = 4;
		const size_t num_params = len;

		if ((params[0] / sizeof(cell) - (param_offset - 1)) != num_params)
		{
			logprintf("parameter count does not match format specifier length %d - %d", num_params, (params[0] / sizeof(cell) - (param_offset - 1)));
			delete[] format;
			return 0;
		}

		cell param_idx = len - 1;
		cell *address_ptr = nullptr;
		cell *array_addr_ptr = nullptr;
		cell amx_address = -1;
		do
		{
			cell tmp_addr;
			switch (*(format + (len - 1)))
			{
			case 'd': //decimal
			case 'i': //integer
			case 'b':
			case 'f':
			{
				amx_GetAddr(amx, params[param_offset + param_idx], &address_ptr);
				cell value = *address_ptr;
				amx_Push(pAMX, value);
			}
			break;
			case 's': //string
			{
				char *str = nullptr;
				amx_StrParam(amx, params[param_offset + param_idx], str);

				if (str == nullptr)
				{
					str = new char[5];
					strcpy(str, "NULL");
				}

				amx_PushString(pAMX, &tmp_addr, nullptr, str, 0, 0);

				delete[] str;
			}
			break;
			case 'a': //array
			{
				cell *arraySize;
				amx_GetAddr(amx, params[param_offset + param_idx], &array_addr_ptr);
				if (amx_GetAddr(amx, params[param_offset + (param_idx + 1)], &arraySize) != AMX_ERR_NONE)
				{
					logprintf("missing 'd' / 'i' specifier for array size");
					return 0;
				}

				if ((*(format + (len))) != 'd' && (*(format + (len))) != 'i')
				{
					logprintf("expected 'd'/'i' specifier for array size (got '%c' instead)", *(format + (len)));
					return 0;
				}

				if (arraySize <= 0)
				{
					logprintf("invalid array size '%d'", arraySize);
					return 0;
				}

				cell *copied_array = static_cast<cell *>(malloc(*arraySize * sizeof(cell)));
				memcpy(copied_array, array_addr_ptr, *arraySize * sizeof(cell));

				amx_PushArray(pAMX, &tmp_addr, nullptr, copied_array, *arraySize);
				free(copied_array);

				if (amx_address < 0)
					amx_address = tmp_addr;

				array_addr_ptr = nullptr;
			}
			break;
			default:
			{
				logprintf("invalid format specifier '%c'", *(format + (len - 1)));
				return 0;
				break;
			}
			}
			param_idx--;
			len--;
		} while (len);

		cell ret;
		amx_Exec(pAMX, &ret, cb_idx);

		if (amx_address >= 0)
			amx_Release(pAMX, amx_address);

		delete[] format;
		return ret;
	}

	// native IsValidNickName(name[]);
	AMX_DECLARE_NATIVE(IsValidNickName)
	{
		CHECK_PARAMS(1, LOADED);

		std::string name;
		CScriptParams::Get()->Read(name);
		return CPlugin::Get()->IsValidNick(const_cast<char*>(name.c_str()));
	}

	// native AllowNickNameCharacter(character, bool:allow);
	AMX_DECLARE_NATIVE(AllowNickNameCharacter)
	{
		CHECK_PARAMS(2, LOADED);

		const char character = static_cast<const char>(params[1]);

		// Enable %s is disallowed
		if (character == '%') return 0;

		CPlugin::Get()->AllowNickNameCharacter(character, static_cast<int>(params[2]) != 0);
		return 1;
	}

	// native GetActiveTimers();
	AMX_DECLARE_NATIVE(GetRunningTimers)
	{
		if (!CPlugin::Get()->IsInitialized()) return std::numeric_limits<int>::lowest(); // If unknown server version

		return pNetGame->pScriptTimers->dwTimerCount;
	}

	// native SendClientMessagef(playerid, color, const message[], {Float,_}:...);
	AMX_DECLARE_NATIVE(SendClientMessagef)
	{
		if (!CPlugin::Get()->IsInitialized()) return std::numeric_limits<int>::lowest(); // If unknown server version

		const int playerid = static_cast<int>(params[1]);
		if (!IsPlayerConnected(playerid)) return 0;

		int len;
		char* szMessage = CSAMPFunctions::format_amxstring(amx, params, 3, len);
		if (!szMessage) return 0;

		RakNet::BitStream bsParams;
		bsParams.Write((DWORD)params[2]);
		bsParams.Write((DWORD)len);
		bsParams.Write(szMessage, len);
		CSAMPFunctions::RPC(&RPC_ClientMessage, &bsParams, HIGH_PRIORITY, RELIABLE_ORDERED, 0, CSAMPFunctions::GetPlayerIDFromIndex(playerid), false, false);
		return 1;
	}

	// native SendClientMessageToAllf(color, const message[], {Float,_}:...);
	AMX_DECLARE_NATIVE(SendClientMessageToAllf)
	{
		if (!CPlugin::Get()->IsInitialized()) return std::numeric_limits<int>::lowest(); // If unknown server version

		int len;
		char* szMessage = CSAMPFunctions::format_amxstring(amx, params, 2, len);
		if (!szMessage) return 0;

		RakNet::BitStream bsParams;
		bsParams.Write((DWORD)params[1]);
		bsParams.Write((DWORD)len);
		bsParams.Write(szMessage, len);
		CSAMPFunctions::RPC(&RPC_ClientMessage, &bsParams, HIGH_PRIORITY, RELIABLE_ORDERED, 0, UNASSIGNED_PLAYER_ID, true, false);
		return 1;
	}

	// native SendRconCommandf(command[], {Float,_}:...);
	AMX_DECLARE_NATIVE(SendRconCommandf)
	{
		if (!CPlugin::Get()->IsInitialized()) return std::numeric_limits<int>::lowest(); // If unknown server version

		int len;
		char* szMessage = CSAMPFunctions::format_amxstring(amx, params, 1, len);
		if (!szMessage) return 0;

		CSAMPFunctions::Execute(szMessage);
		return 1;
	}
}

static AMX_NATIVE_INFO native_list[] =
{
	// Nick name
	AMX_DEFINE_NATIVE(IsValidNickName)	// R8
	AMX_DEFINE_NATIVE(AllowNickNameCharacter) // R7
	//AMX_DEFINE_NATIVE(IsNickNameCharacterAllowed) // R7

	// Player classes
	//AMX_DEFINE_NATIVE(GetAvailableClasses) // R6
	//AMX_DEFINE_NATIVE(RemoveLastClass) // R16
	//AMX_DEFINE_NATIVE(GetPlayerClass) // R6
	//AMX_DEFINE_NATIVE(EditPlayerClass) // R6

	// Generic
	//AMX_DEFINE_NATIVE(SetModeRestartTime)
	//AMX_DEFINE_NATIVE(GetModeRestartTime)
	//AMX_DEFINE_NATIVE(SetMaxPlayers) // R8
	//AMX_DEFINE_NATIVE(SetMaxNPCs) // R8
	//AMX_DEFINE_NATIVE(GetSyncBounds) // R19
	//AMX_DEFINE_NATIVE(SetSyncBounds) // R19

	//AMX_DEFINE_NATIVE(GetFilterScriptCount)
	//AMX_DEFINE_NATIVE(GetFilterScriptName)

	//AMX_DEFINE_NATIVE(AddServerRule)
	//AMX_DEFINE_NATIVE(SetServerRule)
	//AMX_DEFINE_NATIVE(SetServerRuleInt)
	//AMX_DEFINE_NATIVE(IsValidServerRule)
	//AMX_DEFINE_NATIVE(RemoveServerRule) // Doesn't work!
	//AMX_DEFINE_NATIVE(SetServerRuleFlags)
	//AMX_DEFINE_NATIVE(GetServerRuleFlags)

	// Server settings
	//AMX_DEFINE_NATIVE(GetServerSettings)
	//AMX_DEFINE_NATIVE(GetNPCCommandLine) // R19

	// RCON Commands
	//AMX_DEFINE_NATIVE(ChangeRCONCommandName) // R19
	//AMX_DEFINE_NATIVE(GetRCONCommandName) // R19

	// Per AMX function calling
	AMX_DEFINE_NATIVE(CallFunctionInScript) // R19
	
	// Timers
	AMX_DEFINE_NATIVE(GetRunningTimers) // R8
	
	// Recording functions
	//AMX_DEFINE_NATIVE(SetRecordingDirectory) // R17
	//AMX_DEFINE_NATIVE(GetRecordingDirectory) // R17

	// Format functions
	AMX_DEFINE_NATIVE(SendClientMessagef)
	AMX_DEFINE_NATIVE(SendClientMessageToAllf)
	//AMX_DEFINE_NATIVE(GameTextForPlayerf)
	//AMX_DEFINE_NATIVE(GameTextForAllf)
	//AMX_DEFINE_NATIVE(SendPlayerMessageToPlayerf)
	//AMX_DEFINE_NATIVE(SendPlayerMessageToAllf)
	AMX_DEFINE_NATIVE(SendRconCommandf)
};

void ServerLoadNatives()
{
	RegisterNatives(native_list);
}