#include "BlitzSteamworks.h"
using namespace std;

CallbackHandler* CallbackHandler::instance = nullptr;

BS_API(int) Init() {
	if (SteamAPI_Init()) {
		CallbackHandler::instance = new CallbackHandler();
		return 0;
	}
	return 1;
}

BS_API(int) RestartAppIfNecessary(int appId) {
	return SteamAPI_RestartAppIfNecessary(appId);
}

BS_API(void) Update() {
	SteamAPI_RunCallbacks();
}

BS_API(void) Shutdown() {
	delete CallbackHandler::instance;

	SteamAPI_Shutdown();
}

int b = 1;

BS_API(int) Achieve(const char* apiName) {
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

BS_API(int) UnAchieve(const char* apiName) {
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

BS_API(const char*) GetGameLanguage() {
	return SteamApps()->GetCurrentGameLanguage();
}


int overlayState = 0;
int overlayStatePrev = 0;

BS_API(int) GetOverlayState() {
	return overlayState;
}

BS_API(int) GetOverlayUpdated() {
	if (overlayStatePrev == overlayState) {
		return -1;
	}
	overlayStatePrev = overlayState;
	return overlayState;
}


int idUpper(uint64 cid) {
	return cid >> 32;
}

int idLower(uint64 cid) {
	return cid & 0xffffffff;
}

uint64 idMerge(int upper, int lower) {
	return ((uint64) upper << 32) | lower;
}

BS_API(int) StringToIDUpper(const char* cid) {
	return idUpper(atoll(cid));
}

BS_API(int) StringToIDLower(const char* cid) {
	return idLower(atoll(cid));
}

BS_API(const char*) IDToString(int upperID, int lowerID) {
	return to_string(idMerge(upperID, lowerID)).c_str();
}

BS_API(int) GetPlayerIDUpper() {
	return idUpper(SteamUser()->GetSteamID().ConvertToUint64());
}

BS_API(int) GetPlayerIDLower() {
	return idLower(SteamUser()->GetSteamID().ConvertToUint64());
}

BS_API(const char*) GetPlayerName() {
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

BS_API(void) PushByte(int i) {
	Push((uint8_t) i);
}

BS_API(void) PushShort(int i) {
	Push((short) i);
}

BS_API(void) PushInt(int i) {
	Push(i);
}

BS_API(void) PushFloat(float f) {
	Push(f);
}

BS_API(void) PushString(const char* c) {
	char ch;
	do {
		ch = *c++;
		p2poutput.push_back(ch);
	} while (ch);
}

void* p2pinputstart = nullptr; // Maintained for freeing the memory block later
uint8_t* p2pinput = nullptr; // Current reading position pointer

int senderIDUpper = 0;
int senderIDLower = 0;

template <typename T>
T Pull() {
	T* t = reinterpret_cast<T*>(p2pinput);
	p2pinput += sizeof(T);
	return *t;
}

BS_API(int) PullByte() {
	return Pull<uint8_t>();
}

BS_API(int) PullShort() {
	return Pull<short>();
}

BS_API(int) PullInt() {
	return Pull<int>();
}

BS_API(float) PullFloat() {
	return Pull<float>();
}

BS_API(const char*) PullString() {
	const char* c = (const char*) p2pinput;
	while (*p2pinput++); // Move our pointer to the end of the string (after the null termination byte)
	return c;
}

BS_API(int) GetSenderIDUpper() {
	return senderIDUpper;
}

BS_API(int) GetSenderIDLower() {
	return senderIDLower;
}

BS_API(int) LoadPacket() {
	uint32 msgSize = 0;
	if (SteamNetworking()->IsP2PPacketAvailable(&msgSize)) {
		if (p2pinputstart != nullptr) {
			free(p2pinputstart);
			p2pinputstart = nullptr;
		}
		p2pinputstart = malloc(msgSize);
		CSteamID steamIDRemote;
		uint32 bytesRead = 0;
		if (SteamNetworking()->ReadP2PPacket(p2pinputstart, msgSize, &bytesRead, &steamIDRemote)) {
			p2pinput = (uint8_t*) p2pinputstart;
			uint64 id = steamIDRemote.ConvertToUint64();
			senderIDUpper = idUpper(id);
			senderIDLower = idLower(id);
			return 1;
		} else {
			return -1;
		}
	} else {
		return 0;
	}
}

BS_API(int) SendPacketToUser(int upperID, int lowerID) {
	bool b = SteamNetworking()->SendP2PPacket(idMerge(upperID, lowerID), p2poutput.data(), p2poutput.size(), k_EP2PSendUnreliable);
	p2poutput.clear();
	return b;
}

BS_API(int) CloseConnection(int upperID, int lowerID) {
	return SteamNetworking()->CloseP2PSessionWithUser(idMerge(upperID, lowerID));
}

BS_API(void) OpenOnScreenKeyboard(int mode, int x, int y, int width, int height) {
	SteamUtils()->ShowFloatingGamepadTextInput(static_cast<EFloatingGamepadTextInputMode>(mode), x, y, width, height);
}

BS_API(void) CloseOnScreenKeyboard() {
	SteamUtils()->DismissFloatingGamepadTextInput();
}

BS_API(const char*) EE(const char* cid) {
	P2PSessionState_t p2pSessionState;
	if (SteamNetworking()->GetP2PSessionState((uint64) std::atoll(cid), &p2pSessionState)) {
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
	SteamNetworking()->AcceptP2PSessionWithUser(callback->m_steamIDRemote);
}

int c = -1;
void CallbackHandler::handleP2PSessionConnectFail(P2PSessionConnectFail_t* callback) {
	c = callback->m_eP2PSessionError;
}
