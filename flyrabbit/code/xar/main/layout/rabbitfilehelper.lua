--[[
	tRabbitFileList = {
		["data"] = {
			["tFileList"] = {
				[1] = tFileItem1,
				[2] = tFileItem2,
			}
		}
	}
	
	tRabbitFileItem = {
		["tDownLoadConfig"] =
		{
			["strFileName"] = "",
			["nFileSizeInByte"] = "",
			["nDownSizeInByte"] = "",
			["nFinishPercent"] = "",
			["strFileDir"] = "",
			["strFileURL"] = "",
			["bIsResume"] = "",
			["nFileState"] = FILESTATE_START | FILESTATE_PAUSE | FILESTATE_FINISH | FILESTATE_ERROR,
		}
		["hTaskHandle"] = -1,		
		["nDownSpeedInByte"] = 0,		
		["strICOPath"] = "",
	}
	
--]]

local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")
local miniTPUtil = XLGetObject("MiniTP.Util")

local g_tDeleteItemList = {}
local tRabbitFileList = {}
tRabbitFileList.data = {}
tRabbitFileList.data.tFileList = {}


local FILESTATE_NOITEM = 0
local FILESTATE_ERROR = 1
local FILESTATE_PAUSE = 2
local FILESTATE_START = 3
local FILESTATE_FINISH = 4
local FILESTATE_STARTPENDING = 5
local FILESTATE_PAUSEPENDING = 6


tRabbitFileList.FILESTATE_NOITEM = FILESTATE_NOITEM
tRabbitFileList.FILESTATE_START = FILESTATE_START
tRabbitFileList.FILESTATE_PAUSE = FILESTATE_PAUSE
tRabbitFileList.FILESTATE_FINISH = FILESTATE_FINISH
tRabbitFileList.FILESTATE_ERROR = FILESTATE_ERROR
tRabbitFileList.FILESTATE_STARTPENDING = FILESTATE_STARTPENDING
tRabbitFileList.FILESTATE_PAUSEPENDING = FILESTATE_PAUSEPENDING

function tRabbitFileList:GetFileList()
	return self.data.tFileList
end


function tRabbitFileList:GetFileItemByIndex(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileItemByIndex] param error")
		return nil
	end

	local tFileList = self:GetFileList()
	return tFileList[nIndex]
end


