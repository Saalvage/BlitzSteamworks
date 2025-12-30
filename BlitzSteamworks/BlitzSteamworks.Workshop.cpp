#include "BlitzSteamworks.h"

#include <filesystem>

using namespace std;

static int currentUpdateStatus = 0;
static string currentUpdateTitle;
static string currentUpdateDesc;
static string currentUpdatePath;
static string currentUpdateImgPath;
static string changelog;
static bool isCreating; // as opposed to updating
static PublishedFileId_t publishedFileId;

static void UpdateItemInternal() {
	currentUpdateStatus = 2;
	auto updateHandle = SteamUGC()->StartItemUpdate(SteamUtils()->GetAppID(), publishedFileId);
	SteamUGC()->SetItemTitle(updateHandle, currentUpdateTitle.c_str());
	SteamUGC()->SetItemDescription(updateHandle, currentUpdateDesc.c_str());
	SteamUGC()->SetItemContent(updateHandle, currentUpdatePath.c_str());
	if (!currentUpdateImgPath.empty()) {
		SteamUGC()->SetItemPreview(updateHandle, currentUpdateImgPath.c_str());
	}
	CallbackHandler::instance->updateItemCallback.Set(SteamUGC()->SubmitItemUpdate(updateHandle, changelog.c_str()), CallbackHandler::instance, &CallbackHandler::handleItemUpdateSubmitted);
}

BS_API(void) PublishItem(const char* title, const char* desc, const char* itemPath, const char* imgPath) {
	currentUpdateTitle = title;
	currentUpdateDesc = desc;
	currentUpdatePath = filesystem::absolute(itemPath).string();
	currentUpdateImgPath = filesystem::absolute(imgPath).string();
	changelog = "Initial Upload";
	isCreating = true;
	currentUpdateStatus = 1;
	CallbackHandler::instance->createItemCallback.Set(SteamUGC()->CreateItem(SteamUtils()->GetAppID(), k_EWorkshopFileTypeCommunity), CallbackHandler::instance, &CallbackHandler::handleItemCreated);
}

BS_API(void) UpdateItem(const char* publishedFileID, const char* newTitle, const char* newDesc, const char* itemPath, const char* imgPath, const char* changeLogText) {
	currentUpdateTitle = newTitle;
	currentUpdateDesc = newDesc;
	currentUpdatePath = filesystem::absolute(itemPath).string();
	currentUpdateImgPath = filesystem::absolute(imgPath).string();
	publishedFileId = atoll(publishedFileID);
	changelog = changeLogText;
	isCreating = false;
	UpdateItemInternal();
}

BS_API(int) QueryUpdateItemStatus() {
	return currentUpdateStatus;
}

static std::string ret;
BS_API(const char*) GetPublishedItemID() {
	ret = to_string(publishedFileId);
	return ret.c_str();
}

void CallbackHandler::handleItemCreated(CreateItemResult_t* callback, bool bIOFailure) {
	if (callback->m_eResult != k_EResultOK) {
		currentUpdateStatus = 1000 + callback->m_eResult;
		return;
	}

	publishedFileId = callback->m_nPublishedFileId;

	UpdateItemInternal();
}

void CallbackHandler::handleItemUpdateSubmitted(SubmitItemUpdateResult_t* callback, bool bIOFailure) {
	if (callback->m_eResult == k_EResultOK) {
		currentUpdateStatus = 3;
	} else {
		if (isCreating) {
			SteamUGC()->DeleteItem(publishedFileId);
		}
		currentUpdateStatus = 2000 + callback->m_eResult;
	}
}


static vector<PublishedFileId_t> subscribedItems;
BS_API(void) LoadSubscribedItems() {
	auto count = SteamUGC()->GetNumSubscribedItems();
	subscribedItems.resize(count);
	SteamUGC()->GetSubscribedItems(subscribedItems.data(), count);
}

BS_API(int) GetSubscribedItemCount() {
	return subscribedItems.size();
}

static string id;
BS_API(const char*) GetSubscribedItemID(int index) {
	id = to_string(subscribedItems[index]);
	return id.c_str();
}

static constexpr int PATH_BUF_SIZE = 300;
static char pathBuf[PATH_BUF_SIZE];
BS_API(const char*) GetSubscribedItemPath(int index) {
	if (SteamUGC()->GetItemInstallInfo(subscribedItems[index], nullptr, pathBuf, PATH_BUF_SIZE, nullptr)) {
		return pathBuf;
	} else {
		return "";
	}
}
