Var MSG
Var Dialog
Var BGImage      
Var ImageHandle
Var Bool_Sysstup
Var Handle_Font
Var Int_FontOffset
;����������
Var Lbl_Sumary
;��̬��ȡ������
Var str_ChannelID
Var Hwnd_MsgBox
Var btn_MsgBoxSure
Var btn_MsgBoxCancel
Var lab_MsgBoxText
Var lab_MsgBoxText2
Var Btn_Zuixiaohua
Var Btn_Guanbi
Var PB_ProgressBar

;MUI �ִ����涨�� (1.67 �汾���ϼ���)
!include "MUI2.nsh"
!include "WinCore.nsh"
;�����ļ�����ͷ�ļ�
!include "FileFunc.nsh"
!include "nsWindows.nsh"
!include "WinMessages.nsh"
!include "WordFunc.nsh"
;����64λdll
!include "Library.nsh"

!include "cfg.nsh"
OutFile "..\uninst\_uninst.exe"

!define MUI_CUSTOMFUNCTION_UNGUIINIT un.myGUIInit

;�Զ���ҳ��
UninstPage custom un.FirstMsgBox
UninstPage custom un.MyUnstallMsgBox
UninstPage custom un.MyUnstall

;------------------------------------------------------MUI �ִ����涨���Լ���������------------------------
;Ӧ�ó�����ʾ����
Name "${SHORTCUT_NAME} ${PRODUCT_VERSION}"


InstallDir "$PROGRAMFILES\kuaikantu"
InstallDirRegKey HKLM "${PRODUCT_UNINST_KEY}" "UninstallString"

Section MainSetup
SectionEnd



Function .onInit
	WriteUninstaller "$EXEDIR\uninst.exe"
	Abort
FunctionEnd




/****************************************************/
;ж��
/****************************************************/
Var Bmp_StartUnstall
Var Bmp_FinishUnstall
Var Btn_FinishUnstall

Function un.UpdateChanel
	ReadRegStr $R4 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstallSource"
	${If} $R4 == 0
	${OrIf} $R4 == ""
		StrCpy $str_ChannelID "unknown"
	${Else}
		StrCpy $str_ChannelID $R4
	${EndIf}
FunctionEnd

Var Handle_Font2
Function un.onInit
	${InitMutex}
	IfFileExists "$INSTDIR\program\Microsoft.VC90.CRT.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.CRT.manifest" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcp90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcp90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\msvcr90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\msvcr90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\ATL90.dll" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\ATL90.dll" "$TEMP\${PRODUCT_NAME}\"
	IfFileExists "$INSTDIR\program\Microsoft.VC90.ATL.manifest" 0 InitFailed
	CopyFiles /silent "$INSTDIR\program\Microsoft.VC90.ATL.manifest" "$TEMP\${PRODUCT_NAME}\"
	Goto +2
	InitFailed:
	Abort
	
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	SetOverwrite on
	File "..\bin\kksetuphelper.dll"
	
	StrCpy $Int_FontOffset 4
	CreateFont $Handle_Font "����" 10 0
	CreateFont $Handle_Font2 "����" 8 0
	IfFileExists "$FONTS\msyh.ttf" 0 +4
	CreateFont $Handle_Font "΢���ź�" 10 0
	CreateFont $Handle_Font2 "΢���ź�" 8 0
	StrCpy $Int_FontOffset 0
	
	Call un.UpdateChanel
	
	InitPluginsDir
    File "/ONAME=$PLUGINSDIR\un_startbg.bmp" "..\images\uninst\un_startbg.bmp"
	File "/ONAME=$PLUGINSDIR\loading_head.bmp" "..\images\uninst\loading_head.bmp"
	File "/ONAME=$PLUGINSDIR\loading_finish.bmp" "..\images\uninst\loading_finish.bmp"
	File "/ONAME=$PLUGINSDIR\btn_yes.bmp" "..\images\btn_yes.bmp"
	File "/ONAME=$PLUGINSDIR\btn_no.bmp" "..\images\btn_no.bmp"
	File "/ONAME=$PLUGINSDIR\btn_close.bmp" "..\images\btn_close.bmp"
	File "/ONAME=$PLUGINSDIR\btn_min.bmp" "..\images\btn_min.bmp"
	File "/ONAME=$PLUGINSDIR\btn_confirm.bmp" "..\images\uninst\btn_confirm.bmp"
	File "/oname=$PLUGINSDIR\quit.bmp" "..\images\quit.bmp"
	File "/oname=$PLUGINSDIR\loadingbackground.bmp" "..\images\loadingbackground.bmp"
    File "/oname=$PLUGINSDIR\loadingforeground.bmp" "..\images\loadingforeground.bmp"
	
	SkinBtn::Init "$PLUGINSDIR\btn_confirm.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_min.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_close.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_no.bmp"
	SkinBtn::Init "$PLUGINSDIR\btn_yes.bmp"
