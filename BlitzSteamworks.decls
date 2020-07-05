.lib "BlitzSteamworks.dll"

Steam_Init%():"Init"
Steam_Update():"Update"
Steam_Shutdown():"Shutdown"

Steam_Achieve%(ID$):"Achieve"
Steam_UnAchieve%(ID$):"UnAchieve"

Steam_GetOverlayState%():"GetOverlayState"
Steam_GetOverlayUpdated%():"GetOverlayUpdated"

Steam_GetPlayerID$():"GetPlayerID"
Steam_GetPlayerName$():"GetPlayerName"

Steam_PushByte(b%):"PushByte"
Steam_PushShort(s%):"PushShort"
Steam_PushInt%(i%):"PushInt"
Steam_PushFloat#(f#):"PushFloat"
Steam_PushString$(s$):"PushString"

Steam_PullByte%():"PullByte"
Steam_PullShort%():"PullShort"
Steam_PullInt%():"PullInt"
Steam_PullFloat#():"PullFloat"
Steam_PullString$():"PullString"

Steam_LoadPacket%():"LoadPacket"
Steam_SendPacketToUser%(sid$):"SendPacketToUser"
Steam_CloseConnection%(sid$):"CloseConnection"

EE$():"EE"