------------------------------------------------------------------------------------------------------------------------
--Project_Control.lua 
-- Project level control Function
-- made by hjh84
current_file =  "Project_Control.lua"
------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
--  [ Management functions for Platform & Configuration ]
----------------------------------------------------------------------------------------

function Add_Project( project_name , project_type, project_folder, project_lang )
	
	g_final_project_folder = g_solution_directory.."/"..project_folder    --make global variable for flushproject() function
	Declare_Key( project_name )
	AddTable( g_projectname_list, project_name )  --g_projectname_list is ArrayList
	Add_Key( "g_project_type", "All", project_type )
	Add_Key( "g_project_folder", "All", g_final_project_folder )
	if project_lang==nil then
		project_lang="C++"
	end
	Add_Key( "g_project_lang", "All", project_lang )

end


function EngineLibrary( project_name, platform, project_folder )
	g_project_name = project_name
	if project_folder == nil then
		project_folder = project_name
	end
	Add_Project(g_project_name, "StaticLib", project_folder )
		Set_SourceFile("All", {g_final_project_folder.."/**.cpp", g_final_project_folder.."/**.h",
										   g_final_project_folder.."/**.ico", g_final_project_folder.."/**.rc",
										   g_final_project_folder.."/**.nsi", g_final_project_folder.."/**.fp",
										   g_final_project_folder.."/**.vp", g_final_project_folder.."/**.fx"}
								)
		Set_Target_Path( "Win32" , g_final_project_folder.."\\..\\Lib\\" )
		Set_Target_Name_Path( "Win32" ,  "$(ProjectName)_$(Configuration)" )
		Set_Obj_Path( "Win32" , g_final_project_folder.."\\..\\obj\\$(ProjectName)_$(Configuration)\\" )
		
		Add_Flags( "Debug|Win32" , { "Symbols", "NoMinimalRebuild"  })
		Add_Flags( "Release|Win32" , {"Optimize" })
		
		-- VirtualPath
		Add_VirtualPath( "EngineLibrary", g_project_name )
		
end



function EmptyProject( project_name, platform, project_folder )
	g_project_name = project_name
	if project_folder == nil then
		project_folder = project_name
	end
	Add_Project(g_project_name, "StaticLib", project_folder )
		project_uuid = MakeGUIDfromSeed(g_project_name)
		Set_Uuid( "All", project_uuid )
		
		-- VirtualPath
		Add_VirtualPath( "EmptyProject", g_project_name )
end



----------------------------------------------------------------------------------------
--  C# projects
----------------------------------------------------------------------------------------	
function CSharp_Application( project_name, project_folder)
	g_project_name = project_name
	if project_folder == nil then
		project_folder=project_name
	end
	
	Add_Project(g_project_name, "WindowedApp", project_folder, "C#" )
		-- Set GUID
		project_uuid = MakeGUIDfromSeed(g_project_name)
		Set_Uuid( "All", project_uuid )
		
		Set_SourceFile("All", {g_final_project_folder.."/**.cs", g_final_project_folder.."/**.xaml",
										   g_final_project_folder.."/**.resx", g_final_project_folder.."/**.config"}
								)
		Add_Exclude_SourceFile( "All", {"/**.g.cs","/**.g.i.cs"} )
		
		-- VirtualPath :
		Add_VirtualPath( "CSharp_Application", g_project_name )

end
