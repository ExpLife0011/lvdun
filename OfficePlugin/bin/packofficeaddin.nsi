; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "WordEncLock"
;卸载包开关
;!define SWITCH_UNINSTALL 1
!define PRODUCT_VERSION "1.0.0.1"
!define PRODUCT_PUBLISHER "My company, Inc."
!define PRODUCT_WEB_SITE "http://www.mycompany.com"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_INST_ROOT_KEY "HKCU"
!define PRODUCT_MAININFO_FORSELF "Software\WordEncLock"

; MUI 1.67 compatible ------
!include "MUI2.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "Library.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
;!define MUI_LICENSEPAGE_CHECKBOX
;!insertmacro MUI_PAGE_LICENSE "lis.txt"
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "SimpChinese"
Var bIsUpdate
Var strChannelID

; MUI end ------
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}"
InstallDirRegKey HKCU "PRODUCT_MAININFO_FORSELF" "Path"
ShowInstDetails show
ShowUnInstDetails show

/*****************公共函数区域***********/
Function GetLastPart
  Exch $0 ; chop char
  Exch
  Exch $1 ; input string
  Push $2
  Push $3
  StrCpy $2 0
  loop:
    IntOp $2 $2 - 1
    StrCpy $3 $1 1 $2
    StrCmp $3 "" 0 +3
      StrCpy $0 ""
      Goto exit2
    StrCmp $3 $0 exit1
    Goto loop
  exit1:
    IntOp $2 $2 + 1
    StrCpy $0 $1 "" $2
  exit2:
    Pop $3
    Pop $2
    Pop $1
    Exch $0 ; output string
