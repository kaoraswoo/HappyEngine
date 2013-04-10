------------------------------------------------------------------------------------------------------------------------
-- BaseFunction.lua 
--  Functions for Base Utility
-- These functions are not used directly, But they are supportive functions to manage of files, list, variables
current_file =  "BaseFunction.lua"
------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
--  copyfile
--  file copy function from Src to Dest
--  example) success , errormsg = os.copyfile("./projectlist/AdvMaterialTool.lua" ,"../CopyTest")
--  src 경로에서 dest경로로 카피한다 ( 해당경로가 없거나 잘못되면 errormsg를 출력한다
----------------------------------------------------------------------------------------
function copyfile(src, dest)
	success , errormsg = os.copyfile(src ,dest)
	if (success == nil) then
		print(errormsg)
	end
end

----------------------------------------------------------------------------------------
--  removefile
--   remove file function from Src to Dest
--  실패하면 ErrorString을 출력한다
----------------------------------------------------------------------------------------
function removefile(src)
	local Result, ErrorString = os.remove(src)
	if not Result then
	   print(ErrorString)
	end
end

----------------------------------------------------------------------------------------
--  zero_check
--   check the list is empty or not
--  If it is empty, it returns false
----------------------------------------------------------------------------------------
function zero_check( checklist )
	if #checklist == 0 then
		return false
	else
		return true
	end
end

----------------------------------------------------------------------------------------
--  exist_check
--   check the list is empty or not
--  If it is empty, it returns false
----------------------------------------------------------------------------------------
function exist_check( checklist, keyword )
	if keyword == nil or checklist ==nil or #checklist==0 then
		return false;
	end
	for i,v in ipairs(checklist)  do
		if v==keyword then
			return true
		end
	end
	return false;
end

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
		if string.match( keyword, v ) then
			return true
		end
	end
	return false;
end

----------------------------------------------------------------------------------------
--  svpath_exist_check
--  It checks that keyword is exist in checklist's name of index
--  checklist is a double list
--  ex)  checklist = { ["ABC"]={1,2,3},   ["BCD"]={3,4,6} } 
--         keyword will be matched with "ABC" and "BCD"
--  If it is empty, it returns false
----------------------------------------------------------------------------------------
function svpath_exist_check( checklist, keyword )
	if keyword == nil then
		return false;
	end
	for replacement,patterns in pairs(checklist) do
		if replacement==keyword then
			return true
		end
	end
	return false;
end

----------------------------------------------------------------------------------------
--  svpath_index_return
--  It checks that keyword is exist in checklist's name of index
--  checklist is a double list
--  ex)  checklist = { ["ABC"]={1,2,3},   ["BCD"]={3,4,6} } 
--         keyword will be matched with "ABC" and "BCD"
--  If it is empty, it returns false
----------------------------------------------------------------------------------------
function svpath_index_return( checklist, keyword )
	if keyword == nil then
		return false;
	end
	for replacement,patterns in pairs(checklist) do
		if replacement==keyword then
			return replacement
		end
	end
	return false;
end

----------------------------------------------------------------------------------------
--  svpath_value_return
--  It checks that keyword is exist in checklist's name of index
--  checklist is a double list
--  ex)  checklist = { ["ABC"]={1,2,3},   ["BCD"]={3,4,6} } 
--         keyword will be matched with "ABC" and "BCD"
--  If it is empty, it returns false
----------------------------------------------------------------------------------------
function svpath_value_return( checklist, keyword )
	if keyword == nil then
		return false;
	end
	for replacement,patterns in pairs(checklist) do
		if replacement==keyword then
			return patterns
		end
	end
	return false;
end

----------------------------------------------------------------------------------------
--  svpath_value_match_return
--  It checks that keyword is exist in checklist's name of index
--  checklist is a double list
--  ex)  checklist = { ["ABC"]={1,2,3},   ["BCD"]={3,4,6} } 
--         keyword will be matched with "ABC" and "BCD"
--  If it is empty, it returns false
----------------------------------------------------------------------------------------
function svpath_value_match_return( checklist, keyword )
	if keyword == nil then
		return false;
	end
	for replacement,patterns in pairs(checklist) do
		if string.match(keyword, replacement) then
			return patterns
		end
	end
	return false;
