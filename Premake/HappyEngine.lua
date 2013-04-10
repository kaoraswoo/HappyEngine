------------------------------------------------------------------------------------------------------------------------
--solution.lua 
-- 여기서 솔루션을 정의하고, 들어갈 프로젝트들을 정의한다.
--  This file is a main start point of Premake. You can define solution and projects list.
------------------------------------------------------------------------------------------------------------------------


----------------------------------------------------------------------------------------
--You can Add Project list by 3 types
--1. AddProjectGroup( "Group name", "Virtual Folder name" )
--2. AddProject( "Project name" )  or  AddProject( "Project name",  "Virtual Folder name" )  or  AddProject( "Virtual Folder name",  "Parents Virtual Folder name" )
--3. AddProjectlist( "List" )  or  AddProjectlist( "list",  "Virtual Folder name" )
----------------------------------------------------------------------------------------		
	OffProjectGeneration()	
	AddProjects(  {"D3DContent", "ManagedHEngine"} , "Facade" )
	OnProjectGeneration()
	AddProjects(  {"HEngine"} , "Test" )
	


----------------------------------------------------------------------------------------
--Load Project List
----------------------------------------------------------------------------------------
	LoadProjectList("Projects_HappyEngine.lua")	

	
----------------------------------------------------------------------------------------
-- It determines Build or Not ( 빌드 여부를 결정한다)
-- If you write "Configure|Platform" on g_var_projectbuildcheck, that project will build in the "Configure|Platform"
-- You can write only Configure  or, only Platform
----------------------------------------------------------------------------------------					
--	Clear_Build_Platform()  --Clear default Build setting.. Instead of it, We set it manually like below
	
--	Set_Build_Platform( { "Debug|Win32", "Debug_PS3Host|Win32", "Profile|Win32" , "Release|Win32", "Release_PS3Host|Win32"} , 프로젝트명 )
--	Set_Build_Platform( { "Debug|Win32", "Debug_PS3Host|Win32", "Profile|Win32" , "Release|Win32", "Release_PS3Host|Win32", "Debug|PS3", "Profile|PS3", "Release|PS3"}, 프로젝트명 )
