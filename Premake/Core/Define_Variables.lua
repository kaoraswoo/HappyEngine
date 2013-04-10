------------------------------------------------------------------------------------------------------------------------
--DefineVatiables.lua 
-- 프로젝트에서 공통으로 쓰일 변수 선언 가지고 있는 파일
--  This file contains Common variables and this is for variable-clear 
current_file =  "Define_Variables.lua"
------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
--  Global Variable Declaration
----------------------------------------------------------------------------------------
g_project_folder=nil  --Each Project Folder Path
project_name=nil 
g_include_dir = {}   -- Include directory
g_forceinclude_dir = {}  -- forceinclude directory
g_file_list = {}    -- source file list ( *.cpp,  *.h, and so on )
g_link_list = {}   -- external link library list ( *.lib , *.a )
g_link_list_project = {}  -- Reference Project List 
g_libdir_list = {}  -- Library directory Path list
g_vpath_list= {}  -- Virtual Path
g_target_path =  {}   -- Target Path ( Output Directory )
g_targetname_path= {}   -- TargetName
g_defines= {}  -- PreProcessor list
g_flags={}  -- Compile Setting
g_obj_path={}  -- Intermediate Directory
g_exclude_build_folder_keyword={}  -- excluded Build folder path list
g_exclude_build_file_keyword={}  -- excluded Build file list
g_exclude_list={} --  exclude Source Files
g_ignorelib={}  -- Ignore Library
g_buildoption={}  -- Additional Build Command Option
g_linkoption={}  -- Additional Link Command Option
g_moduledef={}  -- Module Definition File List
g_prebuildcommands={}  --Configuration - Build Events - Pre-Build Event 
g_postbuildcommands={} --Build Events - Post-Build Event 
g_custombuildcommands={} --Configuration - Custom Build Step - General - Command Line 
g_custombuildoutputs={} --Configuration - Custom Build Step - General - Outputs 
g_pchcheck={} -- Pre Compile Header
g_projectset={}  --프로젝트set에 대한 설정
g_buildset={}  --프로젝트set에 대한 설정
g_projectbuildcheck={}  --It does not depend on platform and configuration.. It is applied per each project.
g_svpathslist={} -- VirtualPath by Solution Controlled
g_final_svpathslist={} -- Final VirtualPath by Solution Controlled
g_projectgroup={} 
g_uuid={}  -- Project GUID list
g_ttinclude={} -- Add TT file for project
g_ttincludeoutput={}  --Add TT file (*.cpp, *.h) for project
g_importproject={}  -- ImportProject
g_importprops={}  -- ImportProject

----------------------------------------------------------------------------------------
-- project기본세팅관련 변수들
-- project_name, project_type, project_folder, Common_file, ProjectsManageFile List
----------------------------------------------------------------------------------------
g_projectname_list = {}  -- 1차원 리스트
g_project_type = {}
g_project_lang = {}
g_project_folder = {}
g_common_file_list= {}
g_final_project_folder={}
g_project_managers={}
g_generateproject={} -- Generate Project
g_generateprojectgroup={}

----------------------------------------------------------------------------------------
--  Wrapper functions
--  Contol upper global values
--  위의 global 변수들(g_    )  에 값을 넣는 wrapper 함수
----------------------------------------------------------------------------------------
function AddTable(targetlist, srclist)
	if targetlist==nil then
		targetlist={}
    end
	
	if type(srclist) == "table" then
		for k,v in ipairs(srclist) do
			table.insert(targetlist, v)
		end 
	else
		table.insert( targetlist, srclist )
	end
	
	return targetlist
end


function SetTable(targetlist, srclist)
	if targetlist==nil then
		targetlist={}
    end
	
	if type(srclist) == "table" then
		targetlist = srclist
	else
		targetlist= { srclist}
	end
	
	return targetlist
end