end


----------------------------------------------------------------------------------------
--  Table(List) Control
----------------------------------------------------------------------------------------
function joinTables(targetlist, srclist)
    for k,v in ipairs(srclist) do
		table.insert(targetlist, v)
	end
	return targetlist
end

function Add(targetlist, srclist)
	if targetlist==nil then
		targetlist={}
    end
    for k,v in ipairs(srclist) do
		table.insert(targetlist, v)
	end
	return targetlist
end

function Add_item(targetlist, srclist)
	if targetlist==nil then
		targetlist={}
    end
	table.insert(targetlist,srclist)
	return targetlist
end

function Set_item(targetlist, srcitem)
	if targetlist==nil then
		targetlist=""
    end
	targetlist = srcitem
	return targetlist
end


----------------------------------------------------------------------------------------
--  To compare dependency, between checklist and target_project
-- It checks target_project is a dependency project in checklist project
----------------------------------------------------------------------------------------
function Check_dependency( checklist, target_project )  --Target_project가 checklist의 dependency에 있으면 true, 전혀 관게가 없으면 false
	if exist_check(g_link_list_project[checklist]["All"], target_project)==true then
		return true
	else
		return false
	end
end

----------------------------------------------------------------------------------------
--  LinkedList Object
--  ex)
--  LinkedList = CreateLinkedList()
--  LinkedList.Create()
--  LinkedList.PushFront(10)
--  LinkedList.PrintAllElements()
--  final = LinkedList.ReturnList()
----------------------------------------------------------------------------------------
---------------------------------------
--[[ Generate Instance of LinkedList (링크드 리스트 객체를 생성) ]]--
---------------------------------------
function CreateLinkedList()
 local LinkedList = {}
 local self = LinkedList;

 -------------------
 --[[ Declaration of Data 멤버 정의 --]]
 -------------------
 LinkedList.m_HeadNode = nil 
 -------------------
 --[[ Declaration of Function함수 정의 --]]
 -------------------
 LinkedList.CreateNewMode = function( InValue )
  return { Next = nil, Value = InValue };
 end
 -- Push Node Front- 맨앞에 노드 추가 
 LinkedList.PushFront = function( InValue )
  local NewFirstNode = self.CreateNewMode( InValue )
  local OldFirstNode = self.m_HeadNode.Next;
  self.m_HeadNode.Next = NewFirstNode;
  NewFirstNode.Next = OldFirstNode;
 end
 -- Push Node Back 맨끝에 노드 추가
 LinkedList.PushBack = function( InValue )
  if self.m_HeadNode.Next == nil then
	local NewFirstNode = self.CreateNewMode( InValue )
	local OldFirstNode = self.m_HeadNode.Next;
	self.m_HeadNode.Next = NewFirstNode;
	NewFirstNode.Next = OldFirstNode;
  else                                       
   local CurrentNode = self.m_HeadNode.Next
   while CurrentNode.Next do
     CurrentNode = CurrentNode.Next;
   end
   CurrentNode.Next = self.CreateNewMode( InValue )
  end
 end
 -- 모든 노드를 출력
  LinkedList.PrintAllElements = function()
  local CurrentNode = self.m_HeadNode.Next
  while CurrentNode do
   print( CurrentNode.Value )
      CurrentNode = CurrentNode.Next;
  end
 end
 -- 객체 초기화
 LinkedList.Create = function()
        self.m_HeadNode = self.CreateNewMode(0);
 end
 
 --리스트 리턴
 LinkedList.ReturnList = function()
 local finallist = {}
 local CurrentNode = self.m_HeadNode.Next
 while CurrentNode do
    table.insert(finallist, CurrentNode.Value)
      CurrentNode = CurrentNode.Next;
  end
 return finallist
 end
 
 -- Search Object Node 오브젝트 찾기
 LinkedList.ReturnNode = function( nodename )
  local CurrentNode = self.m_HeadNode.Next
  local PreviewNode = self.m_HeadNode
  while CurrentNode do
     if CurrentNode.Value == nodename then
        return CurrentNode, PreviewNode
     end
      PreviewNode = CurrentNode
      CurrentNode = CurrentNode.Next;
  end
    return false, false
 end
 
  -- 오브젝트 삽입 - Target 노드 뒤에
 LinkedList.InsertNode = function( TargetValue, InValue )
  local CurrentNode = self.m_HeadNode.Next
  while CurrentNode do
     if CurrentNode.Value == TargetValue then
        local NewNode = self.CreateNewMode( InValue )
		NewNode.Next = CurrentNode.Next
		CurrentNode.Next =  NewNode
		return true
     end
      CurrentNode = CurrentNode.Next;
  end
    return false
 end
 
   -- 오브젝트 이동 - Target 노드 뒤에
 LinkedList.MoveNode = function( TargetValue, InValue )
  local CurrentNode = self.m_HeadNode.Next
  while CurrentNode do
     if CurrentNode.Value == TargetValue then   --CurrentNode를 TargetValue까지 이동시키고,
        local NewNode, PreNode = self.ReturnNode( InValue )  --Invalue의 노드를 찾아서 리턴한다 그 노드를 NewNode라 하고, 해당 newNode의 Prev 를 PreNode에 넣는다
        if PreNode ~= nil then  -- PreNode 가 nil이 아니라면 (Nil이 없을수밖에 없는 것이 Head Node가 항상 앞에 있기에 가장 처음도 Head를 리턴할 것)
            PreNode.Next = NewNode.Next
        end
		NewNode.Next = CurrentNode.Next
		CurrentNode.Next =  NewNode
		return true
     end
      CurrentNode = CurrentNode.Next;
  end
    return false
 end
 
 -- Sort by Object Dependencies - 오브젝트 디펜던시 정렬
  LinkedList.DepSort = function()
	local standlist = self.ReturnList()  -- 현재 리스트에 있는 값을 standlist에 넣는다
	local CurrentNode
	for j=1, 5 do  -- 반복할 수록 정확도가 높아진다(현재는 임시로 5개)
		for i, CurrentNode in ipairs(standlist) do    --standlist 에서 값 읽어서 반복하면서
			local last_stand = false
			local CurrentNode2 = self.m_HeadNode.Next

			while CurrentNode2 do   --링크트리스트에서 다시 반복하면서
				if Check_dependency( CurrentNode,  CurrentNode2.Value ) == true then  --CurrentNode2.Value가 CurrentNode의 Dependency리스트에 존재한다면
					last_stand = CurrentNode2.Value  --가장 마지막 값을 찾는다
				end
				CurrentNode2 = CurrentNode2.Next;
			end
					
			if last_stand ~= false then   --마지막 값이 존재한다면
				self.MoveNode ( last_stand, CurrentNode  )   --해당 last_stand 뒤에 CurrentNode를 넣는다
			end
		end
	end
  end
  
  return LinkedList;
 end
