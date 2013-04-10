--
-- vs2005_solution.lua
-- Generate a Visual Studio 2005-2010 solution.
-- Copyright (c) 2009-2011 Jason Perkins and the Premake project
--

	premake.vstudio.sln2005 = { }
	local vstudio = premake.vstudio
	local sln2005 = premake.vstudio.sln2005
	
	---------------------------------------------------------------------------
	-- Blueside Customizing
	-- svpaths keyword ( �����Ͽ��� �������� ����.. �� ���� �Լ����� ȣ���ؾ��ϹǷ�..)
	-- refer 
	----------------------------------------------------------------------------
	svpathuuid={}
	---------------------------------------------------------------------------
	--/End of Blueside Customization
	----------------------------------------------------------------------------
	
	
	

	function sln2005.generate(sln)
		io.eol = '\r\n'

		-- Precompute Visual Studio configurations
		-- Blueside Customizing
		sln.vstudio_configs = premake.vstudio.buildconfigs2(sln)
		
		-- Mark the file as Unicode
		_p('\239\187\191')

		sln2005.header(sln)
		
		sln2005.declare_svpaths(sln) -- Blueside Customizing
		
		--------------------------------------------------------------------------
		-- Blueside Customizing
		-- Perforce Integration
		-- refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-notgenerateproject
		----------------------------------------------------------------------------
		for prj in premake.solution.eachproject(sln) do
			--uuid�� ������ �����ϴ� ������ ������
			local getuuid = get_project_uuid(prj)
			if prj.generateproject == "false" and getuuid ~= -1 then   --�ش� ������Ʈ�� ���� ���ϴ� ������ ǥ��Ǿ� �ְ� ������Ʈ������ �����ؼ� ����� uuid�� �޾ƿ�����
				prj.uuid = getuuid
			end
			--------------
			sln2005.project(prj)
		end
		---------------------------------------------------------------------------
		--/End of Blueside Customization
		----------------------------------------------------------------------------
		

		_p('Global')
		---------------------------------------------------------------------------
		-- Blueside Customizing
		-- Perforce Integration
		-- refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-Perforce-Integration
		----------------------------------------------------------------------------
		sln2005.perforce(sln)
		---------------------------------------------------------------------------
		--/End of Blueside Customization
		----------------------------------------------------------------------------
		
		sln2005.platforms(sln)
		sln2005.project_platforms(sln)
		sln2005.properties(sln)
		
		---------------------------------------------------------------------------
		-- Blueside Customizing
		-- svpaths keyword
		-- refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-svpaths&action=edit&redlink=1
		----------------------------------------------------------------------------
		_p('\tGlobalSection(NestedProjects) = preSolution')
			sln2005.set_svpaths_vpath(sln)
		for prj in premake.solution.eachproject(sln) do
			sln2005.set_project_vpath(sln,prj) -- Blueside Customizing
		end
		_p('\tEndGlobalSection')
		---------------------------------------------------------------------------
		--/End of Blueside Customization
		----------------------------------------------------------------------------
		
		_p('EndGlobal')
	end


--
-- Generate the solution header
--

	function sln2005.header(sln)
		local version = {
			vs2005 = 9,
			vs2008 = 10,
			vs2010 = 11
		}

		_p('Microsoft Visual Studio Solution File, Format Version %d.00', version[_ACTION])
		_p('# Visual Studio %s', _ACTION:sub(3))
	end


--
-- Write out an entry for a project
--

	function sln2005.project(prj)
		-- Build a relative path from the solution file to the project file
		local projpath = path.translate(path.getrelative(prj.solution.location, vstudio.projectfile(prj)), "\\")
			
		_p('Project("{%s}") = "%s", "%s", "{%s}"', vstudio.tool(prj), prj.name, projpath, prj.uuid)
		sln2005.projectdependencies(prj)
		_p('EndProject')
	end


--
-- Write out the list of project dependencies for a particular project.
--

	function sln2005.projectdependencies(prj)
		local deps = premake.getdependencies(prj)
		if #deps > 0 then
			_p('\tProjectSection(ProjectDependencies) = postProject')
			for _, dep in ipairs(deps) do
				_p('\t\t{%s} = {%s}', dep.uuid, dep.uuid)
			end
			_p('\tEndProjectSection')
		end
	end