----------------------------------------------------------------------------------------
-- Declare_Key
-- Variable wrapper
----------------------------------------------------------------------------------------
function Declare_Key(project_name)
	if project_name == nil then
		project_name = g_project_name
	end
	g_include_dir[project_name] = {}
	g_forceinclude_dir[project_name] = {}
	g_file_list[project_name] = {}
	g_link_list[project_name] = {}
	g_link_list_project[project_name] = {}
	g_libdir_list[project_name] = {}
	g_vpath_list[project_name]= {}
	g_target_path[project_name] =  {}
	g_targetname_path[project_name]= {}
	g_defines[project_name]= {}
	g_flags[project_name]={}
	g_obj_path[project_name]={}
	g_exclude_build_folder_keyword[project_name]={}
	g_exclude_build_file_keyword[project_name]={}
	g_exclude_list[project_name]={}
	g_ignorelib[project_name]={}
	g_buildoption[project_name]={}
	g_linkoption[project_name]={}
	g_moduledef[project_name]={}
	g_prebuildcommands[project_name]={}
	g_postbuildcommands[project_name]={}
	g_custombuildcommands[project_name]={}
	g_custombuildoutputs[project_name]={}
	g_projectset[project_name]={}  --프로젝트set에 대한 설정
	g_buildset[project_name]={}  --프로젝트set에 대한 설정
	g_projectbuildcheck[project_name]={}  --It does not depend on platform and configuration.. It is applied per each project.
	g_pchcheck[project_name]="Y"
	g_uuid[project_name]={}
	g_ttinclude[project_name]={}
	g_ttincludeoutput[project_name]={}
	g_importproject[project_name]={}
	g_importprops[project_name]={}
	
	----------------------------------------------------------------------------------------
	-- project기본세팅관련 변수들
	-- project_name, project_type, project_folder, Common_file
	----------------------------------------------------------------------------------------
	g_projectname_list[project_name] = {}  -- 1차원 리스트
	g_project_type[project_name]= nil
	g_project_lang[project_name]= nil
	g_project_folder[project_name]= nil
	g_common_file_list[project_name]= {}
end

----------------------------------------------------------------------------------------
--  Control_Key
--  control_mode : Add / Set
--  keyword :  global variables
--  conplat : Configuration|Platform ( ex: Debug|PS3 )
--  list : values
----------------------------------------------------------------------------------------
function Control_Key(control_mode, keywordlist, confplat, list, project_name)
	if list == nil then
        return
    end
	if project_name == nil then
		project_name = g_project_name
	end
	
	if keywordlist == "g_project_type" then
		if type(list) == "table" then
			print ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
			return
		end
		g_project_type[project_name] = list
	elseif keywordlist == "g_project_lang" then
		if type(list) == "table" then
			print ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
			return
		end
		g_project_lang[project_name] = list
		
	elseif keywordlist == "g_project_folder" then
		if type(list) == "table" then
			print ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
			return
		end
		g_project_folder[project_name] = list

	elseif keywordlist == "g_pchcheck" then
		if type(list) == "table" then
			print ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
			return
		end
			g_pchcheck[project_name] = list
			
	elseif keywordlist == "g_generateproject" then
		if type(list) == "table" then
			print ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
			return
		end
			g_generateproject[project_name] = list
			
	else
		if control_mode == "Add" then
			if type(confplat) == "table" then
				for i, iterConfPlat in ipairs(confplat) do
					keywordlist[project_name][iterConfPlat] = AddTable( keywordlist[project_name][iterConfPlat] , list )
				end
			else
					keywordlist[project_name][confplat] = AddTable( keywordlist[project_name][confplat] , list )
			end
		elseif control_mode == "Set" then
			if type(confplat) == "table" then
				for i, iterConfPlat in ipairs(confplat) do
					keywordlist[project_name][iterConfPlat] = list
				end
			else
					keywordlist[project_name][confplat] = list
			end
		end
	end
	
end



----------------------------------------------------------------------------------------
--  Wrapper functions
--  Contol upper functions
--  위의 wrapper를 읽기 쉽게 한 함수화
----------------------------------------------------------------------------------------
function Add_Key( keyword, confplat, list, project_name )
	Control_Key("Add", keyword, confplat, list, project_name)
end

function Set_Key( keyword, confplat, list, project_name )
	Control_Key("Set", keyword, confplat, list, project_name)
end


function Add_Include( conplat, list, project_name )
	Control_Key( "Add", g_include_dir, conplat, list, project_name )
end
function Set_Include( conplat, list, project_name )
	Control_Key( "Set", g_include_dir, conplat, list, project_name )
end

function Add_ForceInclude( conplat, list, project_name )
	Control_Key( "Add", g_forceinclude_dir, conplat, list, project_name )
end
function Set_ForceInclude( conplat, list, project_name )
	Control_Key( "Set", g_forceinclude_dir, conplat, list, project_name )
end

