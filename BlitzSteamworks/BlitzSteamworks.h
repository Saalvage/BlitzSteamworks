// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the DLL1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// DLL1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BS_EXPORTS
#define BS_API(x) extern "C" __declspec(dllexport) x _stdcall
#else
#define BS_API(x) __declspec(dllimport) x _stdcall
#endif

#include "windows.h"
#include "steam_api.h"
#include <vector>
#include <string>


// Needs to be called and return 0 before being able to use the Steam API
// Returns
// 0 if successful
// 1 if SteamAPI_Init() failed
// 2 if RequestCurrentStats() failed
BS_API(int) Init();

// Updates callbacks
// Should be called every tick
BS_API(void) Update();

BS_API(void) Shutdown();

// Used to unlock achievements
// Param is the API Name of the achievement
// Returns
// 0 if successful
// 1 if the specified achievement doesn't exist (this could also mean that it simply hasn't been published yet)
// 2 if StoreStats() failed
// 3 if RequestCurrentStats() hasn't successfully returned yet
// <0 if RequestCurrentStats() has failed, the value is a code of EResult that has been negated
BS_API(int) Achieve(const char* apiName);

// Used to relock achievements, should only ever be used for testing purposes
// Param is the API Name of the achievement
// Returns
// 0 if successful
// 1 if the specified achievement doesn't exist (this could also mean that it simply hasn't been published yet)
// 2 if StoreStats() failed
// 3 if RequestCurrentStats() hasn't successfully returned yet
// <0 if RequestCurrentStats() has failed, the value is a code of EResult that has been negated
BS_API(int) UnAchieve(const char* apiName);


// Returns if the overlay is currently open
// 0 if it is closed
// 1 if it is open
BS_API(int) GetOverlayState();

// Used to check if the Overlay has been opened/closed, consumes the change
// Returns
// -1 if nothing changed
// 0 if it was just closed
// 1 if it was just opened
BS_API(int) GetOverlayUpdated();


BS_API(int) StringToIDUpper(const char* cid);
BS_API(int) StringToIDLower(const char* cid);

BS_API(const char*) IDToString(int upperID, int lowerID);

// Returns the unique unsigned 64-bit player ID as a two ints
BS_API(int) GetPlayerIDUpper();
BS_API(int) GetPlayerIDLower();

// Returns the user's public username
BS_API(const char*) GetPlayerName();


// Pushes data onto the vector
BS_API(void) PushByte(int b);
BS_API(void) PushShort(int s);
BS_API(void) PushInt(int i);
BS_API(void) PushFloat(float f);
BS_API(void) PushString(const char* c);

// Pulls data from the loaded packet
BS_API(int) PullByte();
BS_API(int) PullShort();
BS_API(int) PullInt();
BS_API(float) PullFloat();
BS_API(const char*) PullString();

BS_API(int) GetSenderIDUpper();
BS_API(int) GetSenderIDLower();

// Loads the next packet into memory
// Returns
// -1 if the packet failed to be loaded
// 0 if there was no packet available
// 1 if a packet was loaded successfully
BS_API(int) LoadPacket();

// Sends a packet to a specified Steam ID, returns if a packet has begun to send successfully
BS_API(int) SendPacketToUser(int upperID, int lowerID);

// Returns true if the connection was successfully closed and false if there existed no connection
BS_API(int) CloseConnection(int upperID, int lowerID);


class CallbackHandler {
private:
	STEAM_CALLBACK(CallbackHandler, handleUserStatsReceived, UserStatsReceived_t);
	STEAM_CALLBACK(CallbackHandler, handleGameOverlayActivated, GameOverlayActivated_t);
	STEAM_CALLBACK(CallbackHandler, handleP2PSessionRequest, P2PSessionRequest_t);
	STEAM_CALLBACK(CallbackHandler, handleP2PSessionConnectFail, P2PSessionConnectFail_t);
};

