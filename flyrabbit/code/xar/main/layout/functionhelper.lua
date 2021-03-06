local tipUtil = XLGetObject("API.Util")
local tipAsynUtil = XLGetObject("API.AsynUtil")

local gStatCount = 0
local gForceExit = nil

function IsRealString(str)
	return type(str) == "string" and str ~= ""
end

function IsNilString(AString)
	if AString == nil or AString == "" then
		return true
	end
	return false
end

function TipLog(strLog)
	if type(tipUtil.Log) == "function" then
		tipUtil:Log("@@Project_Log: " .. tostring(strLog))
	end
end


function LoadTableFromFile(strDatFilePath)
	local tResult = nil

	if IsRealString(strDatFilePath) and tipUtil:QueryFileExists(strDatFilePath) then
		local tMod = XLLoadModule(strDatFilePath)
		if type(tMod) == "table" and type(tMod.GetSubTable) == "function" then
			local tDat = tMod.GetSubTable()
			if type(tDat) == "table" then
				tResult = tDat
			end
		end
	end
	
	return tResult
end

function FetchValueByPath(obj, path)
	local cursor = obj
	for i = 1, #path do
		cursor = cursor[path[i]]
		if cursor == nil then
			return nil
		end
	end
	return cursor
end

function GetCommandStrValue(strKey)
	local bRet, strValue = false, nil
	local cmdString = tipUtil:GetCommandLine()
	
	if string.find(cmdString, strKey .. " ") then
		local cmdList = tipUtil:CommandLineToList(cmdString)
		if cmdList ~= nil then	
			for i = 1, #cmdList, 1 do
				local strTmp = tostring(cmdList[i])
				if strTmp == strKey 
					and not string.find(tostring(cmdList[i + 1]), "^/") then		
					bRet = true
					strValue = tostring(cmdList[i + 1])
					break
				end
			end
		end
	end
	return bRet, strValue
end


function ExitProcess()
	SaveAllConfig()

	TipLog("************ Exit ************")
	tipUtil:Exit("Exit")
end

function HideMainWindow()
	local objMainWnd = GetMainWndInst()
	if objMainWnd then
		objMainWnd:Show(0)
	end
end

function ReportAndExit()
	HideTray()
	HideMainWindow()	
	DestroyPopupWnd()	
	SendRunTimeReport(0, true)
	SendLocalReport(10)
	local tRabbitFileList = XLGetGlobal("Project.RabbitFileList")
	tRabbitFileList:UnInit()
	tRabbitFileList:SaveListToFile()
	
	local tStatInfo = {}
	tStatInfo.strEC = "exit"	
	tStatInfo.strEA = GetInstallSrc() or ""
	tStatInfo.Exit = true
			
	TipConvStatistic(tStatInfo)
end


function SendRunTimeReport(nTimeSpanInSec, bExit)
	local tStatInfo = {}
	tStatInfo.strEC = "runtime"
	tStatInfo.strEA = GetInstallSrc() or ""
	
	local nRunTime = 0
	local nLastReportRunTmUTC = XLGetGlobal("Project.LastReportRunTime") 
	if bExit and nLastReportRunTmUTC ~= 0 then
		nRunTime = math.abs(tipUtil:GetCurrentUTCTime() - nLastReportRunTmUTC)
	else
		nRunTime = nTimeSpanInSec
	end
	tStatInfo.strEV = nRunTime
	
	TipConvStatistic(tStatInfo)
end


function IsUserFullScreen()
	local bRet = false
	if type(tipUtil.IsNowFullScreen) == "function" then
		bRet = tipUtil:IsNowFullScreen()
	end
	return bRet
end



function UrlEncode(strUrlToBeEncoded)
	local strUrlEncoded = nil
	if type(strUrlToBeEncoded) == "string" then
		strUrlEncoded = ""
		local nPos = 1
		while nPos <= #strUrlToBeEncoded do
			local nCharCode = string.byte(strUrlToBeEncoded, nPos)
			if (nCharCode == 46) or (nCharCode > 47 and nCharCode < 58) or (nCharCode > 64 and nCharCode < 91) or (nCharCode == 95) or (nCharCode > 96 and nCharCode < 123) then
				strUrlEncoded = strUrlEncoded .. string.char(nCharCode)
			else
				strUrlEncoded = strUrlEncoded .. string.format("%%%02X", nCharCode)
			end
			nPos = nPos + 1
		end
	end
		
	return strUrlEncoded
end


