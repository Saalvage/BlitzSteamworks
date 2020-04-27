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

PublishedFileId_t pubFileId;
CreateItemResult_t result;
UGCHandle_t hUpdate;

BS_API int CreateItem() {
	if (b != 0) {
		SteamAPICall_t hSteamWorkshop = SteamUGC()->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity);
		if (result.m_eResult == k_EResultOK) {
			return 0;
			pubFileId = result.m_nPublishedFileId;
		}
		else {
			return -1;
		}
	}
	return b == 1 ? b : 3;
}

BS_API void UploadItem(const char *path) {
	if (b != 0) {
		SteamAPICall_t hItemUpdate = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), pubFileId);
		SteamUGC()->SetItemTitle(hUpdate, "Testing mod");
		SteamUGC()->SetItemDescription(hUpdate, "Something interesting put here.");
		SteamUGC()->SetItemContent(hUpdate, path);
		SteamAPICall_t hItemSubmit = SteamUGC()->SubmitItemUpdate(hUpdate, "");
	}
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