---------------------------------------
--[[ End of Linked List Object ]]--
---------------------------------------




----------------------------------------------------------------------------------------
--  get_recursive_path_files
--  it returns list of file's name in m_paths and its child folder.
--  At first, it use "lua file system" and search all files in directory including child folder
--  But, Now, It has problem in Premake4... --  특정 횟수 이상을 호출하면 불규칙하게 premake 에러가 반환됨, 
--  So, I change the merhod to use "os.matchfiles" instead of "lfs"
-- 왜 exclude_keyword를 넣었냐면, 만약 특정폴더 아래를 찾을때, PS3폴더안의 파일들  제외한 것을 찾고 싶을때
-- exclude_keyword에 PS3 를 넣으면 된다. 그렇다고 PS3/childfolder의 파일들도 제외되지는 않는다.
----------------------------------------------------------------------------------------
function get_recursive_path_files(m_paths, exclude_keyword)
		return_matchesfiles = {}
		matchesdirs = os.matchdirs(m_paths.."/**")  --os.matchdirs only can search in m_paths' child folder
		table.insert(matchesdirs, m_paths)  --So, insert m_paths own path
		for i,v in ipairs(matchesdirs)  do
			if exclude_keyword ~=nil then  --if there is exclude_keyword
				if  ( v ~= m_paths.."/"..exclude_keyword)  then   --exclude_keyword와 다른 폴더야만 검색한다
					matchesfiles = os.matchfiles(v.."/*.*")
					for j,w in ipairs(matchesfiles)  do
					table.insert(return_matchesfiles , w)
					end
				end
			else
				matchesfiles = os.matchfiles(v.."/*.*")
				for j,w in ipairs(matchesfiles)  do
				table.insert(return_matchesfiles , w)
				end
			end
		end
		return return_matchesfiles