FunctionEnd
Function UpdateChanel
	ReadRegStr $R0 ${PRODUCT_INST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R0 != 0
	${AndIf} $R0 != ""
	${AndIf} $R0 != "unknown"
		StrCpy $strChannelID $R0
	${Else}
		System::Call 'kernel32::GetModuleFileName(i 0, t R2R2, i 256)'
		Push $R2
		Push "\"
		Call GetLastPart
		Pop $R1
		
		${WordFind2X} "$R1" "_" "." -1 $R3
		${If} $R3 == 0
		${OrIf} $R3 == ""
			StrCpy $strChannelID ${INSTALL_CHANNELID}
		${ElseIf} $R1 == $R3
			StrCpy $strChannelID "unknown"
		${Else}
			StrCpy $strChannelID $R3
		${EndIf}
	${EndIf}
FunctionEnd
Function un.UpdateChanel
	ReadRegStr $R4 ${PRODUCT_INST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R4 == 0
	${OrIf} $R4 == ""
		StrCpy $strChannelID "unknown"
	${Else}
		StrCpy $strChannelID $R4
	${EndIf}
FunctionEnd
!macro _CheckMutex
	System::Call "kernel32::CreateMutexA(i 0, i 0, t 'Global\{B4C2C367-61B4-4141-B46B-4ABDF683FEF1}_OfficePluginSetup') i .r1 ?e"
	Pop $R0
	StrCmp $R0 0 +2
	Abort
!macroend

!define CheckMutex "!insertmacro _CheckMutex"

!macro _InitSetup
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "release\WordEncLock.dll"
	File "release\Microsoft.VC90.CRT.manifest"
	File "release\msvcp90.dll"
	File "release\msvcr90.dll"
	File "release\Microsoft.VC90.ATL.manifest"
	File "release\ATL90.dll"
!macroend
!macro _SendState ec ea el ev
	System::Call "$TEMP\${PRODUCT_NAME}\WordEncLock::SendAnyHttpStat(t '${ec}', t '${ea}', t '${el}', i ${ev})"
!macroend
!macro Random
	Exch $0
	Push $1
	System::Call kernel32::QueryPerformanceCounter(*l.r1)
	System::Int64Op $1 % $0
	Pop $0
	Pop $1
	Exch $0
!macroend

!define InitSetup "!insertmacro _InitSetup"
!define SendState "!insertmacro _SendState"

Function Rename4Del
	Exch $0
	Push $1
	Delete $0
	IfFileExists $0 0 RMOK
	BRM:
	Push "1000" 
	!insertmacro Random
	Pop $1
	IfFileExists "$0.$1" BRM
	Rename $0 "$0.$1"
	Delete /REBOOTOK "$0.$1"
	RMOK:
	Pop $1
	Pop $0
FunctionEnd

Function un.Rename4Del
	Exch $0
	Push $1
	Delete $0
	IfFileExists $0 0 RMOK
	BRM:
	Push "1000" 
	!insertmacro Random
	Pop $1
	IfFileExists "$0.$1" BRM
	Rename $0 "$0.$1"
	Delete /REBOOTOK "$0.$1"
	RMOK:
	Pop $1
	Pop $0
FunctionEnd
/**********************END***************/

Name "Word安全锁"
!ifdef SWITCH_UNINSTALL
	OutFile "bin\_uninst.exe"
!else
	OutFile "bin\welsetup${PRODUCT_VERSION}_0001.exe"
!endif

Section "MainSection1" SEC01
	Call DoInstall
	SetOutPath "$INSTDIR"
	SetOverwrite on
	File "bin\uninst.exe"
	CreateDirectory "$STARTMENU\WordEncLock"
	CreateShortCut "$STARTMENU\WordEncLock\卸载Word安全锁.lnk" "$INSTDIR\uninst.exe"
SectionEnd

Section -AdditionalIcons
SectionEnd

Section -Post
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr HKLM "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#
Function .onInit
	!ifdef SWITCH_UNINSTALL
		WriteUninstaller "$EXEDIR\uninst.exe"
		Abort
	!endif
	;检查互斥量
	${CheckMutex}
	;释放help文件到temp目录，判断是否64位
	${InitSetup}
	;更新渠道号
	Call UpdateChanel
	;是否静默安装
	Var /GLOBAL bIsSilent
	StrCpy $bIsSilent 0
	${GetExeName} $R1
	${WordReplace} $CMDLINE $R1 "" +1 $R2
	ClearErrors
	${GetOptions} $R2 "/s" $R0
	IfErrors +2 0
	StrCpy $bIsSilent 1
	${If} $bIsSilent == 0
		ClearErrors
		${GetOptions} $R2 "/silent" $R0
		IfErrors +2 0
		StrCpy $bIsSilent 1
	${EndIf}
	;是否覆盖安装
	ReadRegStr $2 ${PRODUCT_INST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path"
	StrCpy $bIsUpdate 0
	IfFileExists $2 0 StartInstall
		StrCpy $bIsUpdate 1
		${GetFileVersion} $2 $1
		${VersionCompare} $1 ${PRODUCT_VERSION} $3
		${If} $3 == "2" ;已安装的版本低于该版本
			Goto StartInstall
		${Else} 
			;是否强制安装
			ClearErrors
			${GetOptions} $R2 "/w" $R0
			IfErrors +2 0
			Goto StartInstall
			${If} $bIsSilent == 1
				Abort
			${Else}
				 MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "检测到已安装了高版本的插件，是否覆盖安装？" IDYES +2
				 Abort
			${EndIf}
		${EndIf}
	StartInstall:
	${If} $bIsSilent == 1
		;当无界面静默安装该插件后，则不会出现在开始菜单的程序栏，无卸载；
		Call DoInstall
		System::Call "$TEMP\${PRODUCT_NAME}\WordEncLock::Exit() ? u"
		Abort
	${EndIf}
	;${SendState} "ecccccc" "eaaaaaaa" "ellllllllll" 123
FunctionEnd
	
Function DoInstall
	${If} ${RunningX64}
		!define LIBRARY_X64
		!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$INSTDIR\core\WordEncLock64.dll"
		!undef LIBRARY_X64
	${EndIf}
	!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$INSTDIR\core\WordEncLock.dll"
	Push "$INSTDIR\core\WordEncLock.dll"
	Call Rename4Del
	Push "$INSTDIR\core\WordEncLock64.dll"
	Call Rename4Del
	RMDir "$INSTDIR"
	RMDir "$STARTMENU\WordEncLock"
	DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
	SetOutPath "$INSTDIR\core"
	SetOverwrite on
	File "release\ATL90.dll"
	File "release\Microsoft.VC90.ATL.manifest"
	File "release\Microsoft.VC90.CRT.manifest"
	File "release\msvcp90.dll"
	File "release\msvcr90.dll"
	File "release\config.ini"
	${If} ${RunningX64}
		!define LIBRARY_X64
		!insertmacro InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "release\WordEncLock64.dll" "$INSTDIR\core\WordEncLock64.dll" "$INSTDIR\core"
		!undef LIBRARY_X64
	${EndIf}
	!insertmacro InstallLib REGDLL NOTSHARED REBOOT_PROTECTED "release\WordEncLock.dll" "$INSTDIR\core\WordEncLock.dll" "$INSTDIR\core"
	WriteRegStr ${PRODUCT_INST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "Path" "$INSTDIR\core\WordEncLock.dll"
	WriteRegStr ${PRODUCT_INST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource" "$strChannelID"
	${WordFind} "${PRODUCT_VERSION}" "." -1 $R0
	${If} $bIsUpdate == 1
		${SendState} "update" "$R0" "$strChannelID" 1
	${Else}
		${SendState} "install" "$R0" "$strChannelID" 1
	${EndIf}
FunctionEnd

Function .onGUIEnd
	;MessageBox MB_OK "onGUIEnd"
FunctionEnd

Function .onInstSuccess
	;MessageBox MB_OK "onInstSuccess"
	HideWindow
	System::Call "$TEMP\${PRODUCT_NAME}\WordEncLock::Exit() ? u"
FunctionEnd

/************************************************/
/*****************以下是卸载部分*****************/
/************************************************/
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) 已成功地从你的计算机移除。"
  System::Call "$TEMP\${PRODUCT_NAME}\WordEncLock::Exit() ? u"
  SetAutoClose true
FunctionEnd

Function un.onInit
	;检查互斥量
	${CheckMutex}
	IfFileExists "$INSTDIR\core\config.ini" +2
	Abort
	${InitSetup} 
	Call un.UpdateChanel
	MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，其及所有的组件？" IDYES +2
	Abort
FunctionEnd

Section Uninstall
	${If} ${RunningX64}
		!define LIBRARY_X64
		!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$INSTDIR\core\WordEncLock64.dll"
		!undef LIBRARY_X64
	${EndIf}
	!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_PROTECTED "$INSTDIR\core\WordEncLock.dll"
	Push "$INSTDIR\core\WordEncLock.dll"
	Call un.Rename4Del
	Push "$INSTDIR\core\WordEncLock64.dll"
	Call un.Rename4Del
	Delete "$INSTDIR\uninst.exe"
	Delete "$INSTDIR\core\WordEncLock.dll"
	Delete "$INSTDIR\core\WordEncLock64.dll"
	Delete "$INSTDIR\core\config.ini"
	Delete "$INSTDIR\core\msvcr90.dll"
	Delete "$INSTDIR\core\msvcp90.dll"
	Delete "$INSTDIR\core\Microsoft.VC90.CRT.manifest"
	Delete "$INSTDIR\core\Microsoft.VC90.ATL.manifest"
	Delete "$INSTDIR\core\ATL90.dll"

	Delete "$STARTMENU\WordEncLock\卸载Word安全锁.lnk"

	RMDir "$STARTMENU\WordEncLock"
	RMDir "$INSTDIR\core"
	RMDir "$INSTDIR"

	DeleteRegKey HKLM "${PRODUCT_UNINST_KEY}"
	DeleteRegKey ${PRODUCT_INST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}"
	${WordFind} "${PRODUCT_VERSION}" "." -1 $R0
	${SendState} "uninstall" "$R0" "$strChannelID" 1
SectionEnd