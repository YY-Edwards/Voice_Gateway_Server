%1 mshta vbscript:CreateObject("Shell.Application").ShellExecute("cmd.exe","/c %~s0 ::","","runas",1)(window.close)&&exit

%~d0
cd %~sdp0

echo %cd%

start Dispatch.exe install
start WirelineSchedule.exe install


echo exit