end

----------------------------------------------------------------------------------------
--  get_recursive_path_files_with_convert
--  it returns list of file's name in m_paths and its child folder.
--  Almost same as get_recursive_path_files(). But, it contains some string process to change "-" to "\-"
--  Premake can not recognize "-" itself.. That's why It needs to convert "-" to "\-"
--  ex) filelist = get_recursive_path_files2("E:/System", "PS3" )
--      filelist has a list of file in E:\System excluding "PS3" folder
----------------------------------------------------------------------------------------
function get_recursive_path_files2(m_paths, exclude_keyword)
		return_matchesfiles = {}
		matchesdirs = os.matchdirs(m_paths.."/**")  --여기서는 자기 아래의 자식폴더만 찾기에
		table.insert(matchesdirs, m_paths)  --자기 자신폴더도 insert시켜준다
		for i,v in ipairs(matchesdirs)  do
			 comparestring = string.gsub(v, "\\", "/")
				if  string.find( comparestring, "/"..exclude_keyword)   then   --exclude_keyword와 같은 폴더야만 검색한다
					matchesfiles = os.matchfiles(v.."/*.*")
					for j,w in ipairs(matchesfiles)  do
						w= string.gsub(w, "-", "\-")
						table.insert(return_matchesfiles , w)
					end
				end
		end
		return return_matchesfiles
end

----------------------------------------------------------------------------------------
--  get_recursive_path_files2
--  it returns list of file's name in m_paths and its child folder.
--  Almost same as get_recursive_path_files(). But, it contains some string process to change "-" to "\-"
--  Premake can not recognize "-" itself.. That's why It needs to convert "-" to "\-"
--  ex) filelist = get_recursive_path_files2("E:/System", "PS3" )
--      filelist has a list of file in E:\System excluding "PS3" folder
----------------------------------------------------------------------------------------
function get_non_recursive_path_files(m_paths)
	m_matches=os.matchfiles(m_paths.."/*.*")
	return m_matches
end

function is_identity(match_list, identity_word)
		for i,v in ipairs(match_list)  do
			if  v == identity_word then 
			return true
			end
		end
		return false
end
--------------------------------------------------

----------------------------------------------------------------------------------------
--  get_file_name_list
--  It searchs all files in one directory.
--  현재 위의 get_recursive_path_files 함수가 해결될때까지 임시로 쓸 함수(현재 해결되어서 많이 쓰이지는 않을것으로 예상.)
----------------------------------------------------------------------------------------
function get_file_name_list( folderpath , parentfolder )
finalmatches={}
	for i,v in ipairs(matches)  do
		table.insert(finalmatches, parentfolder.."/"..path.getname(v))
	end
return finalmatches
end




----------------------------------------------------------------------------------------
--  Make GUID
--  Instead of Random os.uuid(), it creates fixed GUID by name string.
--  So, If name string is same, GUID is same.
----------------------------------------------------------------------------------------
function HexDumpString(str,spacer)
	return (
	string.gsub(str,"(.)",
		function (c)
			return string.format("%02X%s",string.byte(c), spacer or "")
		end)
	)
end
	
function string.insert(value, insert, place)
 return   string.sub(value, 1,place-1)..tostring(insert)..string.sub(value, place, string.len(value))
end
	 
function MakeGUIDfromSeed( nameSeed )
  HexaValue = HexDumpString(nameSeed)
  length = tonumber( string.format( "%d", #HexaValue ) )
  finalvalue=""
  for i=1,32 do
   index = tonumber( string.format( "%d",(i*length/32)+1 ) )
   if index>length then
	index=length
   end
   finalvalue = finalvalue..string.sub(HexaValue, index,index)
  end
  finalvalue = string.insert(finalvalue, "-", 9)
  finalvalue = string.insert(finalvalue, "-", 14)
  finalvalue = string.insert(finalvalue, "-", 19)
  finalvalue = string.insert(finalvalue, "-", 24)
  return finalvalue
end