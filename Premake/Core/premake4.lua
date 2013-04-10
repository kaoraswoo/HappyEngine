------------------------------------------------------------------------------------------------------------------------
-- premake4.lua 
-- First File that Premake.exe refers
current_file =  "premake4.lua"
------------------------------------------------------------------------------------------------------------------------
newoption { trigger = "outpath", description = "Location for generated project files" }
newoption { trigger = "solutionname", description = "solution name" }
newoption { trigger = "solutionpath", description = "solution path" }
newoption { trigger = "pause", description = "pause when Solution is generated" }
newoption { trigger = "onlysolution", description = "turn off projectfile Generating Flag" }

	if not _OPTIONS["solutionname"] then
		error("You must specify the 'solutionname' parameter")
	end
	if not _OPTIONS["outpath"] then
		error("You must specify the 'outpath' parameter")
	end
	
	 
g_pause = _OPTIONS["pause"]
g_solution_name =  _OPTIONS["solutionname"]
script_directory = _OPTIONS["outpath"]
solutionpath = _OPTIONS["solutionpath"]
g_solution_directory = string.gsub(script_directory.."/..", "\\", "/").."/"..solutionpath
premake_directory = path.getdirectory(_SCRIPT )


print("* Premake_Directory : "..path.getdirectory(_SCRIPT ))
print("* Solution_Script_Directory : "..script_directory)
print("* Solution_Directory : "..g_solution_directory)


-- Path Setting
dofile(script_directory.."/Core/Foldermacro.lua")

-- Get Platform and Configuration
dofile(script_directory.."/Platform.lua")

--User specified Platform and Configuration 
	print("-- 플랫폼을 선택해주세요 / 번호입력후 엔터--"  ) 
	print("-- 전체플랫폼 생성은 그냥 엔터를 눌러주세요 --")
	local printplatform=""
	local platformcnt=1
	for i,v in ipairs( g_platforms ) do
		printplatform = printplatform..i..". "
		printplatform = printplatform..v.." only / "
		platformcnt= platformcnt+1
	end
	print(printplatform.." "..platformcnt..". All platform :  Input  [Enter]")
	if g_pause == "no" then
		--skip
	else
		plchoose = io.read()
		--	os.execute("pause")
		if plchoose== "1" then
			print(g_platforms[1].." only")
			 g_platforms= { g_platforms[1] }
		elseif plchoose=="2" then
			print(g_platforms[2].." only")
			 g_platforms= { g_platforms[2] }
		elseif plchoose=="3" then
			print(g_platforms[3].." only")
			g_platforms= { g_platforms[3] }
		else
			print("All platform")
		end
	end


-- Load Functions
dofile(premake_directory.."/HappyBuild.lua")
dofile(premake_directory.."/Project_Control.lua")
dofile(premake_directory.."/Solution_Control.lua")

local generatingflag = _OPTIONS["onlysolution"]
if generatingflag == "yes" then
	print("Turn Off Project File generating Flag")
	OffProjectGeneration()
end

dofile(script_directory.."/".. _OPTIONS["solutionname"]..".lua")	

-- Get Solution Setting
solution (_OPTIONS["solutionname"])
		location(g_solution_directory)
		configurations {g_configurations}
		platforms {g_platforms}
		

-- Flush ( Create projects by Premake )
dofile(premake_directory.."/Flush_All.lua")


