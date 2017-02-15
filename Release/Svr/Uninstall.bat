%1 mshta vbscript:CreateObject("Shell.Application").ShellExecute("cmd.exe","/c %~s0 ::","","runas",1)(window.close)&&exit

%~d0
cd %~sdp0

echo %cd%

start LogServer.exe uninstall
start TServer.exe uninstall
start ServerMonitor.exe uninstall

echo exit




