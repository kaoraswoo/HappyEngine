------------------------------------------------------------------------------------------------------------------------
--Solution_Control.lua 
-- Solution level control Function
-- made by hjh84
current_file =  "Solution_Control.lua"
------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
--  match_keyword
--  It searches "Projects.lua" file on the rule of keyword
--  ex:) Projects.lua
--         Engine_Library("Dragoon")
--  It gets string: "Dragoon"  from the line in Projects.lua and put it into match_list[]
----------------------------------------------------------------------------------------

g_flagGenerationProject = true

function OnProjectGeneration()
	g_flagGenerationProject = true
end

function OffProjectGeneration()
	g_flagGenerationProject = false
end


----------------------------------------------------------------------------------------
-- AddProjects
----------------------------------------------------------------------------------------	
function AddProjects( projectlist , folder_name)
    if (type(projectlist) ~="table") then
        print("AddProjects() "..projectlist.." : It is not table list, You have to write { \"projectname1\", \"projectname2\", .... }")
        return
    end
	if (folder_name == nil) then
		folder_name=""
	end
	g_final_svpathslist[folder_name] = AddTable( g_final_svpathslist[folder_name], projectlist )
	
	if g_flagGenerationProject == false then
		for i,projectname in pairs( projectlist ) do
			Disable_GenerateProject( projectname )
		end
	end
	
end


----------------------------------------------------------------------------------------
-- SetSolutionDirectory
----------------------------------------------------------------------------------------	
function SetSolutionDirectory( foldername )
	g_solution_directory = script_directory.."/../"..foldername
end

----------------------------------------------------------------------------------------
-- LoadProjectList
----------------------------------------------------------------------------------------	
function LoadProjectList( filename)
	dofile( filename )
end