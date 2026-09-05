@echo off
setlocal

set "PROJECT_DIR=%~dp0"
set "REPO_DIR=%PROJECT_DIR%.."
set "ENGINE_BUILD_BAT=C:\Game Project Sevrin\UE_5.5\Engine\Build\BatchFiles\Build.bat"
set "BUILD_LOG=%TEMP%\lwrpg_build.log"

echo ================================================
echo  Pulling latest changes from GitHub...
echo ================================================
pushd "%REPO_DIR%"
git pull origin claude/new-session-un5x84
if errorlevel 1 (
    echo.
    echo [FAILED] git pull did not complete cleanly - see above.
    echo If it mentions "local changes would be overwritten", that
    echo usually means the editor modified a config file on its own.
    popd
    pause
    exit /b 1
)
popd

echo.
echo ================================================
echo  Clearing old build cache...
echo ================================================
if exist "%PROJECT_DIR%Intermediate" rmdir /s /q "%PROJECT_DIR%Intermediate"
if exist "%PROJECT_DIR%Binaries" rmdir /s /q "%PROJECT_DIR%Binaries"
echo Done.

echo.
echo ================================================
echo  Building LivingWorldRPG (full rebuild, ~1-2 min)...
echo  (this is quiet until it finishes - that's normal)
echo ================================================
"%ENGINE_BUILD_BAT%" LivingWorldRPGEditor Win64 Development -Project="%PROJECT_DIR%LivingWorldRPG.uproject" -WaitMutex > "%BUILD_LOG%" 2>&1
set BUILD_RESULT=%errorlevel%

echo.
if %BUILD_RESULT% neq 0 (
    echo ================================================
    echo  BUILD FAILED
    echo ================================================
    echo Error lines from the build:
    echo ------------------------------------------------
    findstr /I "error" "%BUILD_LOG%"
    echo ------------------------------------------------
    echo Copy everything between the lines above and send it.
    echo ^(Full log saved at: %BUILD_LOG% if more detail is needed^)
) else (
    echo ================================================
    echo  BUILD SUCCEEDED
    echo ================================================
    echo You can now open LivingWorldRPG.uproject.
)

echo.
pause
