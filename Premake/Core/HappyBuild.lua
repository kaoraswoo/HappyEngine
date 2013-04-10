------------------------------------------------------------------------------------------------------------------------
-- HappyBuild.lua 
--  made by hjh84
current_file =  "HappyBuild.lua"
------------------------------------------------------------------------------------------------------------------------

dofile("BaseFunction.lua")
dofile("Define_Variables.lua")  --Declararing Variable && Clearing Variable


	
----------------------------------------------------------------------------------------
--  project_base_create
--  It creates project file
--  language_type ( C, C++, or C# )
--  target_type (ConsoleApp,WindowedApp,SharedLib,StaticLib)
----------------------------------------------------------------------------------------
function project_base_create(project_name, language_type, target_type, project_folder)
	project(prefixproject..project_name)
	language (language_type)
	kind(target_type)
	location(project_folder)
	
	--Generate Project
	if g_generateproject[project_name] ~=nil then --If there is no project g_importproject setting, It would be set.
	   -- don't generate project file
		generateproject ( g_generateproject[project_name] )
	else
		-- generate projectfile
	end
end

----------------------------------------------------------------------------------------
--  set_pch
--  If pch_folder is not exist, it follows default setting ( project_relative.."/Impl/StdAfx.h" )
--  pch_folder 파라메터값 생략하면 기본세팅을 따른다( project_relative.."/Impl/StdAfx.h" )
----------------------------------------------------------------------------------------
function set_pch(pch_folder)
	finalfilename = "StdAfx"
	if pch_folder == nil or pch_folder=="" then
		project_relative=path.getrelative(g_solution_directory.."/..", g_final_project_folder)
		filematches = os.matchfiles(g_final_project_folder.."/Impl/**.h") 
		for i,filename in pairs(filematches) do
			if string.match( string.lower(filename), "stdafx.h" ) then
				finalfilename = path.getbasename(path.getname(filename))
				pchheader (project_relative.."/Impl/"..finalfilename..".h")
			end
		end
	else
		pchheader ( pch_folder)
		filematches = os.matchfiles( g_final_project_folder.."/"..path.getdirectory( pch_folder ).."/**.h" )
		for i,filename in pairs(filematches) do
			if string.match( string.lower(filename), "stdafx.h" ) then
				finalfilename = path.getbasename(path.getname(filename))
			end
		end
	end
	fileprecompile { finalfilename..".cpp" }
end


----------------------------------------------------------------------------------------
--  SyncPlatform
--  Originally, premake use "x32", "ps3" keyword, But We use "Win32", "PS3".
--  x32, ps3로 표시되는 premake 플랫폼에 대해서, 최종 플랫폼값 return
----------------------------------------------------------------------------------------
function SyncPlatform(platform)
	local changedplatform
	if platform=="x32" then
		changedplatform="Win32"
	elseif platform=="ps3" then
		changedplatform="PS3"
	else
		changedplatform=platform
	end
	return changedplatform
end

----------------------------------------------------------------------------------------
--  create_project( project_name , project_type, project_folder, Common_file)
--  project_name : 프로젝트명   /   project_type : 타입 (ConsoleApp,WindowedApp,SharedLib,StaticLib)   /   project_folder : 솔루션위치에서의 상대경로이다.   / Common_file: 세팅을 따를 commonfile
----------------------------------------------------------------------------------------
function create_project( project_name , project_type, project_folder, project_lang )
	if project_folder==nil or project_folder=="" then  --만약 프로젝트 폴더를 지정 안하면, 프로젝트명 규칙에 따라서 폴더 탐색(solution명/project명)
		project_folder = "../".. get_naming_folder(project_name)
	end
	
	Do_CommonFile()
	

	g_project_name = project_name  --make global variable for flushproject() function
	--g_final_project_folder = g_solution_directory.."/"..project_folder    --make global variable for flushproject() function
	g_final_project_folder = project_folder    --make global variable for flushproject() function
	
	--Create project
	project_base_create (project_name, project_lang, project_type, g_final_project_folder) 


	
end


----------------------------------------------------------------------------------------
--  flushproject
--  It sets all the configuration in platform&configures combination.
----------------------------------------------------------------------------------------
function flushproject(l_project_name)
	g_project_name = l_project_name
	if exist_check(g_projectname_list, l_project_name)==false then
		return
	end
	
	print("Flushing : "..l_project_name)
		
	create_project( l_project_name,  g_project_type[l_project_name],  g_project_folder[l_project_name], g_project_lang[l_project_name] )
	
	--pch header set
	if g_pchcheck[l_project_name]~="N" then
		if g_pchcheck[l_project_name]=="Y" then
			set_pch()
		else
			set_pch(g_pchcheck[l_project_name])
		end
	end
	
	
	-- Add "" <--empty for "All" Platform|Configuration
	if(g_platforms[1]~= "") then
		table.insert(g_platforms,1,"")  --  Add entire platforms  ""<--empty
		end
		if(g_configurations[1]~="") then
		table.insert(g_configurations,1,"")  --  Add entire configuration  ""<--empty
	end
	
	-- Loop all Platforms and all Configuration you defined in Platform.lua
	for i=1, #g_platforms do
		for j=1, #g_configurations do
		
			
			-------------------
			--[[ Making "final_index" for loop --]]
			--(이미 정의되고 값이 넣어진 전역변수들(g_)의 접근을 위해서 final_index를 다음과 같은 규칙에 맞게 만든다) 
			-------------------
			--If platform and configuration are exist (ex : Debug & PS3 ), It becomes "Debug|PS3"
			--둘다 데이터가 있을시에는 "Conf | Platform"  형식으로 출력하고
			if( g_platforms[i] ~="" and g_configurations[j] ~="") then
				final_index=g_configurations[j].."|"..SyncPlatform(g_platforms[i])
				originalindex=g_configurations[j].."|"..g_platforms[i]
			else 
			--If there is only platform or configuration ( ex " PS3" ), It becomes just "PS3" without "|"
			--하나라도 없으면 둘중 하나만 출력이 된다.
				final_index=g_configurations[j]..SyncPlatform(g_platforms[i])
				originalindex=g_configurations[j]..g_platforms[i]
			end
			
			--if final_index is ""<--empty, it does not need to declare "configuration{}"
			if final_index~="" then  --배열index가 "" 일때는 configuration 을 할 필요가 없다
				if( g_platforms[i] ~="" and g_configurations[j] ~="") then
					configuration { g_platforms[i] , g_configurations[j]}
				else
					configuration (originalindex)
				end
			elseif final_index =="" then
				final_index= "All"  -- ""<--empry index becomes "All"
			end
			-------------------
			--[[ End of Making "final_index" for loop --]]
			-------------------
		
			--defines
			if g_defines[l_project_name][final_index] ~=nil and #g_defines[l_project_name][final_index]>0 then
				defines {g_defines[l_project_name][final_index]}
			end
			
			--flags
			if g_flags[l_project_name][final_index] ~=nil and #g_flags[l_project_name][final_index]>0 then
				flags {g_flags[l_project_name][final_index]}
			end
	
			--filespec
			if g_file_list[l_project_name][final_index] ~=nil and #g_file_list[l_project_name][final_index]>0 then
				files {g_file_list[l_project_name][final_index]}
			end
			
			--excludeFile
			if g_exclude_list[l_project_name][final_index] ~=nil and #g_exclude_list[l_project_name][final_index]>0 then
				excludes {g_exclude_list[l_project_name][final_index]}
			end
			
			--excludeBuildfolder
			if g_exclude_build_folder_keyword[l_project_name][final_index] ~=nil and #g_exclude_build_folder_keyword[l_project_name][final_index]>0 then
				for i,v in ipairs(g_exclude_build_folder_keyword[l_project_name][final_index]) do 
					finalmatches = get_recursive_path_files2(g_final_project_folder , v)
					buildexcludes {finalmatches}
				end
			end
			
			--excludeBuildfile
			if g_exclude_build_file_keyword[l_project_name][final_index] ~=nil and #g_exclude_build_file_keyword[l_project_name][final_index]>0 then
					buildexcludes {g_exclude_build_file_keyword[l_project_name][final_index]}
			end
			
			--includedirectory
			if g_include_dir[l_project_name][final_index] ~=nil and #g_include_dir[l_project_name][final_index]>0 then
				includedirs {g_include_dir[l_project_name][final_index]}
			end
			
			--targetdir
			if g_target_path[l_project_name][final_index] ~=nil and #g_target_path[l_project_name][final_index]>0 then
				targetdir (g_target_path[l_project_name][final_index])
			end
			
			--targetname
			if g_targetname_path[l_project_name][final_index] ~=nil and #g_targetname_path[l_project_name][final_index]>0 then
				targetname (g_targetname_path[l_project_name][final_index])
			end
			
			--obj - Intermediate Directory
			if g_obj_path[l_project_name][final_index] ~=nil and #g_obj_path[l_project_name][final_index]>0 then
				objdir (g_obj_path[l_project_name][final_index])
			end
			
			--link
			if g_link_list[l_project_name][final_index] ~=nil and #g_link_list[l_project_name][final_index]>0 then
				links {g_link_list[l_project_name][final_index]}
			end
			
			--link_project
			if g_link_list_project[l_project_name][final_index] ~=nil and #g_link_list_project[l_project_name][final_index]>0 then
				temp_prefix_project_list={}
				for i,v in ipairs(g_link_list_project[l_project_name][final_index]) do   --프로젝트명 앞에 prefixproject 를 붙인다.
					temp_prefix_project_list[i]= prefixproject..v
				end
				links {temp_prefix_project_list}
			end
			
			--library_directory
			if g_libdir_list[l_project_name][final_index] ~=nil and #g_libdir_list[l_project_name][final_index]>0 then
				libdirs {g_libdir_list[l_project_name][final_index]}
			end
			
			--ignore specific library
			if g_ignorelib[l_project_name][final_index] ~=nil and #g_ignorelib[l_project_name][final_index]>0 then
				ignorelib {g_ignorelib[l_project_name][final_index]}
			end
			
			--buildoption
			if g_buildoption[l_project_name][final_index] ~=nil and #g_buildoption[l_project_name][final_index]>0 then
				buildoptions {g_buildoption[l_project_name][final_index]}
			end
			
			--linkoptions
			if g_linkoption[l_project_name][final_index] ~=nil and #g_linkoption[l_project_name][final_index]>0 then
				linkoptions {g_linkoption[l_project_name][final_index]}
			end
			
			--moduleDefinition File
			if g_moduledef[l_project_name][final_index] ~=nil and #g_moduledef[l_project_name][final_index]>0 then
				moduledef {g_moduledef[l_project_name][final_index]}
			end
			
			--prebuildcommands
			if g_prebuildcommands[l_project_name][final_index] ~=nil and #g_prebuildcommands[l_project_name][final_index]>0 then
				prebuildcommands {g_prebuildcommands[l_project_name][final_index]}
			end
			
			--postbuildcommands
			if g_postbuildcommands[l_project_name][final_index] ~=nil and #g_postbuildcommands[l_project_name][final_index]>0 then
				postbuildcommands {g_postbuildcommands[l_project_name][final_index]}
			end
			
			--custombuildcommands
			if g_custombuildcommands[l_project_name][final_index] ~=nil and #g_custombuildcommands[l_project_name][final_index]>0 then
				custombuildcommands {g_custombuildcommands[l_project_name][final_index]}
			end
			
			--custombuildoutputs
			if g_custombuildoutputs[l_project_name][final_index] ~=nil and #g_custombuildoutputs[l_project_name][final_index]>0 then
				custombuildoutputs {g_custombuildoutputs[l_project_name][final_index]}
			end
			
			--projectset per project
			if g_projectset[l_project_name][final_index] ~=nil then
				projectset( g_projectset[l_project_name][final_index] )
			end
			
			--buildset
			if g_buildset[l_project_name][final_index] ~=nil and #g_buildset[l_project_name][final_index]>0 then
				projectset ( g_buildset[l_project_name][final_index] )
			end
			
			--projectbuildcheck in Common level
			if g_projectbuildcheck[l_project_name][final_index] ~=nil and #g_projectbuildcheck[l_project_name][final_index]>0  then --If there is no project g_projectbuildcheck setting, It would be set.
				projectbuildcheck { g_projectbuildcheck[l_project_name][final_index] }
			end
			
			--forceincludedirs for PS3
			if g_forceinclude_dir[l_project_name][final_index] ~=nil and #g_forceinclude_dir[l_project_name][final_index]>0 then
				forceincludedirs { g_forceinclude_dir[l_project_name][final_index] }
			end
			
			--fixed uuid per project
			if g_uuid[l_project_name][final_index] ~=nil and #g_uuid[l_project_name][final_index]>0  then --If there is no project g_uuid setting, It would be set.
				uuid ( g_uuid[l_project_name][final_index] )
			end
			
			--TT file include
			if g_ttinclude[l_project_name][final_index] ~=nil and #g_ttinclude[l_project_name][final_index]>0  then --If there is no project g_ttinclude setting, It would be set.
				ttinclude { g_ttinclude[l_project_name][final_index] }
			end
			
			--TT file Output include
			if g_ttincludeoutput[l_project_name][final_index] ~=nil and #g_ttincludeoutput[l_project_name][final_index]>0  then --If there is no project g_ttincludeoutput setting, It would be set.
				ttincludeoutput { g_ttincludeoutput[l_project_name][final_index] }
			end
			
			--Import Project
			if g_importproject[l_project_name][final_index] ~=nil and #g_importproject[l_project_name][final_index]>0  then --If there is no project g_importproject setting, It would be set.
				importproject { g_importproject[l_project_name][final_index] }
			end
			
			--Import Props
			if g_importprops[l_project_name][final_index] ~=nil and #g_importprops[l_project_name][final_index]>0  then --If there is no project g_importprops setting, It would be set.
				importprops { g_importprops[l_project_name][final_index] }
			end
			
		end
	end
	
	
end

