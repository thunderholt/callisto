Echo Launch dir: "%~dp0"
Echo Current dir: "%CD%"
xcopy "%~dp0Compiled\*" "%~dp0..\Windows\Build\Callisto\Assets" /s /i /d /y