FunctionEnd

Function un.onGUICallback
  ${If} $MSG = ${WM_LBUTTONDOWN}
    SendMessage $HWNDPARENT ${WM_NCLBUTTONDOWN} ${HTCAPTION} $0
  ${EndIf}
FunctionEnd

Function un.onMsgBoxCloseCallback
	${If} $MSG = 0x408
		${If} $9 != "userchoice"
			Abort
		${EndIf}
		StrCpy $9 ""
	${EndIf}
FunctionEnd

Function un.myGUIInit
	;�����߿�
    System::Call `user32::SetWindowLong(i$HWNDPARENT,i${GWL_STYLE},0x9480084C)i.R0`
    ;����һЩ���пؼ�
    GetDlgItem $0 $HWNDPARENT 1034
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1035
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1036
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1037
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1038
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1039
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1256
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 1028
    ShowWindow $0 ${SW_HIDE}
FunctionEnd

Function un.DoUninstall
	SendMessage $PB_ProgressBar ${PBM_SETRANGE32} 0 100  ;�ܲ���Ϊ��������ֵ
	Sleep 1
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 2 0
	Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 4 0
	Sleep 100
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 7 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 14 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 27 0
	;ɾ��
	RMDir /r "$INSTDIR\xar"
	Delete "$INSTDIR\uninst.exe"
	RMDir /r "$INSTDIR\program"
	RMDir /r "$INSTDIR\res"
	
	StrCpy "$R0" "$INSTDIR"
	System::Call 'Shlwapi::PathIsDirectoryEmpty(t R0)i.s'
	Pop $R1
	${If} $R1 == 1
		RMDir /r "$INSTDIR"
	${EndIf}
	SendMessage $PB_ProgressBar ${PBM_SETPOS} 50 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 73 0
    Sleep 100
    SendMessage $PB_ProgressBar ${PBM_SETPOS} 100 0
	Sleep 1000
FunctionEnd

