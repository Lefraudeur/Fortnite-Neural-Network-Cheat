@echo off
echo.
echo  [+] Starting Build...
echo.
echo ___________________________________________
echo.
"C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin\MSBuild.exe" Fortnite.vcxproj /p:configuration=Release /p:platform=x64
echo.
echo ___________________________________________
echo.
echo  [+] Finished
echo.
pause