@echo off

set solutionname=HappyEngine
set premakeroot=Core
set currentfolder=%CD%
set solutionpath=HappyEngine

@echo --------------------------------------------------------
@echo .
@echo .
@echo 1. Happy엔진 프로젝트 및 솔루션을 생성합니다.
@echo .
@echo 2. Visual Studio 2010에 대해서 일괄 생성하도록  설정하였습니다.
@echo .  
@echo .
@echo --------------------------------------------------------
@echo .

cd %premakeroot%
premake4.exe --outpath=%currentfolder% --file=premake4.lua --solutionname=%solutionname% --solutionpath=%solutionpath% vs2010
pause