Function un.UNSD_TimerFun
	GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::KillTimer $0
	;�ȸɵ���ݷ�ʽ
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
	;${WordFind} "${PRODUCT_VERSION}" "." -1 $R1
	StrCpy $R1 "use"
	ClearErrors
	ReadRegDWORD $R2 HKCU ${PRODUCT_MAININFO_FORSELF} "use"
	IfErrors 0 +2
	StrCpy $R1 "nouse"
	${SendStat} "uninstall" "$R1" $str_ChannelID 1
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::Send2KKAnyHttpStat(t '3', t '$str_ChannelID', t '${PRODUCT_VERSION}')"
	${FKillProc} ${EXE_NAME}
	
	ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
	${VersionCompare} "$R0" "6.0" $2
	${if} $2 == 2
		Delete "$QUICKLAUNCH\${SHORTCUT_NAME}.lnk"
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +2
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::PinToStartMenu4XP(b 0, t '$STARTMENU\${SHORTCUT_NAME}.lnk')"
	${else}
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::NewGetOSVersionInfo(t .R2)"
		;2 ǰ<��1 ǰ > �� 0 ��� 
		${VersionCompare} "$R2" "6.3" $R3
		${If} $R3 == 2
			Call un.GetPinPath
			${If} $0 != "" 
			${AndIf} $0 != 0
				ExecShell taskbarunpin "$0\TaskBar\${SHORTCUT_NAME}.lnk"
				StrCpy $R0 "$0\TaskBar\${SHORTCUT_NAME}.lnk"
				Call un.RefreshIcon
				Sleep 200
				ExecShell startunpin "$0\StartMenu\${SHORTCUT_NAME}.lnk"
				StrCpy $R0 "$0\StartMenu\${SHORTCUT_NAME}.lnk"
				Call un.RefreshIcon
				Sleep 200
			${EndIf}
		${Endif}
	${Endif}
	;��ʼ������
    GetFunctionAddress $0 un.DoUninstall
    BgWorker::CallAndWait
	
	ReadRegStr $0 ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}" "InstDir"
	${NSISLOG} "un.UNSD_TimerFun r0 = $0, INSTDIR = $INSTDIR" 
	${If} $0 == "$INSTDIR"
		DeleteRegValue HKCU ${PRODUCT_MAININFO_FORSELF} "use"
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
		DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
		;ɾ�����õ�ע�����Ϣ
		DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_MAININFO_FORSELF}"
		StrCpy $Bool_Sysstup 0
		${UnSetSysBoot}
		SetOutPath "$TEMP\${PRODUCT_NAME}"
		IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +3
		;��ȡ��������ɾ��key
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetAssociate(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b 0)"
		System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::CreateImgKey(t '.jpg;.jpeg;.jpe;.bmp;.png;.gif;.tiff;.tif;.psd;.ico;.pcx;.tga;.wbm;.ras;.mng;.cr2;.nef;.arw;.dng;.srf;.raf;.wmf;', b 0)"
	${EndIf}

	IfFileExists "$DESKTOP\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$DESKTOP\${SHORTCUT_NAME}.lnk"
	IfFileExists "$STARTMENU\${SHORTCUT_NAME}.lnk" 0 +2
		Delete "$STARTMENU\${SHORTCUT_NAME}.lnk"
	RMDir /r "$SMPROGRAMS\�쿴ͼ"
	
	;HideWindow
	ShowWindow $Bmp_StartUnstall ${SW_HIDE}

	ShowWindow $Bmp_FinishUnstall 1
	ShowWindow $Btn_FinishUnstall 1
	ShowWindow $Lbl_Sumary 1
	ShowWindow $PB_ProgressBar 1
	ShowWindow $Btn_Zuixiaohua 1
	ShowWindow $Btn_Guanbi 1
	EnableWindow $Btn_Guanbi 1
	system::Call `user32::SetWindowText(i $Lbl_Sumary, t "�����")`
	BringToFront
FunctionEnd

Function un.RefreshIcon
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::RefleshIcon(t "$R0")'
FunctionEnd

Function un.GetPinPath
	SetOutPath "$TEMP\${PRODUCT_NAME}"
	System::Call '$TEMP\${PRODUCT_NAME}\kksetuphelper::GetUserPinPath(t) i(.r0)'
FunctionEnd

Function un.OnClick_FinishUnstall
	HideWindow
	IfFileExists "$TEMP\${PRODUCT_NAME}\kksetuphelper.dll" 0 +3
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::WaitForStat()"
	RMDir /r /REBOOTOK $PLUGINSDIR
	RMDir /r /REBOOTOK "$TEMP\${PRODUCT_NAME}"
	System::Call "$TEMP\${PRODUCT_NAME}\kksetuphelper::SetUpExit()"
	System::Call 'kernel32::GetModuleFileName(i 0, t R2R2, i 256)'
	Push $R2
	Push "\"
	Call un.GetLastPart
	Pop $R1
	${If} $R1 == ""
		StrCpy $R1 "Au_.exe"
	${EndIf}
	FindProcDLL::FindProc $R1
	${If} $R0 != 0
		KillProcDLL::KillProc $R1
	${EndIf}
FunctionEnd

Function un.FirstMsgBox
	push $0
	Call un.myGUIInit
	GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
	HideWindow
    nsDialogs::Create 1044
    Pop $1
    ${If} $1 == error
        Call un.OnClick_FinishUnstall
    ${EndIf}
    SetCtlColors $1 ""  transparent ;�������͸��
    ${NSW_SetWindowSize} $HWNDPARENT 309 150 ;�ı䴰���С
    ${NSW_SetWindowSize} $1 309 150 ;�ı�Page��С
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	
	;����
	StrCpy $3 4
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 12 $3 80 20 "�쿴ͼ ж��"
	Pop $0
    SetCtlColors $0 "FFFFFF" transparent ;�������͸��
	SendMessage $0 ${WM_SETFONT} $Handle_Font 0
	;�ر�
	${CreateButton} 279 3 22 22 "btn_close.bmp" un.OnClick_FinishUnstall $4
	
	;��
	${CreateButton} 139 114 73 25 "btn_yes.bmp" un.ClickSure2 $4
	;��
	${CreateButton} 223 114 73 25 "btn_no.bmp" un.OnClick_FinishUnstall $4
	
	GetFunctionAddress $0 un.onGUICallback
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\un_startbg.bmp $ImageHandle
	
	WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 un.onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
	
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
	
	ShowWindow $HWNDPARENT ${SW_SHOW}
	BringToFront
