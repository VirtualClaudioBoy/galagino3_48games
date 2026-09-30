@echo off
setlocal
pushd "%~dp0rocnrope"
python rocnrope_rom_convert.py
set "convert_error=%errorlevel%"
popd
endlocal & exit /b %convert_error%
