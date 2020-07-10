.lib "BlitzSteamworks.dll"

Steam_Init%():"Init"
Steam_Update():"Update"
Steam_Shutdown():"Shutdown"

Steam_Achieve%(ID$):"Achieve"
Steam_UnAchieve%(ID$):"UnAchieve"

Steam_GetOverlayState%():"GetOverlayState"
Steam_GetOverlayUpdated%():"GetOverlayUpdated"

Steam_StringToIDUpper%(cid$):"StringToIDUpper"
Steam_StringToIDLower%(cid$):"StringToIDLower"

Steam_IDToString$(upperID%, lowerID%):"IDToString"

Steam_GetPlayerIDUpper%():"GetPlayerIDUpper"
Steam_GetPlayerIDLower%():"GetPlayerIDLower"
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

Steam_GetSenderIDUpper%():"GetSenderIDUpper"
Steam_GetSenderIDLower%():"GetSenderIDLower"

Steam_LoadPacket%():"LoadPacket"
Steam_SendPacketToUser%(upperID%, lowerID%):"SendPacketToUser"
Steam_CloseConnection%(upperID%, lowerID%):"CloseConnection"

Steam_EE$(cid$):"EE"