FunctionEnd

Function un.GsMessageBox
	IsWindow $Hwnd_MsgBox Create_End
	GetDlgItem $0 $HWNDPARENT 1
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 2
    ShowWindow $0 ${SW_HIDE}
    GetDlgItem $0 $HWNDPARENT 3
    ShowWindow $0 ${SW_HIDE}
	HideWindow
    nsDialogs::Create 1044
    Pop $Hwnd_MsgBox
    ${If} $Hwnd_MsgBox == error
        Call un.OnClick_FinishUnstall
    ${EndIf}
    SetCtlColors $Hwnd_MsgBox ""  transparent ;�������͸��
    ${NSW_SetWindowSize} $HWNDPARENT 309 150 ;�ı䴰���С
    ${NSW_SetWindowSize} $Hwnd_MsgBox 309 150 ;�ı�Page��С
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	
	StrCpy $3 4
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 12 $3 65 20 "��ʾ"
	Pop $0
    SetCtlColors $0 "FFFFFF" transparent ;�������͸��
	SendMessage $0 ${WM_SETFONT} $Handle_Font 0
	;�ر�
	${CreateButton} 279 5 22 22 "btn_close.bmp" un.OnClick_FinishUnstall $0
	
	${NSD_CreateButton} 139 114 73 25 ''
	Pop $btn_MsgBoxSure
	StrCpy $1 $btn_MsgBoxSure
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_yes.bmp $1
	SkinBtn::onClick $1 $R7
	${NSD_CreateButton} 223 114 73 25 ''
	Pop $btn_MsgBoxCancel
	StrCpy $1 $btn_MsgBoxCancel
	SkinBtn::Set /IMGID=$PLUGINSDIR\btn_no.bmp $1
	GetFunctionAddress $0 un.OnClick_FinishUnstall
	SkinBtn::onClick $1 $0
	${If} $R8 == ""
		StrCpy $3 60
	${Else}
		StrCpy $3 55
	${EndIf}
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R6
	Pop $lab_MsgBoxText
    SetCtlColors $lab_MsgBoxText "FFFFFF" transparent ;�������͸��
	SendMessage $lab_MsgBoxText ${WM_SETFONT} $Handle_Font 0
	StrCpy $3 75
	IntOp $3 $3 + $Int_FontOffset
	${NSD_CreateLabel} 66 $3 250 20 $R8
	Pop $lab_MsgBoxText2
    SetCtlColors $lab_MsgBoxText2 "FFFFFF" transparent ;�������͸��
	SendMessage $lab_MsgBoxText2 ${WM_SETFONT} $Handle_Font 0

	GetFunctionAddress $0 un.onGUICallback

    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $BGImage
    ${NSD_SetImage} $BGImage $PLUGINSDIR\quit.bmp $ImageHandle

	WndProc::onCallback $BGImage $0 ;�����ޱ߿����ƶ�

	GetFunctionAddress $0 un.onMsgBoxCloseCallback

	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ

	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
	${NSD_FreeImage} $ImageHandle
	Create_End:
	HideWindow
	System::Call  'User32::GetDesktopWindow() i.r8'
	${NSW_CenterWindow} $HWNDPARENT $8
	system::Call `user32::SetWindowText(i $lab_MsgBoxText, t "$R6")`
	system::Call `user32::SetWindowText(i $lab_MsgBoxText2, t "$R8")`
	SkinBtn::onClick $btn_MsgBoxSure $R7
	
	ShowWindow $HWNDPARENT ${SW_SHOW}
	ShowWindow $Hwnd_MsgBox ${SW_SHOW}
	BringToFront
FunctionEnd

Function un.ClickSure
	${FKillProc} ${EXE_NAME}
	Call un.ClickSure2
FunctionEnd

