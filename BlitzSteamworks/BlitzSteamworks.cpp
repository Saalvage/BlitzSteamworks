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

PublishedFileId_t PubFileId;

BS_API bool DownloadSubbedItems() {
	SteamUGC()->GetSubscribedItems(&PubFileId, SteamUGC()->GetNumSubscribedItems());

	bool dlItemHandle = SteamUGC()->DownloadItem(PubFileId, true);

	return dlItemHandle;
}

BS_API int IsStillDownloading() {
	uint32 unItemState = SteamUGC()->GetItemState(2078908559);
	if (unItemState & k_EItemStateDownloading)
	{
		return 2;
	}
	else if ((unItemState & k_EItemStateInstalled) && (unItemState & k_EItemStateNeedsUpdate))
	{
		return 1;
	}
	else {
		return 0;
	}
}

CallbackHandler::CallbackHandler() :
	linkerUserStatsReceived(this, &CallbackHandler::handleUserStatsReceived),
	linkerGameOverlayActivated(this, &CallbackHandler::handleGameOverlayActivated)
{}

void CallbackHandler::handleUserStatsReceived(UserStatsReceived_t* callback) {
	b = -static_cast<int>(callback->m_eResult);
}

void CallbackHandler::handleGameOverlayActivated(GameOverlayActivated_t* callback) {
	overlayState = callback->m_bActive;
}