function Add_SourceFile( conplat, list, project_name )
		for i, v in ipairs( list ) do
			list[i] = g_final_project_folder..list[i]
		end
	Control_Key( "Add", g_file_list, conplat, list, project_name )
end
function Set_SourceFile( conplat, list, project_name )
	Control_Key( "Set", g_file_list, conplat, list, project_name )
end

function Add_Link( conplat, list, project_name )
	Control_Key( "Add", g_link_list , conplat, list, project_name )
end
function Set_Link( conplat, list, project_name )
	Control_Key( "Set", g_link_list , conplat, list, project_name )
end

function Add_Link_Project( conplat, list, project_name )
	Control_Key( "Add", g_link_list_project , conplat, list, project_name )
end
function Set_Link_Project( conplat, list, project_name )
	Control_Key( "Set", g_link_list_project , conplat, list, project_name )
end

function Add_Lib_Dir( conplat, list, project_name )
	Control_Key( "Add", g_libdir_list , conplat, list, project_name )
end
function Set_Lib_Dir( conplat, list, project_name )
	Control_Key( "Set", g_libdir_list , conplat, list, project_name )
end

function Add_P_Vpath( conplat, list, project_name )
	Control_Key( "Add", g_vpath_list  , conplat, list, project_name )
end
function Set_P_Vpath( conplat, list, project_name )
	Control_Key( "Set", g_vpath_list  , conplat, list, project_name )
end

function Set_Target_Path( conplat, list, project_name )
	if type(list) == "table" then
		error ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
		return
	end
	Control_Key( "Set", g_target_path   , conplat, list, project_name )
end

function Set_Target_Name_Path( conplat, list, project_name )
	if type(list) == "table" then
		error ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
		return
	end
	Control_Key( "Set", g_targetname_path   , conplat, list, project_name )
end

function Set_Obj_Path( conplat, list, project_name )
	if type(list) == "table" then
		error ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
		return
	end
	Control_Key( "Set", g_obj_path   , conplat, list, project_name )
end

function Set_BuildSet( conplat, list, project_name )
	if type(list) == "table" then
		error ( current_file.."  ".. project_name.."    :    "..keyword.." is not list type, You have to use one element instead of { list }" )
		return
	end
	Control_Key( "Set", g_buildset  , conplat, list, project_name )
end

function Add_Defines( conplat, list, project_name )
	Control_Key( "Add", g_defines  , conplat, list, project_name )
end
function Set_Defines( conplat, list, project_name )
	Control_Key( "Set", g_defines  , conplat, list, project_name )
end

function Add_Flags( conplat, list, project_name )
	Control_Key( "Add", g_flags  , conplat, list, project_name )
end
function Set_Flags( conplat, list, project_name )
	Control_Key( "Set", g_flags  , conplat, list, project_name )
end

function Add_Exclude_Build_Folder( conplat, list, project_name )
	Control_Key( "Add", g_exclude_build_folder_keyword  , conplat, list, project_name )
end
function Set_Exclude_Build_Folder( conplat, list, project_name )
	Control_Key( "Set", g_exclude_build_folder_keyword  , conplat, list, project_name )
end

function Add_Exclude_Build_File( conplat, list, project_name )
		for i, v in ipairs( list ) do
			list[i] = g_final_project_folder..list[i]
		end
	Control_Key( "Add", g_exclude_build_file_keyword  , conplat, list, project_name )
end
function Set_Exclude_Build_File( conplat, list, project_name )
	Control_Key( "Set", g_exclude_build_file_keyword  , conplat, list, project_name )
end

function Add_Exclude_SourceFile( conplat, list, project_name )
		for i, v in ipairs( list ) do
			list[i] = g_final_project_folder..list[i]
		end
	Control_Key( "Add", g_exclude_list  , conplat, list, project_name )
end
function Set_Exclude_SourceFile( conplat, list, project_name )
	Control_Key( "Set", g_exclude_list  , conplat, list, project_name )
end

function Add_Ignore_Lib( conplat, list, project_name )
	Control_Key( "Add", g_ignorelib   , conplat, list, project_name )
end
function Set_Ignore_Lib( conplat, list, project_name )
	Control_Key( "Set", g_ignorelib   , conplat, list, project_name )
end

function Add_BuildOption( conplat, list, project_name )
	Control_Key( "Add", g_buildoption  , conplat, list, project_name )
end
function Set_BuildOption( conplat, list, project_name )
	Control_Key( "Set", g_buildoption  , conplat, list, project_name )