Function un.ClickSure2
	StrCpy $9 "userchoice"
	SendMessage $HWNDPARENT "0x408" "1" ""
FunctionEnd

Function un.MyUnstallMsgBox
	;���˳���Ϣ
	FindProcDLL::FindProc "${EXE_NAME}.exe"
	${If} $R0 != 0
		StrCpy $R6 "��⵽${SHORTCUT_NAME}�������У��Ƿ�ǿ�ƽ�����"
		StrCpy $R8 ""
		GetFunctionAddress $R7 un.ClickSure
		Call un.GsMessageBox
	${Else}
		Call un.ClickSure
	${EndIf}
FunctionEnd

Function un.MyUnstall
	;push $HWNDPARENT
	;call un.myGUIInit
	GetDlgItem $0 $HWNDPARENT 1
	ShowWindow $0 ${SW_HIDE}
	GetDlgItem $0 $HWNDPARENT 2
	ShowWindow $0 ${SW_HIDE}
	GetDlgItem $0 $HWNDPARENT 3
	ShowWindow $0 ${SW_HIDE}
	
	HideWindow
	nsDialogs::Create 1044
	Pop $0
	${If} $0 == error
		Call un.OnClick_FinishUnstall
	${EndIf}
	SetCtlColors $0 ""  transparent ;�������͸��

	${NSW_SetWindowSize} $HWNDPARENT 548 388 ;�ı��Զ��崰���С
	${NSW_SetWindowSize} $0 548 388 ;�ı��Զ���Page��С
	System::Call 'user32::GetDesktopWindow()i.R9'
	${NSW_CenterWindow} $HWNDPARENT $R9
	
	;������Ҫ˵��
	StrCpy $3 317
	IntOp $3 $3 + $Int_FontOffset
    ${NSD_CreateLabel} 33 $3 90 20 "����ж��..."
    Pop $Lbl_Sumary
    SetCtlColors $Lbl_Sumary "FFFFFF"  transparent ;�������͸��
	SendMessage $Lbl_Sumary ${WM_SETFONT} $Handle_Font 0
	
	;������
	 ${NSD_CreateProgressBar} 33 345 484 14 ""
    Pop $PB_ProgressBar
    SkinProgress::Set $PB_ProgressBar "$PLUGINSDIR\loadingforeground.bmp" "$PLUGINSDIR\loadingbackground.bmp"
	GetFunctionAddress $0 un.UNSD_TimerFun
    nsDialogs::CreateTimer $0 1
	
	;ȷ��
	${CreateButton} 215 247 120 32 "btn_confirm.bmp" un.OnClick_FinishUnstall $Btn_FinishUnstall
	ShowWindow $Btn_FinishUnstall ${SW_HIDE}
	
	;��С��
	${CreateButton} 488 5 22 22 "btn_min.bmp" un.onClickZuixiaohua $Btn_Zuixiaohua
	;�ر�
	${CreateButton} 520 5 22 22 "btn_close.bmp" un.OnClick_FinishUnstall $Btn_Guanbi
	EnableWindow $Btn_Guanbi 0
	
	GetFunctionAddress $0 un.onGUICallback  
	;ж����ɱ���
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_FinishUnstall
    ${NSD_SetImage} $Bmp_FinishUnstall $PLUGINSDIR\loading_finish.bmp $ImageHandle
    WndProc::onCallback $Bmp_FinishUnstall $0 ;�����ޱ߿����ƶ�
	ShowWindow $Bmp_FinishUnstall ${SW_HIDE}
	
    ;��������ͼ
    ${NSD_CreateBitmap} 0 0 100% 100% ""
    Pop $Bmp_StartUnstall
    ${NSD_SetImage} $Bmp_StartUnstall $PLUGINSDIR\loading_head.bmp $ImageHandle
    WndProc::onCallback $Bmp_StartUnstall $0 ;�����ޱ߿����ƶ�
	
	GetFunctionAddress $0 un.onMsgBoxCloseCallback
	WndProc::onCallback $HWNDPARENT $0 ;����ر���Ϣ
    
	ShowWindow $HWNDPARENT ${SW_SHOW}
	nsDialogs::Show
    ${NSD_FreeImage} $ImageHandle
	BringToFront
FunctionEnd

Function un.onClickZuixiaohua
	 ShowWindow $HWNDPARENT 2
FunctionEnd