--
-- Write out the contents of the SolutionConfigurationPlatforms section, which
-- lists all of the configuration/platform pairs that exist in the solution.
--

	function sln2005.platforms(sln)
		_p('\tGlobalSection(SolutionConfigurationPlatforms) = preSolution')
		for _, cfg in ipairs(sln.vstudio_configs) do
			_p('\t\t%s = %s', cfg.name, cfg.name)
		end
		_p('\tEndGlobalSection')
	end
	
--
-- Write out the contents of the Perforce connection section, which maps
--
	---------------------------------------------------------------------------
	-- Blueside Customizing
	-- Perforce Integration
	-- refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-Perforce-Integration
	----------------------------------------------------------------------------
	function sln2005.perforce(sln)
		projectTotalCount = 0
		for prj in premake.solution.eachproject(sln) do
			projectTotalCount = projectTotalCount + 1
		end
		
		print("SolutionLocation:"..sln.location)
		print("RelativeSolution:".. path.getrelative(os.getcwd(), sln.location))
		print("FixedRelativeSolution:".. path.getrelative(os.getcwd().."/..", sln.location))
		FixedRelativeSolution=path.getrelative(os.getcwd().."/..", sln.location)
		FixedRelativeSolution = string.gsub(FixedRelativeSolution, "/", "\\\\")
		
		ConnectionFixedRelativeSolution = path.getrelative(os.getcwd().."/../..", sln.location)
		ConnectionFixedRelativeSolution = string.gsub(ConnectionFixedRelativeSolution, "/", "\\\\")
		_p('\tGlobalSection(SourceCodeControl) = preSolution')
		
		_p('\t\tSccNumberOfProjects = %d', projectTotalCount+1)
		_p('\t\tSccProjectName0 = Perforce\\u0020Project')
		_p('\t\tSccLocalPath0 = ..')
		_p('\t\tSccProvider0 = MSSCCI:Perforce\\u0020SCM')
		_p('\t\tSccProjectFilePathRelativizedFromConnection0 = %s\\\\',ConnectionFixedRelativeSolution)
		
		projectCount =0
		for prj in premake.solution.eachproject(sln) do
			if premake.isdotnetproject(prj) then
				ext =  "csproj"
			else
				ext =  "vcxproj"
			end
			FixedRelativeProjectFolder = path.getrelative(sln.location, prj.location)
			FixedRelativeProjectFolder = string.gsub(FixedRelativeProjectFolder, "/", "\\\\")
			ConnectionFixedRelativeSolution = path.getrelative(sln.location.."/..", prj.location)
			ConnectionFixedRelativeSolution = string.gsub(ConnectionFixedRelativeSolution, "/", "\\\\")
			projectCount = projectCount + 1
			projectfolder = string.gsub(prj.name, "_", "\\\\")
			_p('\t\tSccProjectUniqueName%d = %s\\\\%s.%s',projectCount, FixedRelativeProjectFolder, prj.name, ext )
			_p('\t\tSccProjectTopLevelParentUniqueName%d = %s.sln',projectCount, sln.name)
			_p('\t\tSccLocalPath%d = ..',projectCount)
			_p('\t\tSccProjectFilePathRelativizedFromConnection%d = %s\\\\',projectCount, ConnectionFixedRelativeSolution)
		end
		
		_p('\tEndGlobalSection')
	end
	---------------------------------------------------------------------------
	--/End of Blueside Customization
	----------------------------------------------------------------------------
	
	
	
	---------------------------------------------------------------------------
	-- Blueside Customizing
	-- svpaths keyword
	-- refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-svpaths&action=edit&redlink=1
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
	
	function sln2005.declare_svpaths(sln)
		for replacement,patterns in pairs(sln.svpaths) do
			if replacement ~="" then  --If it is "" , we consider it as Root path
				--svpathuuid[replacement]=os.uuid()
				svpathuuid[replacement]=MakeGUIDfromSeed(replacement)
				_p("Project(\"{2150E333-8FDC-42A3-9474-1A3956D46DE8}\") = \"%s\", \"%s\", \"{%s}\"", replacement, replacement, svpathuuid[replacement])
				--_p('EndGlobalSection')
				_p('EndProject')
			end
		end
	end
	
	function sln2005.set_svpaths_vpath(sln)
		for standard,iterator in pairs(sln.svpaths) do
			for replacement,patterns in pairs(sln.svpaths) do
				if replacement ~="" then  --If it is "" , we consider it as Root path
					for _,pattern in ipairs(patterns) do
						if standard == pattern then
							_p("{%s} = {%s}}", svpathuuid[standard], svpathuuid[replacement])
						end
					end
				end
			end
		end
	end
	
	function sln2005.set_project_vpath(sln,prj)
		for replacement,patterns in pairs(sln.svpaths) do
			for _,pattern in ipairs(patterns) do
				if replacement ~="" then  --If it is "" , we consider it as Root path
					if prj.name == pattern then
						_p("{%s} = {%s}}", prj.uuid, svpathuuid[replacement])
					end
				end
			end
		end
	end
	---------------------------------------------------------------------------
	--/End of Blueside Customization
	----------------------------------------------------------------------------
	

