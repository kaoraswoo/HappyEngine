------------------------------------------------------------------------------------------------------------------------
--FolderMacro.lua 
-- We can define a variable which contains folder path  and User-Macro
-- When we need a common folderpath in Project_.lua file, we can use it
------------------------------------------------------------------------------------------------------------------------

prototype = ""  --If it is not Prototype, it must be ""
prefixproject = ""

----------------------------------------------------------------------------------------
--  match_check
--   check the list is empty or not
--  If it is empty, it returns false
----------------------------------------------------------------------------------------
function match_check( checklist, keyword )
	if keyword == nil or checklist ==nil or #checklist==0 then
		return false;
	end
	for i,v in ipairs(checklist)  do
		if string.match(v, keyword) then
			return true
		end
	end
	return false;
end