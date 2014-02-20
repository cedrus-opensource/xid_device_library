setlocal

set PYTHONPATH=%PYTHONPATH%;..\scons_helpers\scons\
:: Append SCONS_DIR to the python path
set PYTHONPATH=%PYTHONPATH%;%SCONS_DIR%

set CEDRUS_BUILD_CMD=scons %*

echo %CEDRUS_BUILD_CMD%

%CEDRUS_BUILD_CMD%
