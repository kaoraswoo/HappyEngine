------------------------------------------------------------------------------------------------------------------------
-- Flush_All.lua 
-- Make Projects by global variables in which we already put project setting into
-- Match project-list from Solution and Projects
-- Ordering of project by Project dependencies
current_file =  "Flush_All.lua"
------------------------------------------------------------------------------------------------------------------------
	
	local var_project_list={}
	
----------------------------------------------------------------------------------------
-- Extract List from project that is exist in g_projectgroup( g_projectgroup is made in Solution Level function AddProjectFolderGroup() )
-- 프로젝트레벨에서 설정한 g_svpathslist 를, 솔루션레벨에서 정한 g_projectgroup에 포함되는 것만 g_final_svpathslist로  옮긴다
----------------------------------------------------------------------------------------		
	--for replacement,patterns in pairs(g_svpathslist) do
	--	if exist_check( g_projectgroup, replacement )==true then
	--		g_final_svpathslist[replacement] = AddTable( g_final_svpathslist[replacement], g_svpathslist[replacement] )
	--	end
	--end
	for replacement,patterns in pairs(g_svpathslist) do
		local pathkeyword = svpath_value_match_return( g_projectgroup, replacement ) 
		if not (pathkeyword==false) then
			g_final_svpathslist[pathkeyword] = AddTable( g_final_svpathslist[pathkeyword], g_svpathslist[replacement] )
		end
	end
	
----------------------------------------------------------------------------------------
-- Solution_Control.lua에서 넣어주었던 g_generateprojectgroup과 프로젝트레벨의 g_svpathslist의 key와 매치하는게 있으면
-- 해당 프로젝트리스트들을 Disable_GenerateProject에 리스트 넣어준다
----------------------------------------------------------------------------------------			
	for replacement,patterns in pairs(g_svpathslist) do
		if match_check( g_generateprojectgroup, replacement) then
			for i,v in pairs( g_svpathslist[replacement] ) do
				Disable_GenerateProject( v )
			end
		end
	end

	
----------------------------------------------------------------------------------------
-- Set Virtual Path from g_final_svpathslist
----------------------------------------------------------------------------------------					
	svpaths (g_final_svpathslist)  --svpaths is a Premake customizing keyword to define svpathslist
	
	
	
----------------------------------------------------------------------------------------
-- Get "var_project_list" from g_final_Svpathslist
-- g_final_svpathslist 에서 최종 포함될 프로젝트 리스트를 추출해서 var_project_list 에 넣는다.
----------------------------------------------------------------------------------------					
		for replacement,patterns in pairs(g_final_svpathslist) do
			for _,pattern in ipairs(patterns) do
				if svpath_exist_check(g_final_svpathslist,pattern)==false then --exception of virtual folders, insert project name to var_project_list
					table.insert( var_project_list, pattern)
				end
			end
		end
	
----------------------------------------------------------------------------------------
-- Ordering Process by dependency
-- var_project_list를 선택정렬로, dependency에 맞춰서 정렬시킨다
----------------------------------------------------------------------------------------					
	LinkedList = CreateLinkedList()
	LinkedList.Create()
	print("Insert ProjectList to LinkedList for ordering by project dependency")
		for i,v in ipairs(var_project_list) do
			print(v)
			LinkedList.PushBack(v)
		end
	LinkedList.DepSort()  --Sort by Project dependency

	finalprojectlist = LinkedList.ReturnList()

	
----------------------------------------------------------------------------------------
-- Flush
--  위에서 정렬시킨 finalprojectlist 를 전부 flush
----------------------------------------------------------------------------------------						
	for i,v in ipairs(finalprojectlist) do
		flushproject(v)
	end