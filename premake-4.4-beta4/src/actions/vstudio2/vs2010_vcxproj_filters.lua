--
-- vs2010_vcxproj_filters.lua
-- Generate a Visual Studio 2010 C/C++ filters file.
-- Copyright (c) 2009-2011 Jason Perkins and the Premake project
--

	local vc2010 = premake.vstudio.vc2010
	local project = premake.project
	

--
-- The first portion of the filters file assigns unique IDs to each
-- directory or virtual group. Would be cool if we could automatically
-- map vpaths like "**.h" to an <Extensions>h</Extensions> element.
--

	---------------------------------------------------------------------------
	-- Blueside Customizing
	-- make GUID from Seed string
	-- http://mediawiki.blueside.co.kr/index.php5/Premake-structure-createGUIDfromString
	----------------------------------------------------------------------------
	function HexDumpString(str,spacer)
		return (
		string.gsub(str,"(.)",
			function (c)
				return string.format("%02X%s",string.byte(c), spacer or "")
			end)
		)
	end
	
	function string.insert(value, insert, place)
	 return   string.sub(value, 1,place-1) .. tostring(insert) .. string.sub(value, place, string.len(value))
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
	---------------------------------------------------------------------------
	--/End of Blueside Customization
	----------------------------------------------------------------------------
	
	function vc2010.filteridgroup(prj)
		local filters = { }
		local filterfound = false

		for file in project.eachfile(prj) do
			-- split the path into its component parts
			local folders = string.explode(file.vpath, "/", true)
			
			
			local path = ""
			for i = 1, #folders - 1 do
				-- element is only written if there *are* filters
				if not filterfound then
					filterfound = true
					_p(1,'<ItemGroup>')
				end
				
				path = path .. folders[i]
				

				-- have I seen this path before?
				if not filters[path] then
					filters[path] = true
					_p(2, '<Filter Include="%s">', path)
					---------------------------------------------------------------------------
					-- Blueside Customizing
					-- make GUID from Seed string
					-- http://mediawiki.blueside.co.kr/index.php5/Premake-structure-createGUIDfromString
					----------------------------------------------------------------------------
					--_p(3, '<UniqueIdentifier>{%s}</UniqueIdentifier>', os.uuid())
					_p(3, '<UniqueIdentifier>{%s}</UniqueIdentifier>', MakeGUIDfromSeed(path))
					---------------------------------------------------------------------------
					--/End of Blueside Customization
					----------------------------------------------------------------------------
					
					_p(2, '</Filter>')
				end

				-- prepare for the next subfolder
				path = path .. "\\"
			end
		end
		
		if filterfound then
			_p(1,'</ItemGroup>')
		end
	end


--
-- The second portion of the filters file assigns filters to each source
-- code file, as needed. Section is one of "ClCompile", "ClInclude", 
-- "ResourceCompile", or "None".
--

	function vc2010.filefiltergroup(prj, section)
		local files = vc2010.getfilegroup(prj, section)
		if #files > 0 then
			_p(1,'<ItemGroup>')
			for _, file in ipairs(files) do
				local filter
				if file.name ~= file.vpath then
					filter = path.getdirectory(file.vpath)
				else
					filter = path.getdirectory(file.name)
				end				
				
				if filter ~= "." then
					_p(2,'<%s Include=\"%s\">', section, path.translate(file.name, "\\"))
						_p(3,'<Filter>%s</Filter>', path.translate(filter, "\\"))
					_p(2,'</%s>', section)
				else
					_p(2,'<%s Include=\"%s\" />', section, path.translate(file.name, "\\"))
				end
			end
			_p(1,'</ItemGroup>')
		end
	end


--
-- Output the VC2010 filters file
--
	
	function vc2010.generate_filters(prj)
		io.indent = "  "
		vc2010.header()
			vc2010.filteridgroup(prj)
			vc2010.filefiltergroup(prj, "None")
			vc2010.filefiltergroup(prj, "ClInclude")
			vc2010.filefiltergroup(prj, "ClCompile")
			vc2010.filefiltergroup(prj, "ResourceCompile")
		_p('</Project>')
	end
