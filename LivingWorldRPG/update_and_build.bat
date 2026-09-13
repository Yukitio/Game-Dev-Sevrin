@echo off
setlocal

set "PROJECT_DIR=%~dp0"
set "REPO_DIR=%PROJECT_DIR%.."
set "ENGINE_BUILD_BAT=C:\Game Project Sevrin\UE_5.5\Engine\Build\BatchFiles\Build.bat"
set "BUILD_LOG=%TEMP%\lwrpg_build.log"

echo ================================================
echo  Discarding editor config rewrites...
echo ================================================
pushd "%REPO_DIR%"
rem Just opening the editor / Project Settings rewrites the whole
rem Config/*.ini (engine defaults included), which otherwise blocks
rem the pull below with "local changes would be overwritten". If you
rem deliberately changed a project setting in the editor and want to
rem keep it, tell Claude about it BEFORE running this script again so
rem it gets committed properly instead of discarded here.
git checkout -- LivingWorldRPG/Config
echo Done.

echo.
echo ================================================
echo  Pulling latest changes from GitHub...
echo ================================================
git pull origin claude/new-session-un5x84
if errorlevel 1 (
    echo.
    echo [FAILED] git pull did not complete cleanly - see above.
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
call "%ENGINE_BUILD_BAT%" LivingWorldRPGEditor Win64 Development -Project="%PROJECT_DIR%LivingWorldRPG.uproject" -WaitMutex > "%BUILD_LOG%" 2>&1
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
