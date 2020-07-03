// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the DLL1_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// DLL1_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BS_EXPORTS
#define BS_API extern "C" __declspec(dllexport)
#else
#define BS_API __declspec(dllimport)
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
BS_API int Init();

// Updates callbacks
// Should be called every tick
BS_API void Update();

BS_API void Shutdown();

// Used to unlock achievements
// Param is the API Name of the achievement
// Returns
// 0 if successful
// 1 if the specified achievement doesn't exist (this could also mean that it simply hasn't been published yet)
// 2 if StoreStats() failed
// 3 if RequestCurrentStats() hasn't successfully returned yet
// <0 if RequestCurrentStats() has failed, the value is a code of EResult that has been negated
BS_API int Achieve(const char* apiName);

// Used to relock achievements, should only ever be used for testing purposes
// Param is the API Name of the achievement
// Returns
// 0 if successful
// 1 if the specified achievement doesn't exist (this could also mean that it simply hasn't been published yet)
// 2 if StoreStats() failed
// 3 if RequestCurrentStats() hasn't successfully returned yet
// <0 if RequestCurrentStats() has failed, the value is a code of EResult that has been negated
BS_API int UnAchieve(const char* apiName);


// Returns if the overlay is currently open
// 0 if it is closed
// 1 if it is open
BS_API int GetOverlayState();

// Used to check if the Overlay has been opened/closed, consumes the change
// Returns
// -1 if nothing changed
// 0 if it was just closed
// 1 if it was just opened
BS_API int GetOverlayUpdated();


// Returns the unique unsigned 64-bit player ID as a string
BS_API const char* GetPlayerID();

// Returns the user's public username
BS_API const char* GetPlayerName();

// Pushes data onto the vector
BS_API void PushByte(int b);
BS_API void PushShort(int s);
BS_API void PushInt(int i);
BS_API void PushFloat(float f);
BS_API void PushString(const char* c);

// Pulls data from the loaded packet
BS_API int PullByte();
BS_API int PullShort();
BS_API int PullInt();
BS_API float PullFloat();
BS_API const char* PullString();

// Loads the next packet into memory, returns if a packet was successfully loaded
BS_API bool LoadPacket();

// Sends a packet to a specified Steam ID
BS_API void SendPacketToUser(const char* cid);


class CallbackHandler {
private:
	STEAM_CALLBACK(CallbackHandler, handleUserStatsReceived, UserStatsReceived_t);
	STEAM_CALLBACK(CallbackHandler, handleGameOverlayActivated, GameOverlayActivated_t);
	STEAM_CALLBACK(CallbackHandler, handleP2PSessionRequest, P2PSessionRequest_t);
	STEAM_CALLBACK(CallbackHandler, handleP2PSessionConnectFail, P2PSessionConnectFail_t);
};

