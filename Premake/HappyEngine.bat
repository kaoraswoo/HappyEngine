@echo off

set solutionname=HappyEngine
set premakeroot=Core
set currentfolder=%CD%
set solutionpath=HappyEngine

@echo --------------------------------------------------------
@echo .
@echo .
@echo 1. Happy���� ������Ʈ �� �ַ���� �����մϴ�.
@echo .
@echo 2. Visual Studio 2010�� ���ؼ� �ϰ� �����ϵ���  �����Ͽ����ϴ�.
@echo .  
@echo .
@echo --------------------------------------------------------
@echo .

cd %premakeroot%
premake4.exe --outpath=%currentfolder% --file=premake4.lua --solutionname=%solutionname% --solutionpath=%solutionpath% vs2010
pause