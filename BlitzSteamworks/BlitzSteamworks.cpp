#include "BlitzSteamworks.h"
using namespace std;


BS_API int Init() {
	if (SteamAPI_Init()) {
		if (SteamUserStats()->RequestCurrentStats()) {
			new CallbackHandler();
			return 0;
		}
		return 2;
	}
	return 1;
}

BS_API void Update() {
	SteamAPI_RunCallbacks();
}


int b = 1;

BS_API int Achieve(const char* apiName) {
	if (b != 0) {
		if (SteamUserStats()->SetAchievement(apiName)) {
			if (SteamUserStats()->StoreStats()) {
				return 0;
			}
			return 2;
		}
		return 1;
	}
	return b == 1 ? b : 3;
}

BS_API int UnAchieve(const char* apiName) {
	if (b != 0) {
		if (SteamUserStats()->ClearAchievement(apiName)) {
			if (SteamUserStats()->StoreStats()) {
				return 0;
			}
			return 2;
		}
		return 1;
	}
	return b == 1 ? b : 3;
}


int overlayState = 0;
int overlayStatePrev = 0;

BS_API int GetOverlayState() {
	return overlayState;
}

BS_API int GetOverlayUpdated() {
	if (overlayStatePrev == overlayState) {
		return -1;
	}
	overlayStatePrev = overlayState;
	return overlayState;
}


BS_API int GetPlayerID() {
	return SteamUser()->GetSteamID().GetAccountID();
}

BS_API const char* GetPlayerName() {
	return SteamFriends()->GetPersonaName();
}


CallbackHandler::CallbackHandler():
	linkerUserStatsReceived(this, &CallbackHandler::handleUserStatsReceived),
	linkerGameOverlayActivated(this, &CallbackHandler::handleGameOverlayActivated)
{}

void CallbackHandler::handleUserStatsReceived(UserStatsReceived_t* callback) {
	b = -static_cast<int>(callback->m_eResult);
}

void CallbackHandler::handleGameOverlayActivated(GameOverlayActivated_t* callback) {
	overlayState = callback->m_bActive;
}
