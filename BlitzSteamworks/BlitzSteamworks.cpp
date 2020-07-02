#include "BlitzSteamworks.h"
using namespace std;

static CallbackHandler* callbackHandler = nullptr;

BS_API int Init() {
	if (SteamAPI_Init()) {
		if (SteamUserStats()->RequestCurrentStats()) {
			callbackHandler = new CallbackHandler();
			return 0;
		}
		return 2;
	}

	return 1;
}

BS_API void Update() {
	SteamAPI_RunCallbacks();
}

BS_API void Shutdown() {
	delete callbackHandler;

	SteamAPI_Shutdown();
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

BS_API const char* GetEpic() {
	uint32 msgSize = 0;
	while (SteamNetworking()->IsP2PPacketAvailable(&msgSize)) {
		void* packet = malloc(msgSize);
		CSteamID steamIDRemote;
		uint32 bytesRead = 0;
		if (SteamNetworking()->ReadP2PPacket(packet, msgSize, &bytesRead, &steamIDRemote)) {
			// message dispatch code goes here
		}
		free(packet);
		abort();
	}
	return "";
}

BS_API void DoEpic() {
	uint64 cid = 76561198087228694;
	const char* s = "LOL";
	if (!SteamNetworking()->SendP2PPacket(cid, s, sizeof(s), k_EP2PSendReliable)) {
		abort();
	}
}

int c = -1;

#include <string>
BS_API const char* EE() {
	P2PSessionState_t p2pSessionState;
	uint64 cid = 76561198087228694;
	if (SteamNetworking()->GetP2PSessionState(cid, &p2pSessionState)) {
		string s = (to_string(p2pSessionState.m_bConnecting));
		s.append("  ");
		s.append(to_string(p2pSessionState.m_bConnectionActive));
		s.append("  ");
		s.append(to_string(p2pSessionState.m_bUsingRelay));
		s.append("  ");
		s.append(to_string(p2pSessionState.m_eP2PSessionError));
		s.append("  ");
		s.append(to_string(p2pSessionState.m_nBytesQueuedForSend));
		s.append("  ");
		s.append(to_string(p2pSessionState.m_nPacketsQueuedForSend));
		s.append("  ");
		s.append(to_string(p2pSessionState.m_nRemoteIP));
		s.append("  ");
		s.append(to_string(p2pSessionState.m_nRemotePort));
		return s.c_str();
	}
	return "";
}

void CallbackHandler::handleUserStatsReceived(UserStatsReceived_t* callback) {
	b = -static_cast<int>(callback->m_eResult);
}

void CallbackHandler::handleGameOverlayActivated(GameOverlayActivated_t* callback) {
	overlayState = callback->m_bActive;
}

void CallbackHandler::handleP2PSessionRequest(P2PSessionRequest_t* callback) {
	// TODO accept everything for now!
	// CRASH IF WE GET A FUCKING REQUEST
	abort();
	SteamNetworking()->AcceptP2PSessionWithUser(callback->m_steamIDRemote);
}

void CallbackHandler::handleP2PSessionConnectFail(P2PSessionConnectFail_t* callback) {
	abort();
	c = callback->m_eP2PSessionError;
}