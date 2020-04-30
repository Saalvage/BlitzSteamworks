.lib "BlitzSteamworks.dll"

Steam_Init%():"Init"
Steam_Update():"Update"

Steam_Achieve%(ID$):"Achieve"
Steam_UnAchieve%(ID$):"UnAchieve"

Steam_GetOverlayState%():"GetOverlayState"
Steam_GetOverlayUpdated%():"GetOverlayUpdated"

Steam_GetPlayerID%():"GetPlayerID"
Steam_GetPlayerName$():"GetPlayerName"

Steam_DownloadItems():"DownloadSubbedItems"
Steam_CheckDownload():"IsStillDownloading"