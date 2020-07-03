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

BS_API const char* GetPlayerID() {
	return std::to_string(SteamUser()->GetSteamID().ConvertToUint64()).c_str();
}

BS_API const char* GetPlayerName() {
	return SteamFriends()->GetPersonaName();
}

std::vector<uint8_t> p2poutput(0);

template <typename T>
void Push(T t) {
	uint8_t* ptr = (uint8_t*) &t;
	for (int i = 0; i < sizeof(T); i++) {
		p2poutput.push_back(*ptr);
		ptr++;
	}
}

BS_API void PushByte(int i) {
	Push((uint8_t) i);
}

BS_API void PushShort(int i) {
	Push((short) i);
}

BS_API void PushInt(int i) {
	Push(i);
}

BS_API void PushFloat(float f) {
	Push(f);
}

BS_API void PushString(const char* c) {
	char ch;
	do {
		ch = *c++;
		p2poutput.push_back(ch);
	} while (ch);
}

void* p2pinputstart; // Maintained for freeing the memory block later
uint8_t* p2pinput; // Current reading position pointer

template <typename T>
T Pull() {
	T* t = reinterpret_cast<T*>(p2pinput);
	p2pinput += sizeof(T);
	return *t;
}

BS_API int PullByte() {
	return Pull<uint8_t>();
}

BS_API int PullShort() {
	return Pull<short>();
}

BS_API int PullInt() {
	return Pull<int>();
}

BS_API float PullFloat() {
	return Pull<float>();
}

BS_API const char* PullString() {
	const char* c = (const char*) p2pinput;
	while (*p2pinput++); // Move our pointer to the end of the string (after the null termination byte)
	return c;
}

BS_API bool LoadPacket() {
	if (p2pinputstart != nullptr) {
		free(p2pinputstart);
	}
	uint32 msgSize = 0;
	if (SteamNetworking()->IsP2PPacketAvailable(&msgSize)) {
		p2pinputstart = malloc(msgSize);
		CSteamID steamIDRemote;
		uint32 bytesRead = 0;
		if (SteamNetworking()->ReadP2PPacket(p2pinputstart, msgSize, &bytesRead, &steamIDRemote)) {
			p2pinput = (uint8_t*) p2pinputstart;
			return true;
		}
	}
	return false;
}

BS_API void SendPacketToUser(const char* cid) {
	if (!SteamNetworking()->SendP2PPacket((uint64) atoll(cid), p2poutput.data(), p2poutput.size(), k_EP2PSendUnreliable)) {
		abort();
	}
	p2poutput.clear();
}

BS_API const char* EE(const char* cid) {
	P2PSessionState_t p2pSessionState;
	/*if (SteamNetworking()->GetP2PSessionState((uint64) std::atoll(cid), &p2pSessionState)) {
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
	}*/
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
	SteamNetworking()->AcceptP2PSessionWithUser(callback->m_steamIDRemote);
}

int c = -1;
void CallbackHandler::handleP2PSessionConnectFail(P2PSessionConnectFail_t* callback) {
	c = callback->m_eP2PSessionError;
}