--
-- Write out the contents of the ProjectConfigurationPlatforms section, which maps
-- the configuration/platform pairs into each project of the solution.
--

	function sln2005.project_platforms(sln)
		_p('\tGlobalSection(ProjectConfigurationPlatforms) = postSolution')
		for prj in premake.solution.eachproject(sln) do
			for _, cfg in ipairs(sln.vstudio_configs) do
			
				-- .NET projects always map to the "Any CPU" platform (for now, at 
				-- least). For C++, "Any CPU" and "Mixed Platforms" map to the first
				-- C++ compatible target platform in the solution list.
				local mapped
				if premake.isdotnetproject(prj) then
					mapped = "Any CPU"
				else
					if cfg.platform == "Any CPU" or cfg.platform == "Mixed Platforms" then
						mapped = sln.vstudio_configs[3].platform
					else
						mapped = cfg.platform
					end
				end
				
			---------------------------------------------------------------------------
			--Blueside Customizing
			--projectset Ű����� projectbuildcheck Ű���带 ���� ���⼭���� �ٽ� �߰��� ���� (Ȳ����, 20120417)
			--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-solution-projectset-projectbuildcheck&action=edit&redlink=1
			----------------------------------------------------------------------------
				if cfg.platform=="Win32" then    --���� �ַ�ǿ����� premake.getconfig ���� project platform �� Win32��� x32�� �־�� �νĵȴ�. �ٸ� �÷����� ������°� Ȯ��(x64, PS3)
					projectplatform = "x32"
				else
					projectplatform = cfg.platform
				end
				local cfg2 = premake.getconfig(prj, cfg.buildcfg, projectplatform)
				if cfg2.projectset then -- �ش� ������Ʈ�� projectset Ű���忡 ���� �����Ѵٸ� projectset  Ű���� ������ �����Ѵ� 
					replacebuild_platform = cfg2.projectset
				else --�׷��� �ʴٸ� ���� replacebuild_platform = cfg.buildcfg.."|"..mapped �� �״�� ������
					replacebuild_platform = cfg.buildcfg.."|"..mapped
				end
				_p('\t\t{%s}.%s.ActiveCfg = %s', prj.uuid, cfg.name, replacebuild_platform)  --replacebuild_platform ������ ���� �ȴ�
				
				--projectbuildcheck ����(�� ������Ʈ���� ���� �ִ�)
				if mapped == cfg.platform or cfg.platform == "Mixed Platforms" then
					if #prj.projectbuildcheck> 0 then  --�ش� projectbuildcheck ����Ʈ�� ���� �ִٸ�
						for i=1, #prj.projectbuildcheck do --projectbuildcheck list ��ŭ �ݺ�
							finalmatch = cfg.buildcfg.."|"..mapped
							if cfg.buildcfg == prj.projectbuildcheck[i] or mapped==prj.projectbuildcheck[i] or prj.projectbuildcheck[i] == finalmatch  then  --�ش� projectbuildcheck���� ���� platform, conf Ȥ�� conf | platform  �� ��ġ�Ѵٸ�
								--print("������Ʈ��:",prj.name, "  ��:", prj.projectbuildcheck[i])
								_p('\t\t{%s}.%s.Build.0 = %s',  prj.uuid, cfg.name, replacebuild_platform)  --���� ��Ͽ� �߰��Ѵ�.
							end
						end
					end
					--_p('\t\t{%s}.%s.Build.0 = %s|%s',  prj.uuid, cfg.name, cfg.buildcfg, mapped)    --���� �ִ� ������Ʈ ���忩�� ������ ����
				---------------------------------------------------------------------------
				--/End of Blueside Customizing
				---------------------------------------------------------------------------
				end
			end
		end
		_p('\tEndGlobalSection')
	end
	
	

--
-- Write out contents of the SolutionProperties section; currently unused.
--

	function sln2005.properties(sln)	
		_p('\tGlobalSection(SolutionProperties) = preSolution')
		_p('\t\tHideSolutionNode = FALSE')
		_p('\tEndGlobalSection')
	end