function tRabbitFileList:PushFileItem(tFileItem)
	if type(tFileItem) ~= "table" or type(tFileItem.tDownLoadConfig) ~= "table" then
		Log("[PushFileItem] param error")
		return false
	end
	
	local tDownLoadConfig = tFileItem.tDownLoadConfig
	
	if tDownLoadConfig.nFileState ~= nil then
		if tDownLoadConfig.nFileState == FILESTATE_START then
			-- tRabbitFileList:QueryTask(tFileItem)
			tRabbitFileList:StartTask(tFileItem)
		end
	end
	
	local tFileList = self:GetFileList()
	tFileList[#tFileList+1] = tFileItem
	
	Log("[PushFileItem] strFileName: "..tostring(tDownLoadConfig.strFileName))
	return true
end


function tRabbitFileList:PushDeleteItem(tFileItem)
	if type(tFileItem) ~= "table" then
		Log("[PushDeleteItem] param error")
		return false
	end
	
	if tFileItem.hTaskHandle ~= nil and tFileItem.hTaskHandle ~= -1 then
		local hTaskHandle = tFileItem.hTaskHandle
		miniTPUtil:TaskForcePause(hTaskHandle)
	end
		
	g_tDeleteItemList[#g_tDeleteItemList+1] = tFileItem
	tRabbitFileList:DealWithDeleteList()
	
	return true
end


function tRabbitFileList:GetDeleteItemList(tFileItem)
	return g_tDeleteItemList
end


function tRabbitFileList:RemoveFileItem(nFileItemIndex, bDeleteFile)
	if type(nFileItemIndex) ~= "number" then
		Log("[RemoveFileItem] param error")
		return false
	end

	local tFileList = self:GetFileList()
	if type(tFileList[nFileItemIndex]) ~= "table" then
		Log("[RemoveFileItem] no item :" .. tostring(nFileItemIndex))
		return false
	end
	
	local tDeleteItem = tFileList[nFileItemIndex]
	tDeleteItem.bDeleteFile = bDeleteFile
	tRabbitFileList:PushDeleteItem(tDeleteItem)
	
	table.remove(tFileList, nFileItemIndex)
	return true
end


function tRabbitFileList:SetFileName(nIndex, strFileName)
	if type(nIndex) ~= "number" or not IsRealString(strFileName) then
		Log("[SetFileName] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetFileName] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.tDownLoadConfig.strFileName = strFileName
	return true
end


function tRabbitFileList:GetFileName(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileName] param error")
		return ""
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetFileName] GetFileItemByIndex failed"..tostring(nIndex))
		return ""
	end
	
	return tFileItem.tDownLoadConfig.strFileName
end


function tRabbitFileList:SetFileItemState(nIndex, nFileState)
	if type(nIndex) ~= "number" or IsNilString(nFileState) then
		Log("[SetFileItemState] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetFileItemState] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	if nFileState == FILESTATE_START then
		tRabbitFileList:StartTask(tFileItem)
	elseif nFileState == FILESTATE_PAUSE then
		tRabbitFileList:PauseTask(tFileItem)
	elseif nFileState == FILESTATE_ERROR or nFileState == FILESTATE_FINISH then
		tRabbitFileList:DeleteTask(tFileItem)
	end	
	
	tFileItem.tDownLoadConfig.nFileState = nFileState
	return true
end


function tRabbitFileList:GetFileItemState(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileItemState] param error")
		return -1
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetFileItemState] GetFileItemByIndex failed"..tostring(nIndex))
		return -1
	end
	
	return tFileItem.tDownLoadConfig.nFileState
end


function tRabbitFileList:GetFileDir(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileDir] param error")
		return ""
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetFileDir] GetFileItemByIndex failed"..tostring(nIndex))
		return ""
	end
	
	return tFileItem.tDownLoadConfig.strFileDir
end


function tRabbitFileList:GetFileSizeInByte(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetFileSizeInByte] param error")
		return 0
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetFileSizeInByte] GetFileItemByIndex failed"..tostring(nIndex))
		return 0
	end
	
	return tFileItem.tDownLoadConfig.nFileSizeInByte
end