end

function Add_LinkOption( conplat, list, project_name )
	Control_Key( "Add", g_linkoption  , conplat, list, project_name )
end
function Set_LinkOption( conplat, list, project_name )
	Control_Key( "Set", g_linkoption  , conplat, list, project_name )
end

function Add_ModuleDef( conplat, list, project_name )
	Control_Key( "Add", g_moduledef  , conplat, list, project_name )
end
function Set_ModuleDef( conplat, list, project_name )
	Control_Key( "Set", g_moduledef  , conplat, list, project_name )
end

function Add_PreBuild( conplat, list, project_name )
	Control_Key( "Add", g_prebuildcommands  , conplat, list, project_name )
end
function Set_PreBuild( conplat, list, project_name )
	Control_Key( "Set", g_prebuildcommands  , conplat, list, project_name )
end

function Add_PostBuild( conplat, list, project_name )
	Control_Key( "Add", g_postbuildcommands  , conplat, list, project_name )
end
function Set_PostBuild( conplat, list, project_name )
	Control_Key( "Set", g_postbuildcommands  , conplat, list, project_name )
end

function Add_CustomBuild( conplat, list, project_name )
	Control_Key( "Add", g_custombuildcommands  , conplat, list, project_name )
end
function Set_CustomBuild( conplat, list, project_name )
	Control_Key( "Set", g_custombuildcommands  , conplat, list, project_name )
end

function Add_CustomBuildOutput( conplat, list, project_name )
	Control_Key( "Add", g_custombuildoutputs  , conplat, list, project_name )
end
function Set_CustomBuildOutput( conplat, list, project_name )
	Control_Key( "Set", g_custombuildoutputs  , conplat, list, project_name )
end

function Add_VirtualPath( path_name, project_name )
	g_svpathslist[path_name] =AddTable(g_svpathslist[path_name], project_name )
end

function Set_VirtualPath( path_name, project_name )
	g_svpathslist[path_name] =AddTable(g_svpathslist[path_name], project_name )
end

--g_project_type
--g_project_folder    이 두개는 래퍼없이

--g_common_file_list  
function Add_CommonFile( list, project_name )
	Control_Key( "Add", g_common_file_list  , "All", list, project_name )
end
function Set_CommonFile( list, project_name )
	Control_Key( "Set", g_common_file_list  , "All", list, project_name )
end

function Do_CommonFile()
	if  g_common_file_list[g_project_name]["All"] ~= nil then
		for i,v in ipairs( g_common_file_list[g_project_name]["All"] ) do
			
			if os.isfile( premake_directory.."/../Common/"..v) then
				dofile( premake_directory.."/../Common/"..v )
			else
				print( g_project_name.." : ".. premake_directory.."/../Common/"..v.."   is not exist." )
			end
		end
		Set_CommonFile(  {} )
	end
end

--g_projectset
--g_buildset
--g_projectbuildcheck   이 세개도 다른 래퍼가 존재함?

function PreCompiledHeader( list, project_name )
	Control_Key( "Set", "g_pchcheck" , "All" , list, project_name )
end

function Set_Uuid( conplat, list, project_name )
	Control_Key( "Set", g_uuid  , conplat, list, project_name )
end

function Add_TTFile( conplat, list, project_name )
	Control_Key( "Add", g_ttinclude  , conplat, list, project_name )
end

function Set_TTFile( conplat, list, project_name )
	Control_Key( "Set", g_ttinclude  , conplat, list, project_name )
end

function Add_TTFileOutput( conplat, list, project_name )
	Control_Key( "Add", g_ttincludeoutput  , conplat, list, project_name )
end

function Set_TTFileOutput( conplat, list, project_name )
	Control_Key( "Set", g_ttincludeoutput  , conplat, list, project_name )
end

function Add_ImportProject( conplat, list, project_name )
	Control_Key( "Add", g_importproject  , conplat, list, project_name )
end

function Set_ImportProject( conplat, list, project_name )
	Control_Key( "Set", g_importproject  , conplat, list, project_name )
end

function Add_ImportProps( conplat, list, project_name )
	Control_Key( "Add", g_importprops  , conplat, list, project_name )
end

function Set_ImportProps( conplat, list, project_name )
	Control_Key( "Set", g_importprops  , conplat, list, project_name )
end


function Disable_GenerateProject( project_name )
	Control_Key("Set", "g_generateproject", "All", "false", project_name )
end
