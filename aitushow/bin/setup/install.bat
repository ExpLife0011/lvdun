@echo off
setlocal EnableDelayedExpansion
goto :start
:error
cls
echo ��ʾ^:%1
echo ^-^>��������˳�
pause>nul
exit

:start
set nsisexepath="C:\Program Files (x86)\NSIS\makensis.exe"
set nsispath="%~dp0nsis\install.nsi"
set exepath="%~dp0input_main\program\kuaikan.exe"
set exepath=%exepath:\=\\%
echo %exepath%
for /f "skip=1" %%i in ('wmic datafile where "Name='%exepath:~1,-1%'" get Version') do (
  echo %%i
  set ver=%%i
  echo !ver!
  set ver=!ver:~6,3!
  echo !ver!
  goto :nsis

)
:nsis
if not DEFINED ver (
  call :error ��ȡ������汾ʧ��
) else (
  %nsisexepath% /DBuildNum=!ver! %nsispath%
)

pause
@echo on