function tRabbitFileList:SetFileSizeInByte(nIndex, nFileSizeInByte)
	if type(nIndex) ~= "number" or type(nFileSizeInByte) ~= "number" then
		Log("[SetFileSizeInByte] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetFileSizeInByte] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.tDownLoadConfig.nFileSizeInByte = nFileSizeInByte
	return true
end


function tRabbitFileList:SetDownSpeedInByte(nIndex, nDownSpeedInByte)
	if type(nIndex) ~= "number" or type(nDownSpeedInByte) ~= "number" then
		Log("[SetDownSpeedInByte] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetDownSpeedInByte] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.nDownSpeedInByte = nDownSpeedInByte
	return true
end


function tRabbitFileList:GetDownSpeedInByte(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetDownSpeedInByte] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetDownSpeedInByte] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	return tFileItem.nDownSpeedInByte
end


function tRabbitFileList:GetDownSizeInByte(nIndex)
	if type(nIndex) ~= "number" then
		Log("[GetDownSizeInByte] param error")
		return 0
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[GetDownSizeInByte] GetFileItemByIndex failed"..tostring(nIndex))
		return 0
	end
	
	return tFileItem.tDownLoadConfig.nDownSizeInByte
end


function tRabbitFileList:SetDownSizeInByte(nIndex, nDownSizeInByte)
	if type(nIndex) ~= "number" or type(nDownSizeInByte) ~= "number" then
		Log("[SetDownSizeInByte] param error")
		return false
	end

	local tFileItem = self:GetFileItemByIndex(nIndex)
	if type(tFileItem) ~= "table" then
		Log("[SetDownSizeInByte] GetFileItemByIndex failed"..tostring(nIndex))
		return false
	end
	
	tFileItem.tDownLoadConfig.nDownSizeInByte = nDownSizeInByte
	return true
end


---文件读写
function tRabbitFileList:LoadListFromFile()
	local tFunHelper = XLGetGlobal("Project.FunctionHelper")
	if type(tFunHelper) ~= "table" then
		return false
	end

	local tFileListFromDisk = tFunHelper.ReadConfigFromMemByKey("tFileList") or {}
	if type(tFileListFromDisk.tConfigList) ~= "table" then
		return false
	end
	
	for nIndex, tConfigItem in ipairs(tFileListFromDisk.tConfigList) do
		local tFileItem = {}
		tFileItem.tDownLoadConfig = tConfigItem
		tFileItem.hTaskHandle = -1

		tRabbitFileList:PushFileItem(tFileItem)
	end

	Log("[LoadListFromFile] load end")
	return true
end


function tRabbitFileList:SaveListToFile()
	local tFunHelper = XLGetGlobal("Project.FunctionHelper")
	if type(tFunHelper) ~= "table" then
		return false
	end

	local tFileList = tRabbitFileList:GetFileList()
	local tFileListFromDisk = tFunHelper.ReadConfigFromMemByKey("tFileList") or {}
	tFileListFromDisk.tConfigList = {}
	for nIndex, tFileItem in ipairs(tFileList) do
		tFileListFromDisk.tConfigList[nIndex] = tFileItem.tDownLoadConfig
	end
	tFunHelper.SaveConfigToFileByKey("tFileList")
	return true
end


------minitp util
local g_bHasInit = false

function tRabbitFileList:Init()
	if g_bHasInit then
		return true
	end
	
	local bSucc = miniTPUtil:LoadXLDL()
	if not bSucc then
		Log("[Init] LoadXLDL failed")
		return false
	end
	
	local bSucc = miniTPUtil:Init()
	if not bSucc then
		Log("[Init] miniTPUtil Init failed")
		return false
	end
	
	g_bHasInit = true
	Log("[Init] success")
	return true
end


function tRabbitFileList:UnInit()
	if not g_bHasInit then
		Log("[UnInit] miniTPUtil has not init")
		return false
	end
	
	local bSucc = miniTPUtil:UnInit()
	if not bSucc then
		Log("[UnInit] miniTPUtil UnInit failed")
		return false
	end
	
	g_bHasInit = false
	Log("[UnInit] success")
	return true
end


function tRabbitFileList:CreateTask(tFileItem)
	local bPassCheck = CheckFileItemForDownload(tFileItem)
	if not bPassCheck then
		Log("[CreateTask] CheckFileItemForDownload failed")
		return false
	end
	
	local tDownLoadConfig = tFileItem.tDownLoadConfig
	local strRefURL = nil
	local strCookies = nil
	local bOnlyOriginal = nil
	local nDisableAutoRename = 1  --0是不重命名
	local strUrl = self:ParseThunderPrivateUrl(tDownLoadConfig.strFileURL)
	local hTaskHandle = miniTPUtil:TaskCreate(strUrl, tDownLoadConfig.strFileDir
						, tDownLoadConfig.strFileName, strRefURL, strCookies, strCookies
						, bOnlyOriginal, nDisableAutoRename, tDownLoadConfig.bIsResume)
					
	Log("[CreateTask] tFileItem.strFileURL:"..tostring(tDownLoadConfig.strFileURL)
			.."   hTaskHandle: "..tostring(hTaskHandle))
			
	return hTaskHandle
end


function tRabbitFileList:StartTask(tFileItem)
	if type(tFileItem) ~= "table" then
		Log("[StartTask] tFileItem not valid")
		return false
	end
	
	local hHandle = tRabbitFileList:CreateTask(tFileItem)
	tFileItem.hTaskHandle = hHandle

	local hTaskHandle = tFileItem.hTaskHandle
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[StartTask] hTaskHandle not valid")
		return false
	end
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	if tFileItem.hStartTaskTimer ~= nil then
		Log("[StartTask] already try starting")
		return false
	end
		
	local  bQueryRet, tTaskInfo = tRabbitFileList:QueryTask(tFileItem)
	Log("[StartTask] first QueryTask  strFileState: " ..tostring(tTaskInfo.stat).." bQueryRet: "..tostring(bQueryRet))		
	if bQueryRet and tTaskInfo.stat == FILESTATE_START then	
		Log("[StartTask] already in START state")
		return true
	end
		
	miniTPUtil:TaskStart(hTaskHandle)
	
	tFileItem.hStartTaskTimer = timerManager:SetTimer(function(item, id)
		local  bQueryRet, tTaskInfo = tRabbitFileList:QueryTask(tFileItem)
		Log("[StartTask] try STARTING QueryTask  strFileState: " ..tostring(tTaskInfo.stat).." bQueryRet: "..tostring(bQueryRet))
				
		if bQueryRet then
			if tTaskInfo.stat == FILESTATE_START then
				Log("[StartTask] strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).."  start task success")
				item:KillTimer(id)
				tFileItem.hStartTaskTimer = nil
				return
			end
		
			if tTaskInfo.stat ~= tRabbitFileList.FILESTATE_PAUSEPENDING	and tTaskInfo.stat ~= tRabbitFileList.FILESTATE_STARTPENDING then
				local bRet = miniTPUtil:TaskStart(hTaskHandle)
				Log("[StartTask] execute start,  strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bRet))
				item:KillTimer(id)
				tFileItem.hStartTaskTimer = nil
			end
		end
	end, 1*1000)	
		
	return true	
end


function tRabbitFileList:PauseTask(tFileItem)
	if type(tFileItem) ~= "table" then
		Log("[PauseTask] tFileItem not valid")
		return false
	end
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	local hTaskHandle = tFileItem.hTaskHandle
	
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[PauseTask] hTaskHandle not valid")
		return false
	end
		
	if tFileItem.hPauseTimer ~= nil then
		Log("[PauseTask] already try pausing")
		return false
	end
		
	local  bQueryRet, tTaskInfo = tRabbitFileList:QueryTask(tFileItem)
	Log("[PauseTask] first QueryTask  strFileState: " ..tostring(tTaskInfo.stat).." bQueryRet: "..tostring(bQueryRet))		
	if bQueryRet and tTaskInfo.stat == FILESTATE_PAUSE then	
		Log("[PauseTask] already in puase state")
		return true
	end
		
	miniTPUtil:TaskPause(hTaskHandle)
	
	tFileItem.hPauseTimer = timerManager:SetTimer(function(item, id)
		local  bQueryRet, tTaskInfo = tRabbitFileList:QueryTask(tFileItem)
		Log("[PauseTask] trypauseing QueryTask  strFileState: " ..tostring(tTaskInfo.stat).." bQueryRet: "..tostring(bQueryRet))
				
		if bQueryRet then
			if tTaskInfo.stat == FILESTATE_PAUSE then
				miniTPUtil:TaskDelete(hTaskHandle)
				item:KillTimer(id)
				tFileItem.hPauseTimer = nil
				Log("[PauseTask] delete timer and handle, in state pause")
				return
			end
		
			if tTaskInfo.stat ~= tRabbitFileList.FILESTATE_PAUSEPENDING	and tTaskInfo.stat ~= tRabbitFileList.FILESTATE_STARTPENDING then
				local bRet = miniTPUtil:TaskPause(hTaskHandle)
				Log("[PauseTask] execute pause,  strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bRet))
				
				if bRet then
					local bRet = miniTPUtil:TaskDelete(hTaskHandle)
					item:KillTimer(id)
					tFileItem.hPauseTimer = nil
					Log("[PauseTask] delete timer and handle, after execute TaskPause")
				end
			end
		end
	end, 1*1000)	
		
	return true
end


function tRabbitFileList:QueryTask(tFileItem)
	if type(tFileItem) ~= "table" then
		return false
	end

	local hTaskHandle = tFileItem.hTaskHandle
	
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[QueryTask] hTaskHandle not valid")
		return false
	end
	
	local bRet, tTaskInfo = miniTPUtil:TaskQueryEx(hTaskHandle)
	Log("[QueryTask] strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bRet))
	
	return bRet, tTaskInfo
end


local g_DeleteTimer = nil
function tRabbitFileList:DealWithDeleteList()
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")

	local tDeleteItemList = tRabbitFileList:GetDeleteItemList()
	if g_DeleteTimer ~= nil then
		timerManager:KillTimer(g_DeleteTimer)
		g_DeleteTimer = nil 
		
		if #tDeleteItemList < 1 then
			return
		end
	end
	
	g_DeleteTimer = timerManager:SetTimer(function(item, id)
		for nIndex, tFileItem in pairs(tDeleteItemList) do 
			local bHasDelete = tRabbitFileList:DeleteErrorHandleFile(tFileItem, tDeleteItemList, nIndex)
		
			if not bHasDelete then
				bHasDelete = tRabbitFileList:DeleteFinishStateFile(tFileItem, tDeleteItemList, nIndex)
				
				if not bHasDelete then
					tRabbitFileList:DeleteUnFinishStateFile(tFileItem, tDeleteItemList, nIndex)
				end
			end
		end
		
		if #tDeleteItemList < 1 then
			item:KillTimer(id)
			g_DeleteTimer = nil
		end
		
	end, 1*1000)
end


function tRabbitFileList:DeleteErrorHandleFile(tFileItem, tDeleteItemList, nIndex)
	local bHasDelete = true
	
	if type(tFileItem) ~= "table" or type(tFileItem.tDownLoadConfig) ~= "table" then
		Log("[DeleteErrorHandleFile] tFileItem param error")
		return not bHasDelete
	end
	
	local hTaskHandle = tFileItem.hTaskHandle
	if hTaskHandle ~= nil and hTaskHandle ~= -1 then
		return not bHasDelete
	end
	
	local bDeleteFile = tFileItem.bDeleteFile
	if bDeleteFile then
		local bRet = tRabbitFileList:DelTempFile(tFileItem)
		Log("[DeleteErrorHandleFile]  DelTempFile strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bRet))
	end
	
	Log("[DeleteErrorHandleFile] delete from list")
	table.remove(tDeleteItemList, nIndex)
	return bHasDelete
end


function tRabbitFileList:DeleteFinishStateFile(tFileItem, tDeleteItemList, nIndex)
	local bHasDelete = true
	
	if type(tFileItem) ~= "table" or type(tFileItem.tDownLoadConfig) ~= "table" then
		Log("[DeleteFinishStateFile] tFileItem param error")
		return not bHasDelete
	end

	if tFileItem.tDownLoadConfig.nFileState ~= FILESTATE_FINISH then
		return not bHasDelete
	end
	
	tRabbitFileList:DeleteTask(tFileItem)
	
	local bDeleteFile = tFileItem.bDeleteFile
	if bDeleteFile then
		local bRet = tRabbitFileList:DelTempFile(tFileItem)
		Log("[DeleteFinishStateFile]  DelTempFile strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bRet))
	end
	
	Log("[DeleteFinishStateFile] delete from list")
	table.remove(tDeleteItemList, nIndex)
	return bHasDelete	
end


function tRabbitFileList:DeleteUnFinishStateFile(tFileItem, tDeleteItemList, nIndex)
	local bHasDelete = true
	
	if type(tFileItem) ~= "table" or type(tFileItem.tDownLoadConfig) ~= "table" then
		Log("[DeleteUnFinishStateFile] tFileItem param error, remove fileitem from list")
		table.remove(tDeleteItemList, nIndex)
		return
	end
			
	local  bQueryRet, tTaskInfo = tRabbitFileList:QueryTask(tFileItem)
	Log("[DeleteUnFinishStateFile] strFileName: ".. tostring(tFileItem.tDownLoadConfig.strFileName) .. "  strFileState: " ..tostring(tTaskInfo.stat).." bQueryRet: "..tostring(bQueryRet))
	
	if bQueryRet and (tTaskInfo.stat == FILESTATE_PAUSE or tTaskInfo.stat == FILESTATE_ERROR) then
		local hTaskHandle = tFileItem.hTaskHandle
		local bDeleteFile = tFileItem.bDeleteFile

		local bDeleteRet = miniTPUtil:TaskDelete(hTaskHandle)
		Log("[DeleteUnFinishStateFile] TaskDelete strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bDeleteRet))
						
		if bDeleteFile then
			local bRet = tRabbitFileList:DelTempFile(tFileItem)
			Log("[DeleteUnFinishStateFile] DelTempFile strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bRet))
		end	
		
		if bDeleteRet then
			table.remove(tDeleteItemList, nIndex)
		end
	end
end


function tRabbitFileList:DeleteTask(tFileItem)
	if type(tFileItem) ~= "table" then
		return false
	end

	local hTaskHandle = tFileItem.hTaskHandle
	
	if hTaskHandle == nil or hTaskHandle == -1 then
		Log("[DeleteTask] hTaskHandle not valid")
		return false
	end
	
	local bRet = miniTPUtil:TaskDelete(hTaskHandle)
	Log("[DeleteTask] strFileName: " ..tostring(tFileItem.tDownLoadConfig.strFileName).." bRet: "..tostring(bRet))
							
	return bRet
end


function tRabbitFileList:DelTempFile(tFileItem)
	if type(tFileItem) ~= "table" or type(tFileItem.tDownLoadConfig) ~= "table" then
		Log("[DelTempFile] tFileItem param error")
		return false
	end

	local strFileDir = tFileItem.tDownLoadConfig.strFileDir or ""
	local strFileName = tFileItem.tDownLoadConfig.strFileName or ""
	local strFilePath = tipUtil:PathCombine(strFileDir, strFileName)
	
	local bRet = miniTPUtil:DelTempFile(strFileDir, strFileName)
	Log("[DelTempFile] strFileName: " ..tostring(strFileName).. "  strDir:" ..tostring(strFileDir) .. " bRet: "..tostring(bRet))
	
	DeletePathFile(strFilePath)
	
	local timerManager = XLGetObject("Xunlei.UIEngine.TimerManager")
	timerManager:SetOnceTimer(function(item, id)
		DeletePathFile(strFilePath)
	end, 5*1000)
	
	return bRet
end


function DeletePathFile(strFilePath)
	if not IsRealString(strFilePath) then
		return
	end	
	
	local bRet = tipUtil:DeletePathFile(strFilePath)
	Log("[DelTempFile] DeletePathFile: " ..tostring(strFilePath).." bRet: "..tostring(bRet))
	
	local strTempFile = strFilePath..".td"
	bRet = tipUtil:DeletePathFile(strTempFile)
	Log("[DelTempFile] DeletePathFile: " ..tostring(strTempFile).." bRet: "..tostring(bRet))
	
	strTempFile = strFilePath..".td.cfg"
	bRet = tipUtil:DeletePathFile(strTempFile)
	Log("[DelTempFile] DeletePathFile: " ..tostring(strTempFile).." bRet: "..tostring(bRet))
end


function tRabbitFileList:ParseThunderPrivateUrl(strThunderUrl)
	Log("[ParseThunderPrivateUrl] strThunderUrl = "..tostring(strThunderUrl))
	if not IsRealString(strThunderUrl) then
		return strUrl
	end
	if string.find(string.lower(strThunderUrl),"^thunder://") == nil then
		return strThunderUrl
	end
	local bRet,strUrl = miniTPUtil:ParseThunderPrivateUrl(strThunderUrl)
	Log("[ParseThunderPrivateUrl] Parse bRet = "..tostring(bRet) .. ", strUrl = " .. tostring(strUrl))
	if bRet then
		return strUrl
	else
		return strThunderUrl
	end
end


function tRabbitFileList:AsynGetFileSizeWithUrlInByte(strURL,fnCallBack)
	if not IsRealString(strURL) then
		return
	end
	tipAsynUtil:AsynGetFileSizeWithUrl(strURL,function(iRet, nFileSizeInByte)
		if iRet == 0 and nFileSizeInByte > 0 then
			nFileSizeInByte = nFileSizeInByte/1024
			fnCallBack(iRet,nFileSizeInByte)
		else
			fnCallBack(iRet,0)
		end
	end)
end

function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end


function CheckFileItemForDownload(tFileItem)
	if type(tFileItem) ~= "table" or type(tFileItem.tDownLoadConfig) ~= "table" then
		return false
	end

	local tDownLoadConfig = tFileItem.tDownLoadConfig

	if not IsRealString(tDownLoadConfig.strFileURL) 
		or not IsRealString(tDownLoadConfig.strFileDir) 
		or not IsRealString(tDownLoadConfig.strFileName) then
		return false
	end
	
	if type(tDownLoadConfig.bIsResume) ~= "boolean" then
		tDownLoadConfig.bIsResume = true
	end
	
	return true
end


XLSetGlobal("Project.RabbitFileList", tRabbitFileList)

------------------
function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function Log(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@RabbitFileHelper_Log: " .. tostring(strLog))
	end
end