function CheckIsNewVersion(strNewVer, strCurVer)
	if not IsRealString(strNewVer) or not IsRealString(strCurVer) then
		return false
	end

	local a,b,c,d = string.match(strNewVer, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	local A,B,C,D = string.match(strCurVer, "(%d+)%.(%d+)%.(%d+)%.(%d+)")
	
	a = tonumber(a)
	b = tonumber(b)
	c = tonumber(c)
	d = tonumber(d)
	
	A = tonumber(A)
	B = tonumber(B)
	C = tonumber(C)
	D = tonumber(D)
	
	return a>A or (a==A and (b>B or (b==B and (c>C or (c==C and d>D)))))
end


function GetPeerID()
	local strPeerID = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\FlyRabbit\\PeerId")
	if IsRealString(strPeerID) then
		return strPeerID
	end

	local strRandPeerID = tipUtil:GetPeerId()
	if not IsRealString(strRandPeerID) then
		return ""
	end
	
	RegSetValue("HKEY_LOCAL_MACHINE\\Software\\FlyRabbit\\PeerId", strRandPeerID)
	return strRandPeerID
end

--渠道
function GetInstallSrc()
	local strInstallSrc = RegQueryValue("HKEY_LOCAL_MACHINE\\Software\\FlyRabbit\\InstallSource")
	if not IsNilString(strInstallSrc) then
		return tostring(strInstallSrc)
	end
	
	return ""
end


function FailExitTipWnd(self, iExitCode)
	local tStatInfo = {}
	tStatInfo.Exit = true
	TipConvStatistic(tStatInfo)
end


function CheckTimeIsAnotherDay(LastTime)
	local bRet = false
	local LYear, LMonth, LDay, LHour, LMinute, LSecond = tipUtil:FormatCrtTime(LastTime)
	local curTime = tipUtil:GetCurrentUTCTime()
	local CYear, CMonth, CDay, CHour, CMinute, CSecond = tipUtil:FormatCrtTime(curTime)
	if LYear ~= CYear or LMonth ~= CMonth or LDay ~= CDay then
		bRet = true
	end
	return bRet
end


function GetTimeStamp(nHour)
	local nRealHour = nHour or 24
	local strPeerId = GetPeerID()
	local iFlag = tonumber(string.sub(strPeerId, 12, 12), 16) or 0
	local iTime = tipUtil:GetCurrentUTCTime()
	local ss = math.floor((iTime + 8 * 3600  - (iFlag + 1) * 3600)/(nRealHour*3600))
	local strStamp = "?stamp=" .. tostring(ss)
	return strStamp 
end


function OpenFolderDialog()
	local bOpenFileDialog = true 
	local strExtName  = nil
	local strDefName = nil
	local strFilter = "HTML Files (*.htm, *.html, *.mht)|*.htm, *.html, *.mht|Text Files (*.txt)|*.txt|GIF Files (*.gif)|*.gif|JPGE Files (*.jpg, *.jpge)|*.jpg, *.jpge|All Files (*.*)|*.*||"
	return tipUtil:FolderDialog(bOpenFileDialog, strFilter, strExtName, strDefName)
end


function TipConvStatistic(tStat)
	local rdRandom = tipUtil:GetCurrentUTCTime()
	local tStatInfo = tStat or {}
	local strDefaultNil = "null"
	
	local strCID = GetPeerID()
	local strEC = tStatInfo.strEC 
	local strEA = tStatInfo.strEA 
	local strEL = tStatInfo.strEL
	local strEV = tStatInfo.strEV
	local strTID = tStatInfo.strTID or "UA-62827200-1"
	
	if IsNilString(strEC) then
		strEC = strDefaultNil
	end
	
	if IsNilString(strEA) then
		strEA = strDefaultNil
	end
	
	if IsNilString(strEL) then
		strEL = strDefaultNil
	end
	
	if tonumber(strEV) == nil then
		strEV = 1
	end
	
	local strUrl = "http://www.google-analytics.com/collect?v=1&tid="..strTID.."&cid="..tostring(strCID)
						.."&t=event&ec="..tostring(strEC).."&ea="..tostring(strEA)
						.."&el="..tostring(strEL).."&ev="..tostring(strEV)
	
	TipLog("TipConvStatistic: " .. tostring(strUrl))
	
	gStatCount = gStatCount + 1
	if not gForceExit and tStat.Exit then
		gForceExit = true
	end
	tipAsynUtil:AsynSendHttpStat(strUrl, function()
		gStatCount = gStatCount - 1
		if gStatCount == 0 and gForceExit then
			ExitProcess()
		end
	end)
	
	local iStatCount = gStatCount
	if gForceExit and iStatCount > 0 and gTimeoutTimerId == nil then	--开启定时退出定时器
		local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
		gTimeoutTimerId = timeMgr:SetTimer(function(Itm, id)
			Itm:KillTimer(id)
			ExitProcess()
		end, 15000 * iStatCount)
	end
end


function SendLocalReport(nOPeration)
	local strCID = GetPeerID()
	local strChannelID = GetInstallSrc()
	local strVer = GetMinorVer()
	local strRandom = tipUtil:GetCurrentUTCTime()
	
	local strPort = "8082"
	if nOPeration == 10 then   --心跳上报的端口为8083
		strPort = "8083"
	end
	
	local strUrl = "http://stat.feitwo.com:"..tostring(strPort).."/c?appid=1001&peerid=".. tostring(strCID)
					.."&proid=14&op="..tostring(nOPeration).."&cid="..(strChannelID)
					.."&ver="..tostring(strVer).."&rd="..tostring(strRandom)
	
	TipLog("SendLocalReport: " .. tostring(strUrl))
	tipAsynUtil:AsynSendHttpStat(strUrl, function() end)
end


function NewAsynGetHttpFile(strUrl, strSavePath, bDelete, funCallback, nTimeoutInMS)
	local bHasAlreadyCallback = false
	local timerID = nil
	
	tipAsynUtil:AsynGetHttpFile(strUrl, strSavePath, bDelete, 
		function (nRet, strTargetFilePath, strHeaders)
			if timerID ~= nil then
				tipAsynUtil:KillTimer(timerID)
			end
			if not bHasAlreadyCallback then
				bHasAlreadyCallback = true
				funCallback(nRet, strTargetFilePath, strHeaders)
			end
		end)
	
	timerID = tipAsynUtil:SetTimer(nTimeoutInMS or 2 * 60 * 1000,
		function (nTimerId)
			tipAsynUtil:KillTimer(nTimerId)
			timerID = nil
			if not bHasAlreadyCallback then
				bHasAlreadyCallback = true
				funCallback(-2)
			end
		end)
end


function CheckMD5(strFilePath, strExpectedMD5) 
	local bPassCheck = false
	
	if not IsNilString(strFilePath) then
		local strMD5 = tipUtil:GetMD5Value(strFilePath)
		TipLog("[CheckMD5] strFilePath = " .. tostring(strFilePath) .. ", strMD5 = " .. tostring(strMD5))
		if not IsRealString(strExpectedMD5) 
			or (not IsNilString(strMD5) and not IsNilString(strExpectedMD5) and string.lower(strMD5) == string.lower(strExpectedMD5))
			then
			bPassCheck = true
		end
	end
	
	TipLog("[CheckMD5] strFilePath = " .. tostring(strFilePath) .. ", strExpectedMD5 = " .. tostring(strExpectedMD5) .. ". bPassCheck = " .. tostring(bPassCheck))
	return bPassCheck
end


function DownLoadFileWithCheck(strURL, strSavePath, strCheckMD5, fnCallBack)
	if type(fnCallBack) ~= "function"  then
		return
	end

	if IsRealString(strCheckMD5) and CheckMD5(strSavePath, strCheckMD5) then
		TipLog("[DownLoadFileWithCheck]File Already existed")
		fnCallBack(1, strSavePath)
		return
	end
	
	NewAsynGetHttpFile(strURL, strSavePath, false, function(bRet, strDownLoadPath)
		TipLog("[DownLoadFileWithCheck] NewAsynGetHttpFile:bret = " .. tostring(bRet) 
				.. ", strURL = " .. tostring(strURL) .. ", strDownLoadPath = " .. tostring(strDownLoadPath))
		if 0 == bRet then
			strSavePath = strDownLoadPath
            if CheckMD5(strSavePath, strCheckMD5) then
				fnCallBack(bRet, strSavePath)
			else
				TipLog("[DownLoadFileWithCheck]Did Not Pass MD5 Check")
				fnCallBack(-2)
			end	
		else
			TipLog("[DownLoadFileWithCheck] DownLoad failed")
			fnCallBack(-3)
		end
	end)
end


function GetProgramTempDir(strSubDir)
	local strSysTempDir = tipUtil:GetSystemTempPath()
	local strProgramTempDir = tipUtil:PathCombine(strSysTempDir, strSubDir)
	if not tipUtil:QueryFileExists(strProgramTempDir) then
		tipUtil:CreateDir(strProgramTempDir)
	end
	
	return strProgramTempDir
end


function GetFileSaveNameFromUrl(url)
	local _, _, strFileName = string.find(tostring(url), ".*/(.*)$")
	local npos = string.find(strFileName, "?", 1, true)
	if npos ~= nil then
		strFileName = string.sub(strFileName, 1, npos-1)
	end
	return strFileName
end


function GetFileDirFromPath(strFilePath)
	if not IsRealString(strFilePath) then
		return ""
	end
	
	local npos1, npos2, strDir = string.find(strFilePath, "(.*)\\[^\\]*$")
	return strDir
end


function MessageBox(str)
	if not IsRealString(str) then
		return
	end
	
	tipUtil:MsgBox(str, "错误", 0x10)
end


function QueryAllUsersDir()	
	local bRet = false
	local strPublicEnv = "%PUBLIC%"
	local strRet = tipUtil:ExpandEnvironmentStrings(strPublicEnv)
	if strRet == nil or strRet == "" or strRet == strPublicEnv then
		local nCSIDL_COMMON_APPDATA = 35 --CSIDL_COMMON_APPDATA(0x0023)
		strRet = tipUtil:GetSpecialFolderPathEx(nCSIDL_COMMON_APPDATA)
	end
	if not IsNilString(strRet) and tipUtil:QueryFileExists(strRet) then
		bRet = true
	end
	return bRet, strRet
end


function GetExePath()
	return tipUtil:GetModuleExeName()
end


function GetProgramDir()
	local strExePath = GetExePath()
	local _, _, strProgramDir = string.find(strExePath, "(.*)\\.*$")
	return strProgramDir
end


function GetProjectVersion()
	local strEXEPath = GetExePath()
	if not IsRealString(strEXEPath) or not tipUtil:QueryFileExists(strEXEPath) then
		return ""
	end

	return tipUtil:GetFileVersionString(strEXEPath)
end


function GetMinorVer()
	local strVersion = GetProjectVersion()
	if not IsRealString(strVersion) then
		return ""
	end
	
	local _, _, strMinorVer = string.find(strVersion, "%d+%.%d+%.%d+%.(%d+)")
	return strMinorVer
end


function RegQueryValue(sPath)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:QueryRegValue(sRegRoot, sRegPath, sRegKey or "") or ""
		end
	end
	return ""
end


function RegDeleteValue(sPath)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath = string.match(sPath, "^(.-)[\\/](.*)")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:DeleteRegValue(sRegRoot, sRegPath)
		end
	end
	return false
end


function RegSetValue(sPath, value)
	if IsRealString(sPath) then
		local sRegRoot, sRegPath, sRegKey = string.match(sPath, "^(.-)[\\/](.*)[\\/](.-)$")
		if IsRealString(sRegRoot) and IsRealString(sRegPath) then
			return tipUtil:SetRegValue(sRegRoot, sRegPath, sRegKey or "", value or "")
		end
	end
	return false
end

----UI相关---

function GetMainWndInst()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local objMainWnd = hostwndManager:GetHostWnd("MainTipWnd.MainFrame")
	return objMainWnd
end


function GetMainCtrlChildObj(strObjName)
	local objMainWnd = GetMainWndInst()
	if not objMainWnd then
		return nil
	end
	
	local objTree = objMainWnd:GetBindUIObjectTree()
	
	if not objMainWnd or not objTree then
		TipLog("[GetMainCtrlChildObj] get main wnd or tree failed")
		return nil
	end
	
	local objRootCtrl = objTree:GetUIObject("root.layout:root.ctrl")
	if not objRootCtrl then
		TipLog("[GetMainCtrlChildObj] get objRootCtrl failed")
		return nil
	end 

	return objRootCtrl:GetControlObject(tostring(strObjName))
end


function ShowPopupWndByName(strWndName, bSetTop)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd(tostring(strWndName))
	if frameHostWnd == nil then
		TipLog("[ShowPopupWindow] GetHostWnd failed: "..tostring(strWndName))
		return
	end

	if not IsUserFullScreen() then
		if type(tipUtil.SetWndPos) == "function" then
			local hWnd = frameHostWnd:GetWndHandle()
			if hWnd ~= nil then
				TipLog("[ShowPopupWndByName] success")
				if bSetTop then
					frameHostWnd:SetTopMost(true)
					tipUtil:SetWndPos(hWnd, 0, 0, 0, 0, 0, 0x0043)
				else
					tipUtil:SetWndPos(hWnd, -2, 0, 0, 0, 0, 0x0043)
				end
			end
		end
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			frameHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
	
	frameHostWnd:Show(5)
end


function SetPopupWndCenterByName(strWndName)
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local frameHostWnd = hostwndManager:GetHostWnd(tostring(strWndName))
	if frameHostWnd == nil then
		TipLog("[SetPopupWndCenterByName] GetHostWnd failed: "..tostring(strWndName))
		return
	end

	local objtree = frameHostWnd:GetBindUIObjectTree()
	local objRootLayout = objtree:GetUIObject("root.layout")
	if not objRootLayout then
		return
	end

	if type(objRootLayout.MoveWindowToCenter) == "function" then
		objRootLayout:MoveWindowToCenter()
	end
end


function ShowModalDialog(wndClass, wndID, treeClass, treeID, userData, xarName)
 	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
    local modalHostWndTemplate = templateMananger:GetTemplate(wndClass,"HostWndTemplate")
    if modalHostWndTemplate == nil then
        return false
    end
    local modalHostWnd = modalHostWndTemplate:CreateInstance(wndID)
    if modalHostWnd == nil then
        return false
    end

    local objectTreeTemplate = templateMananger:GetTemplate(treeClass,"ObjectTreeTemplate")
    if objectTreeTemplate == nil then
        return false
    end
    local uiObjectTree = objectTreeTemplate:CreateInstance(treeID, xarName)
    if uiObjectTree == nil then
        return false
    end

	modalHostWnd:SetUserData(userData)
    modalHostWnd:BindUIObjectTree(uiObjectTree)
	local objMainWnd = GetMainWndInst()
	
	AsynCall(function ()
		local nRes = modalHostWnd:DoModal(objMainWnd)
		hostwndManager:RemoveHostWnd(modalHostWnd:GetID())
		local objtreeManager = XLGetObject("Xunlei.UIEngine.TreeManager")
		objtreeManager:DestroyTree(uiObjectTree)
	end)
	
	-- return nRes
end


--弹出窗口--
local g_tPopupWndList = {
	-- [1] = {"TipDeleteTaskWnd", "DeleteTaskTree"},
	-- [2] = {"TipSavePictureWnd", "SavePictureTree"},
	-- [3] = {"TipAboutWnd", "AboutTree"},
	-- [4] = {"TipDownloadFileWnd", "DownloadFileTree"},
	-- [5] = {"TipDeleteAllTaskWnd", "DeleteAllTaskTree"},
	-- [6] = {"TipNewTaskWnd", "NewTaskTree"},
	-- [7] = {"TipUpdateWnd", "TipUpdateTree"},
}

function CreatePopupTipWnd()
	for key, tItem in pairs(g_tPopupWndList) do
		local strHostWndName = tItem[1]
		local strTreeName = tItem[2]
		local bSucc = CreateWndByName(strHostWndName, strTreeName)
	end
	
	InitToolTip()
	return true
end

function CreateWndByName(strHostWndName, strTreeName)
	local bSuccess = false
	local strInstWndName = strHostWndName..".Instance"
	local strInstTreeName = strTreeName..".Instance"
	
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local frameHostWndTemplate = templateMananger:GetTemplate(strHostWndName, "HostWndTemplate" )
	if frameHostWndTemplate then
		local frameHostWnd = frameHostWndTemplate:CreateInstance(strInstWndName)
		if frameHostWnd then
			local objectTreeTemplate = nil
			objectTreeTemplate = templateMananger:GetTemplate(strTreeName, "ObjectTreeTemplate")
			if objectTreeTemplate then
				local uiObjectTree = objectTreeTemplate:CreateInstance(strInstTreeName)
				if uiObjectTree then
					frameHostWnd:BindUIObjectTree(uiObjectTree)
					local iRet = frameHostWnd:Create()
					if iRet ~= nil and iRet ~= 0 then
						bSuccess = true
					end
				end
			end
		end
	end

	return bSuccess
end

function DestroyPopupWnd()
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")

	for key, tItem in pairs(g_tPopupWndList) do
		local strPopupWndName = tItem[1]
		local strPopupInst = strPopupWndName..".Instance"
		
		local objPopupWnd = hostwndManager:GetHostWnd(strPopupInst)
		if objPopupWnd then
			hostwndManager:RemoveHostWnd(strPopupInst)
		end
	end
end

------------UI--


-------文件操作---
local g_bLoadCfgSucc = false
local g_tConfigFileStruct = {
	["tUserConfig"] = {
		["strFileName"] = "UserConfig.dat",
		["tContent"] = {}, 
	},
	["tFileList"] = {
		["strFileName"] = "FileList.dat",
		["tContent"] = {}, 
	},
}


function ReadAllConfigInfo()
	for strKey, tConfig in pairs(g_tConfigFileStruct) do
		local strFileName = tConfig["strFileName"]
		local strCfgPath = GetCfgPathWithName(strFileName)
		local infoTable = LoadTableFromFile(strCfgPath) or {}
		
		if type(infoTable) ~= "table" then
			TipLog("[ReadAllConfigInfo] no content in file: "..tostring(strFileName))
		end
		
		local tContent = infoTable
		local bMerge = false
		local fnMergeOldFile = tConfig["fnMergeOldFile"]
		if type(fnMergeOldFile) == "function" then
			bMerge, tContent = fnMergeOldFile(infoTable, strFileName)
		end
		
		tConfig["tContent"] = tContent
		if bMerge then
			SaveConfigToFileByKey(strKey)
		end
	end
	
	g_bLoadCfgSucc = true
	TipLog("[ReadAllConfigInfo] success!")
	return true
end


function GetCfgPathWithName(strCfgName)
	local bOk, strBaseDir = QueryAllUsersDir()
	if not bOk then
		return ""
	end
	local strCfgDir = tipUtil:PathCombine(strBaseDir, "FlyRabbit")
	if not tipUtil:QueryFileExists(strCfgDir) then
		tipUtil:CreateDir(strCfgDir)
	end
	local strCfgFilePath = tipUtil:PathCombine(strCfgDir, tostring(strCfgName))
	return strCfgFilePath or ""
end


function ReadConfigFromMemByKey(strKey)
	if not IsRealString(strKey) or type(g_tConfigFileStruct[strKey])~="table" then
		return nil
	end

	local tContent = g_tConfigFileStruct[strKey]["tContent"]
	return tContent
end


function SaveConfigToFileByKey(strKey)
	if not IsRealString(strKey) or type(g_tConfigFileStruct[strKey])~="table" then
		return
	end

	local strFileName = g_tConfigFileStruct[strKey]["strFileName"]
	local tContent = g_tConfigFileStruct[strKey]["tContent"] or {}
	local strConfigPath = GetCfgPathWithName(strFileName)
	if IsRealString(strConfigPath) then
		tipUtil:SaveLuaTableToLuaFile(tContent, strConfigPath)
	end
end


function SaveAllConfig()
	if g_bLoadCfgSucc then
		for strKey, tContent in pairs(g_tConfigFileStruct) do
			if strKey ~= "tFileList" then
				SaveConfigToFileByKey(strKey)
			end
		end
	end
end


---升级--
local g_bIsUpdating = false

function DownLoadNewVersion(tNewVersionInfo, fnCallBack)
	local strPacketURL = tNewVersionInfo.strPacketURL
	local strMD5 = tNewVersionInfo.strMD5
	if not IsRealString(strPacketURL) then
		return
	end
	
	local strFileName = GetFileSaveNameFromUrl(strPacketURL)
	if not string.find(strFileName, "%.exe$") then
		strFileName = strFileName..".exe"
	end
	local strSaveDir = tipUtil:GetSystemTempPath()
	local strSavePath = tipUtil:PathCombine(strSaveDir, strFileName)

	local strStamp = GetTimeStamp()
	local strURLFix = strPacketURL..strStamp
	
	DownLoadFileWithCheck(strURLFix, strSavePath, strMD5
	, function(bRet, strRealPath)
		TipLog("[DownLoadNewVersion] strOpenLink:"..tostring(strURLFix)
		        .."  bRet:"..tostring(bRet).."  strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(strRealPath, tNewVersionInfo)
			return
		end
		
		if 1 == bRet then	--安装包已经存在
			fnCallBack(strSavePath, tNewVersionInfo)
			return
		end
		
		fnCallBack(nil)
	end)	
end


function CheckCommonUpdateTime(nTimeInDay)
	return CheckUpdateTimeSpan(nTimeInDay, "nLastCommonUpdateUTC")
end

function CheckAutoUpdateTime(nTimeInDay)
	return CheckUpdateTimeSpan(nTimeInDay, "nLastAutoUpdateUTC")
end

function CheckUpdateTimeSpan(nTimeInDay, strUpdateType)
	if type(nTimeInDay) ~= "number" then
		return false
	end
	
	local nTimeInSec = nTimeInDay*24*3600
	local nCurTimeUTC = tipUtil:GetCurrentUTCTime()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local nLastUpdateUTC = tUserConfig[strUpdateType] or 0
	local nTimeSpan = math.abs(nCurTimeUTC - nLastUpdateUTC)
	
	if nTimeSpan > nTimeInSec then
		return true
	end	
	
	return false
end


function SaveCommonUpdateUTC()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLastCommonUpdateUTC"] = tipUtil:GetCurrentUTCTime()
	SaveConfigToFileByKey("tUserConfig")
end


function SaveAutoUpdateUTC()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["nLastAutoUpdateUTC"] = tipUtil:GetCurrentUTCTime()
	SaveConfigToFileByKey("tUserConfig")
end


function CheckIsUpdating()
	return g_bIsUpdating
end

function SetIsUpdating(bIsUpdating)
	if type(bIsUpdating) == "boolean" then
		g_bIsUpdating = bIsUpdating
	end
end


function DownLoadServerConfig(fnCallBack, nTimeInMs)
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local strConfigURL = tUserConfig["strServerConfigURL"]
	
	if not IsRealString(strConfigURL) then
		strConfigURL = "http://dl.feitwo.com/update/1.0/RabbitServerConfig.dat"
	end
	
	local strConfigName = GetFileSaveNameFromUrl(strConfigURL)
	local strSavePath = GetCfgPathWithName(strConfigName)
	if not IsRealString(strSavePath) then
		fnCallBack(-1)
		return
	end
	
	local strStamp = GetTimeStamp(1)
	local strURLFix = strConfigURL..strStamp	
	local nTime = tonumber(nTimeInMs) or 60*1000
		
	NewAsynGetHttpFile(strURLFix, strSavePath, false
	, function(bRet, strRealPath)
		TipLog("[DownLoadServerConfig] bRet:"..tostring(bRet)
				.." strRealPath:"..tostring(strRealPath))
				
		if 0 == bRet then
			fnCallBack(0, strSavePath)
		else
			fnCallBack(bRet)
		end		
	end, nTime)
end
--

------------------文件--


----菜单--
function TryDestroyOldMenu(objUIElem, strMenuKey)
	local uHostWndMgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local uObjTreeMgr = XLGetObject("Xunlei.UIEngine.TreeManager")
	local strHostWndName = strMenuKey..".HostWnd.Instance" 
	local strObjTreeName = strMenuKey..".Tree.Instance"

	if uHostWndMgr:GetHostWnd(strHostWndName) then
		uHostWndMgr:RemoveHostWnd(strHostWndName)
	end
	
	if uObjTreeMgr:GetUIObjectTree(strObjTreeName) then
		uObjTreeMgr:DestroyTree(strObjTreeName)
	end
end


--nTopSpan: 离弹出控件的高度差
--bRBtnPopup：右键弹出菜单
function CreateAndShowMenu(objUIElem, strMenuKey, nTopSpan, bRBtnPopup)
	local uTempltMgr = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local uHostWndMgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local uObjTreeMgr = XLGetObject("Xunlei.UIEngine.TreeManager")

	if uTempltMgr and uHostWndMgr and uObjTreeMgr then
		local uHostWnd = nil
		local strHostWndName = strMenuKey..".HostWnd.Instance"
		local strHostWndTempltName = "MenuHostWnd"
		local strHostWndTempltClass = "HostWndTemplate"
		local uHostWndTemplt = uTempltMgr:GetTemplate(strHostWndTempltName, strHostWndTempltClass)
		if uHostWndTemplt then
			uHostWnd = uHostWndTemplt:CreateInstance(strHostWndName)
		end

		local uObjTree = nil
		local strObjTreeTempltName = strMenuKey.."Tree"
		local strObjTreeTempltClass = "ObjectTreeTemplate"
		local strObjTreeName = strMenuKey..".Tree.Instance"
		local uObjTreeTemplt = uTempltMgr:GetTemplate(strObjTreeTempltName, strObjTreeTempltClass)
		if uObjTreeTemplt then
			uObjTree = uObjTreeTemplt:CreateInstance(strObjTreeName)
		end

		if uHostWnd and uObjTree then
			--函数会阻塞
			local bSucc = ShowMenuHostWnd(objUIElem, uHostWnd, uObjTree, nTopSpan, bRBtnPopup)
			
			if bSucc and uHostWnd:GetMenuMode() == "manual" then
				uObjTreeMgr:DestroyTree(strObjTreeName)
				uHostWndMgr:RemoveHostWnd(strHostWndName)
			end
		end
	end
end


function ShowMenuHostWnd(objUIElem, uHostWnd, uObjTree, nTopSpan, bRBtnPopup)
	uHostWnd:BindUIObjectTree(uObjTree)
					
	local objMainLayout = uObjTree:GetUIObject("Menu.MainLayout")
	if not objMainLayout then
		TipLog("[ShowMenuHostWnd] find Menu.MainLayout obj failed")
	    return false
	end	
	
	local objNormalMenu = uObjTree:GetUIObject("Menu.Context")
	if not objNormalMenu then
		TipLog("[ShowMenuHostWnd] find normalmenu obj failed")
	    return false
	end	
	objNormalMenu:BindRelateObject(objUIElem)
	objNormalMenu:InitMenuWithRelateObject()
	
	local nL, nT, nR, nB 
	local objMenuFrame = objNormalMenu:GetControlObject("menu.frame")
	if objMenuFrame then
		nL, nT, nR, nB = objMenuFrame:GetObjPos()				
	else
		nL, nT, nR, nB = objNormalMenu:GetObjPos()	
	end	
	
	local nMenuContainerWidth = nR - nL
	local nMenuContainerHeight = nB - nT

	local nMenuLeft, nMenuTop = 0, 0
	if bRBtnPopup then
		nMenuLeft, nMenuTop = tipUtil:GetCursorPos() 	
		nTopSpan = 0 
	else
		nMenuLeft, nMenuTop = GetScreenAbsPos(objUIElem)
	end
	
	local nMenuL, nMenuT, nMenuR, nMenuB = objUIElem:GetAbsPos()
	local nMenuHeight = nMenuB - nMenuT
	if tonumber(nTopSpan) == nil then
		nTopSpan = nMenuHeight
	end
	
	local nMenuLeft, nMenuTop = AdjustScreenEdge(nMenuLeft, nMenuTop, nMenuContainerWidth, nMenuContainerHeight, nTopSpan)
	
	--函数会阻塞
	local bOk = uHostWnd:TrackPopupMenu(objHostWnd, nMenuLeft, nMenuTop, nMenuContainerWidth, nMenuContainerHeight)
	return bOk
end


function GetScreenAbsPos(objUIElem)
	local objTree = objUIElem:GetOwner()
	local objHostWnd = objTree:GetBindHostWnd()
	local nL, nT, nR, nB = objUIElem:GetAbsPos()
	return objHostWnd:HostWndPtToScreenPt(nL, nT)
end


function AdjustScreenEdge(nMenuLeft, nMenuTop, nMenuContainerWidth, nMenuContainerHeight, nTopSpan)
	local nScrnLeft, nScrnTop, nScrnRight, nScrnBottom = tipUtil:GetWorkArea()
	
	if nMenuLeft < nScrnLeft then
		nMenuLeft = nScrnLeft
	end
	
	if nMenuLeft+nMenuContainerWidth > nScrnRight then
		nMenuLeft = nScrnRight - nMenuContainerWidth
	end
	
	if nMenuTop < nScrnTop then
		nMenuTop = nTopSpan+nScrnTop
	elseif nMenuTop+nMenuContainerHeight+nTopSpan > nScrnBottom then
		nMenuTop = nMenuTop - nMenuContainerHeight
	else
		nMenuTop = nTopSpan+nMenuTop
	end	
		
	return nMenuLeft, nMenuTop
end

-------托盘---
local g_tipNotifyIcon = nil

function HideTray()
	if g_tipNotifyIcon then
		g_tipNotifyIcon:Hide()
	end
end


function InitTrayTipWnd(objHostWnd)
    if not objHostWnd then
	    TipLog("[InitTrayTipWnd] para error")
	    return
	end

	--创建托盘
    local tipNotifyIcon = XLGetObject("FR.NotifyIcon")
	if not tipNotifyIcon then
		TipLog("[InitTrayTipWnd] not support NotifyIcon")
	    return
	end
	
	local hostwndManager = XLGetObject("Xunlei.UIEngine.HostWndManager")
	
	----托盘事件响应
	function OnTrayEvent(event1,event2,event3,event4)
		local strHostWndName = "GSTrayMenuHostWnd.MainFrame"
		local newWnd = hostwndManager:GetHostWnd(strHostWndName)	
				
		--单击右键,创建并显示菜单
		if event3 == 517 then
			if not newWnd then
        		CreateTrayTipWnd(objHostWnd)
			end
		end
		
		--单击左键
		if event3 == 0x0202 then
			ShowMainPanleByTray(objHostWnd)
		end
		
		--点击气泡
		if event3 == 1029 then
			if g_bShowWndByTray then
				ShowMainPanleByTray(objHostWnd)	
			end
		end
		
		--mousemove
		if event3 == 512 then
			SetNotifyIconState()
		end
	end

	tipNotifyIcon:Attach(OnTrayEvent)
	g_tipNotifyIcon = tipNotifyIcon
	SetNotifyIconState()
	tipNotifyIcon:Show()
end


function ShowMainPanleByTray(objHostWnd)
	if objHostWnd then
		objHostWnd:Show(5)
		SetWndForeGround(objHostWnd)
		objHostWnd:BringWindowToTop(true)
	end
end


function SetWndForeGround(objHostWnd)
	if not objHostWnd then
		return
	end

	if not IsUserFullScreen() then
		-- objHostWnd:SetTopMost(true)
		if type(tipUtil.SetWndPos) == "function" then
			local hWnd = objHostWnd:GetWndHandle()
			if hWnd ~= nil then
				TipLog("[SetWndForeGround] success")
				tipUtil:SetWndPos(hWnd, 0, 0, 0, 0, 0, 0x0043)
			end
		end
	elseif type(tipUtil.GetForegroundProcessInfo) == "function" then
		local hFrontHandle, strPath = tipUtil:GetForegroundProcessInfo()
		if hFrontHandle ~= nil then
			objHostWnd:BringWindowToBack(hFrontHandle)
		end
	end
end


function SetNotifyIconState(strText)
	g_tipNotifyIcon:SetIcon()
end


function PopupNotifyIconTip(strText, bShowWndByTray)
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local bBubbleRemind = FetchValueByPath(tUserConfig, {"tConfig", "BubbleRemind", "bState"})
	
	if not bBubbleRemind then
		g_bShowWndByTray = false
		return
	end
	
	if IsRealString(strText) and g_tipNotifyIcon then
		g_tipNotifyIcon:ShowNotifyIconTip(true, strText)
	end
	
	g_bShowWndByTray = bShowWndByTray
end


function CreateTrayTipWnd(objHostWnd)
	local uTempltMgr = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local uHostWndMgr = XLGetObject("Xunlei.UIEngine.HostWndManager")
	local uObjTreeMgr = XLGetObject("Xunlei.UIEngine.TreeManager")

	if uTempltMgr and uHostWndMgr and uObjTreeMgr then
		local uHostWnd = nil
		local strHostWndTempltName = "MenuHostWnd"
		local strHostWndTempltClass = "HostWndTemplate"
		local strHostWndName = "TrayMenuHostWnd.MainFrame"
		local uHostWndTemplt = uTempltMgr:GetTemplate(strHostWndTempltName, strHostWndTempltClass)
		if uHostWndTemplt then
			uHostWnd = uHostWndTemplt:CreateInstance(strHostWndName)
		end

		local uObjTree = nil
		local strObjTreeTempltName = "TrayMenuTree"
		local strObjTreeTempltClass = "ObjectTreeTemplate"
		local strObjTreeName = "TrayMenuWnd.MainObjectTree"
		local uObjTreeTemplt = uTempltMgr:GetTemplate(strObjTreeTempltName, strObjTreeTempltClass)

		if uObjTreeTemplt then
			uObjTree = uObjTreeTemplt:CreateInstance(strObjTreeName)
		end

		if uHostWnd and uObjTree then
			--函数会阻塞
			local bSucc = ShowPopupMenu(uHostWnd, uObjTree)
	
			if bSucc and uHostWnd:GetMenuMode() == "manual" then
				uObjTreeMgr:DestroyTree(strObjTreeName)
				uHostWndMgr:RemoveHostWnd(strHostWndName)
			end
		end
	end
end


function ShowPopupMenu(uHostWnd, uObjTree)
	uHostWnd:BindUIObjectTree(uObjTree)
					
	local nPosCursorX, nPosCursorY = tipUtil:GetCursorPos()
	if type(nPosCursorX) ~= "number" or type(nPosCursorY) ~= "number" then
		return false
	end
	
	local nScrnLeft, nScrnTop, nScrnRight, nScrnBottom = tipUtil:GetScreenArea()
	
	local objMainLayout = uObjTree:GetUIObject("Menu.MainLayout")
	if not objMainLayout then
	    return false
	end	
		
	local nL, nT, nR, nB = objMainLayout:GetObjPos()				
	local nMenuContainerWidth = nR - nL
	local nMenuContainerHeight = nB - nT
	local nMenuScreenLeft = nPosCursorX
	local nMenuScreenTop = nPosCursorY - nMenuContainerHeight
	TipLog("[ShowTrayCtrlPanel] about to popup menu")
	
	if nMenuScreenLeft+nMenuContainerWidth > nScrnRight - 10 then
		nMenuScreenLeft = nPosCursorX - nMenuContainerWidth
	end
	
	-- uHostWnd:SetFocus(false) --先失去焦点，否则存在菜单不会消失的bug
	
	--函数会阻塞
	local bOk = uHostWnd:TrackPopupMenu(objHostWnd, nMenuScreenLeft, nMenuScreenTop, nMenuContainerWidth, nMenuContainerHeight)
	
	TipLog("[ShowPopupMenu] end menu")
	
	return bOk
end

-----------托盘--


-------------悬浮窗
local g_objToolTipWnd = nil
function InitToolTip()
	local templateMananger = XLGetObject("Xunlei.UIEngine.TemplateManager")
	local tipsHostWndTemplate = templateMananger:GetTemplate("ToolTipWnd","HostWndTemplate")
	if tipsHostWndTemplate == nil then return nil end
	
	local tipsHostWnd = tipsHostWndTemplate:CreateInstance("ToolTipWndIns")
	if tipsHostWnd == nil then return nil end

	local objectTreeTemplate = templateMananger:GetTemplate("ToolTipTree", "ObjectTreeTemplate")
	if objectTreeTemplate == nil then return nil end
	
	local uiObjectTree = objectTreeTemplate:CreateInstance("ToolTipTreeIns")
	if uiObjectTree == nil then return nil end
	
	tipsHostWnd:BindUIObjectTree(uiObjectTree)

	local nSuccess = tipsHostWnd:Create()
	if not nSuccess then
		return nil
	end
	
	tipsHostWnd:SetVisible(false)
	g_objToolTipWnd = tipsHostWnd
	
	return tipsHostWnd
end


function SetToolTipText(strText)
	local objToolTipWnd = g_objToolTipWnd
	if objToolTipWnd == nil then return end
	
	local objTree = objToolTipWnd:GetBindUIObjectTree()
	if objTree == nil then return end
	
	local objRootCtrl = objTree:GetUIObject("RootCtrl")
	if objRootCtrl == nil then return end
	
	objRootCtrl:SetToolTipText(strText)
end


function SetToolTipPos(objUIItem)
	local objToolTipWnd = g_objToolTipWnd
	if objToolTipWnd == nil or objUIItem == nil then return end
	
	local objToolTipTree = objToolTipWnd:GetBindUIObjectTree()
	if objToolTipTree == nil then return end
	
	local objRootCtrl = objToolTipTree:GetUIObject("RootCtrl")
	if objRootCtrl == nil then return end
	
	local objHostWnd = GetMainWndInst()	
		
	local nToolLeft, nToolTop, nToolRight, nToolBottom = objToolTipWnd:GetWindowRect()
	local nItemLeft, nItemTop, nItemRight, nItemBottom = objUIItem:GetAbsPos()
	local nScrItemL, nScrItemT, nScrItemR, nScrItemB = objHostWnd:HostWndRectToScreenRect(nItemLeft, nItemTop, nItemRight, nItemBottom) 
	local nWidth = objRootCtrl:GetToolTipWidth()
	
	objToolTipWnd:Move(nScrItemL, nScrItemT+20, nWidth, nToolBottom-nToolTop) 

	local nDeskLeft, nDeskTop, nDeskRight, nDeskBottom = tipUtil:GetWorkArea()
	local nToolLeft, nToolTop, nToolRight, nToolBottom = objToolTipWnd:GetWindowRect()
	local nToolHeight = nToolBottom - nToolTop
	
	if nToolRight > nDeskRight then
		local nWidth = objRootCtrl:GetToolTipWidth()
		objToolTipWnd:Move(nDeskRight-nWidth, nToolTop, nWidth, nToolBottom-nToolTop) 
	end
end


local g_hToolTipTimer = nil
function ShowToolTip(bShow, strText, objUIItem)
	local timeMgr = XLGetObject("Xunlei.UIEngine.TimerManager")
	local objToolTipWnd = g_objToolTipWnd
	if objToolTipWnd == nil then return end
	
	if IsRealString(strText) then
		SetToolTipText(strText)
	end
	
	if g_hToolTipTimer then
		timeMgr:KillTimer(g_hToolTipTimer)
	end
	
	if bShow then
		g_hToolTipTimer = timeMgr:SetTimer(function(Itm, id)
			Itm:KillTimer(id)

			SetToolTipPos(objUIItem)
			objToolTipWnd:UpdateWindow()
			objToolTipWnd:DelayPopup(0)
			
		end, 500)	
	else	
		objToolTipWnd:SetVisible(false)
	end
end


-----
function FormatFileSize(nFileSizeInByte)
	local strFileSize = ""
	if tonumber(nFileSizeInByte) == nil then
		return strFileSize
	end

	local nSize = 0
	local strUnit = ""
	if nFileSizeInByte >= 1024*1024*1024 then  
		nSize = nFileSizeInByte/(1024*1024*1024)
		strUnit = "GB"
		
	elseif nFileSizeInByte >= 1024*1024 then  
		nSize = nFileSizeInByte/(1024*1024)
		strUnit = "MB"
		
	elseif nFileSizeInByte >= 1024 then   
		nSize = nFileSizeInByte/1024
		strUnit = "KB"
		
	else
		nSize = nFileSizeInByte
		strUnit = "B"
	end
	
	strFileSize = string.format("%.2f", nSize)
	-- strFileSize = string.gsub(strFileSize, "0+$", "")
	strFileSize = strFileSize..strUnit
	
	return strFileSize
end


function GetDiskSizeInByte(strDirPath)
	if not IsRealString(strDirPath) or not tipUtil:QueryFileExists(strDirPath) then
		return -1
	end

	local nAvailableInByte = tipUtil:GetDiskFreeSpaceEx(strDirPath)
	if tonumber(nAvailableInByte) == nil then
		return -1
	end
	
	return nAvailableInByte
end


function GetDownFileSaveDir()
	local strSaveDir = GetUserSetSaveDir()
	if IsRealString(strSaveDir) and tipUtil:QueryFileExists(strSaveDir) then
		return strSaveDir
	end
	
	return GetDefaultSaveDir()
end


function GetUserSetSaveDir()
	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	local strSaveDir = tUserConfig["strSaveDir"] or ""
	return strSaveDir
end

function SetUserSetSaveDir(strSaveDir)
	if not IsRealString(strSaveDir) then
		return
	end

	local tUserConfig = ReadConfigFromMemByKey("tUserConfig") or {}
	tUserConfig["strSaveDir"] = strSaveDir
	-- SaveConfigToFileByKey("tUserConfig")
end


function GetDefaultSaveDir()
	local strDefaultSaveDir = "c:\\FlyRabbitDownload"
	local strExePath = GetExePath()
	if not IsRealString(strExePath) or not tipUtil:QueryFileExists(strExePath) then
		return strDefaultSaveDir
	end
	
	local _, _, strDiskRoot = string.find(tostring(strExePath), "^(.:\\).*")
	if not IsRealString(strDiskRoot) or not tipUtil:QueryFileExists(strDiskRoot) then
		return strDefaultSaveDir
	end
		
	strDefaultSaveDir = tipUtil:PathCombine(strDiskRoot, "FlyRabbitDownload")
	if not tipUtil:QueryFileExists(strDefaultSaveDir) then
		tipUtil:CreateDir(strDefaultSaveDir)
	end
	
	return strDefaultSaveDir
end


function GetSelectItemObject()
	local objDownloadList = GetMainCtrlChildObj("DownLoadList")
	if not objDownloadList then
		return nil
	end
	
	return objDownloadList:GetSelectItemObject()
end


function GetFileItemUIByIndex(nIndex)
	local objDownloadList = GetMainCtrlChildObj("DownLoadList")
	if not objDownloadList then
		return nil
	end
	
	return objDownloadList:GetFileItemUIByIndex(nIndex)
end


function UpdateFileList()
	local objDownloadList = GetMainCtrlChildObj("DownLoadList")
	objDownloadList:UpdateFileList()
end

function UpdateFileStateUI()
	UpdateFileItemStyle()
	UpdateBottomStyle()
end


function UpdateFileItemStyle()
	local objFileItem = GetSelectItemObject()
	if not objFileItem then
		return
	end
	
	objFileItem:UpdateFileItemStyle()
end


function UpdateBottomStyle()
	local objBottomCtrl = GetMainCtrlChildObj("BottomCtrl")
	if not objBottomCtrl then
		return nil
	end
	
	objBottomCtrl:UpdateBottomStyle()
end


---


local obj = {}
obj.tipUtil = tipUtil
obj.tipAsynUtil = tipAsynUtil

--通用
obj.TipLog = TipLog
obj.MessageBox = MessageBox
obj.GetPeerID = GetPeerID
obj.FailExitTipWnd = FailExitTipWnd
obj.TipConvStatistic = TipConvStatistic
obj.SendLocalReport = SendLocalReport
obj.ExitProcess = ExitProcess
obj.ReportAndExit = ReportAndExit
obj.GetCommandStrValue = GetCommandStrValue
obj.GetExePath = GetExePath
obj.LoadTableFromFile = LoadTableFromFile
obj.CheckIsNewVersion = CheckIsNewVersion
obj.GetFileSaveNameFromUrl = GetFileSaveNameFromUrl
obj.GetFileDirFromPath = GetFileDirFromPath
obj.CheckMD5 = CheckMD5
obj.DownLoadFileWithCheck = DownLoadFileWithCheck

obj.NewAsynGetHttpFile = NewAsynGetHttpFile
obj.GetProgramTempDir = GetProgramTempDir
obj.GetProjectVersion = GetProjectVersion
obj.GetInstallSrc = GetInstallSrc
obj.GetMinorVer = GetMinorVer
obj.GetTimeStamp = GetTimeStamp
obj.OpenFolderDialog = OpenFolderDialog
obj.UrlEncode = UrlEncode
obj.SetWndForeGround = SetWndForeGround

--UI
obj.GetMainWndInst = GetMainWndInst
obj.GetMainCtrlChildObj = GetMainCtrlChildObj
obj.ShowPopupWndByName = ShowPopupWndByName
obj.SetPopupWndCenterByName = SetPopupWndCenterByName
obj.CreatePopupTipWnd = CreatePopupTipWnd
obj.ShowModalDialog = ShowModalDialog

--菜单
obj.TryDestroyOldMenu = TryDestroyOldMenu
obj.CreateAndShowMenu = CreateAndShowMenu

--托盘
obj.InitTrayTipWnd = InitTrayTipWnd
obj.SetNotifyIconState = SetNotifyIconState

--文件
obj.GetCfgPathWithName = GetCfgPathWithName
obj.ReadConfigFromMemByKey = ReadConfigFromMemByKey
obj.SaveConfigToFileByKey = SaveConfigToFileByKey
obj.ReadAllConfigInfo = ReadAllConfigInfo

--升级
obj.DownLoadServerConfig = DownLoadServerConfig
obj.DownLoadNewVersion = DownLoadNewVersion
obj.CheckIsUpdating = CheckIsUpdating
obj.SetIsUpdating = SetIsUpdating
obj.CheckCommonUpdateTime = CheckCommonUpdateTime
obj.SaveCommonUpdateUTC = SaveCommonUpdateUTC
obj.SaveAutoUpdateUTC = SaveAutoUpdateUTC

--注册表
obj.RegQueryValue = RegQueryValue
obj.RegDeleteValue = RegDeleteValue
obj.RegSetValue = RegSetValue


--悬浮窗
obj.InitToolTip = InitToolTip
obj.SetToolTipText = SetToolTipText
obj.ShowToolTip = ShowToolTip
obj.SetToolTipPos = SetToolTipPos


--其他
obj.FormatFileSize = FormatFileSize
obj.GetDiskSizeInByte = GetDiskSizeInByte
obj.GetDefaultSaveDir = GetDefaultSaveDir
obj.GetDownFileSaveDir = GetDownFileSaveDir
obj.SetUserSetSaveDir = SetUserSetSaveDir
obj.GetSelectItemObject = GetSelectItemObject
obj.GetFileItemUIByIndex = GetFileItemUIByIndex
obj.UpdateFileStateUI = UpdateFileStateUI
obj.UpdateBottomStyle = UpdateBottomStyle
obj.UpdateFileList = UpdateFileList

XLSetGlobal("Project.FunctionHelper", obj)

