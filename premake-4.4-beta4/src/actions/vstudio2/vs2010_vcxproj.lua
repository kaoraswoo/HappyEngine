--
-- vs2010_vcxproj.lua
-- Generate a Visual Studio 2010 C/C++ project.
-- Copyright (c) 2009-2011 Jason Perkins and the Premake project
--

	premake.vstudio.vc2010 = { }
	local vc2010 = premake.vstudio.vc2010
	local vstudio = premake.vstudio


	local function vs2010_config(prj)
		_p(1,'<ItemGroup Label="ProjectConfigurations">')
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
				_p(2,'<ProjectConfiguration Include="%s">', premake.esc(cfginfo.name))
					_p(3,'<Configuration>%s</Configuration>',cfginfo.buildcfg)
					_p(3,'<Platform>%s</Platform>',cfginfo.platform)
				_p(2,'</ProjectConfiguration>')
		end
		_p(1,'</ItemGroup>')
	end

	local function vs2010_globals(prj)
		_p(1,'<PropertyGroup Label="Globals">')
			_p(2,'<ProjectGuid>{%s}</ProjectGuid>',prj.uuid)
			_p(2,'<RootNamespace>%s</RootNamespace>',prj.name)
		--if prj.flags is required as it is not set at project level for tests???
		--vs200x generator seems to swap a config for the prj in test setup
		if prj.flags and prj.flags.Managed then
			_p(2,'<TargetFrameworkVersion>v4.0</TargetFrameworkVersion>')
			_p(2,'<Keyword>ManagedCProj</Keyword>')
		else
			_p(2,'<Keyword>Win32Proj</Keyword>')
		end
			_p(2,'<SccProjectName>Perforce Project</SccProjectName>')
			local deppath = path.getrelative(prj.location, prj.solution.location.."\\.." )
			_p(2,'<SccLocalPath>%s</SccLocalPath>', path.translate(deppath, "\\"))
			_p(2,'<SccProvider>MSSCCI:Perforce SCM</SccProvider>')
		_p(1,'</PropertyGroup>')
	end

	function vc2010.config_type(config)
		local t =
		{
			SharedLib = "DynamicLibrary",
			StaticLib = "StaticLibrary",
			ConsoleApp = "Application",
			WindowedApp = "Application"
		}
		return t[config.kind]
	end



	local function if_config_and_platform()
		return 'Condition="\'$(Configuration)|$(Platform)\'==\'%s\'"'
	end

	local function optimisation(cfg)
		local result = "Disabled"
		for _, value in ipairs(cfg.flags) do
			if (value == "Optimize") then
				result = "Full"
			elseif (value == "OptimizeSize") then
				result = "MinSpace"
			elseif (value == "OptimizeSpeed") then
				result = "MaxSpeed"
			end
		end
		return result
	end
	
	---------------------------------------------------------------------------
	--Blueside Customizing
	--for OptimizationLevel
	--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-Setting-PS3-Keyword&action=edit&redlink=1
	----------------------------------------------------------------------------
	local function optilevel(cfg)
		local result = "Level0"
		for _, value in ipairs(cfg.flags) do
			if (value == "OPTLEVEL0") then
				result = "Level0"
			elseif (value == "OPTLEVEL1") then
				result = "Level1"
			elseif (value == "OPTLEVEL2") then
				result = "Level2"
			elseif (value == "OPTLEVEL3") then
				result = "Level3"
			elseif (value == "OPTLEVEL4") then
				result = "Level4"
			elseif (value == "OPTLEVELS") then
				result = "Levels"
			end
		end
		return result
	end
	---------------------------------------------------------------------------
	--/End of Blueside Customizing
	----------------------------------------------------------------------------


--
-- This property group describes a particular configuration: what
-- kind of binary it produces, and some global settings.
--

	function vc2010.configurationPropertyGroup(cfg, cfginfo)
		_p(1,'<PropertyGroup '..if_config_and_platform() ..' Label="Configuration">'
				, premake.esc(cfginfo.name))
		---------------------------------------------------------------------------
		--Blueside Customizing
		--for Flag-Utility job 플스3 관련 프로젝트 지원을 위해서 키워드 추가했음 
		--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-Flags-PS3
		----------------------------------------------------------------------------
		if cfg.flags.UTILITY then
			_p(2,'<ConfigurationType>Utility</ConfigurationType>')
		else
			_p(2,'<ConfigurationType>%s</ConfigurationType>',vc2010.config_type(cfg))
		end
		---------------------------------------------------------------------------
		--/End of Blueside Customization
		----------------------------------------------------------------------------	
		
		_p(2,'<UseDebugLibraries>%s</UseDebugLibraries>', iif(optimisation(cfg) == "Disabled","true","false"))
		_p(2,'<CharacterSet>%s</CharacterSet>',iif(cfg.flags.Unicode,"Unicode","MultiByte"))
		
		---------------------------------------------------------------------------
		--Blueside Customizing
		--for Flag-Utility job 플스3 관련 프로젝트 지원을 위해서 키워드 추가했음 
		--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-Flags-SPU
		--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-Flags-PS3
		----------------------------------------------------------------------------
		if cfg.flags.SPU then  --황제현 추가
		    _p(2,'<PlatformToolset>SPU</PlatformToolset>')
			_p(2,'<SpursUsage>SpursInit</SpursUsage>')
		end
		
		if cfg.flags.ExceptRTTI then  --황제현 추가
			_p(2,'<ExceptionsAndRtti>WithExceptsWithRtti</ExceptionsAndRtti>')
		elseif cfg.flags.NoExceptWithRTTI then
			_p(2,'<ExceptionsAndRtti>NoExceptsWithRtti</ExceptionsAndRtti>')
		end
		
		if cfg.flags.CALLPROF then  --황제현 추가
			_p(2,'<Callprof>true</Callprof>')
		end
		---------------------------------------------------------------------------
		--/End of Blueside Customization
		----------------------------------------------------------------------------	
		
		
		if cfg.flags.MFC then
			_p(2,'<UseOfMfc>%s</UseOfMfc>', iif(cfg.flags.StaticRuntime, "Static", "Dynamic"))
		end

		if cfg.flags.Managed then
			_p(2,'<CLRSupport>true</CLRSupport>')
		end
		_p(1,'</PropertyGroup>')
	end


	local function import_props(prj)
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			_p(1,'<ImportGroup '..if_config_and_platform() ..' Label="PropertySheets">'
					,premake.esc(cfginfo.name))
				_p(2,'<Import Project="$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props" Condition="exists(\'$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\')" Label="LocalAppDataPlatform" />')
			_p(1,'</ImportGroup>')
		end
	end
	
	local function import_userprops(prj)
			local roofflag= true
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
					if roofflag == true then
						for _, projectfile in ipairs(cfg.importprops) do
									_p(4, '<Import Project= \"%s\" />', path.getrelative( prj.location, path.translate(projectfile) ) )
						end
						roofflag = false
					end
						
		end
	end

	local function incremental_link(cfg,cfginfo)
		if cfg.kind ~= "StaticLib" then
			_p(2,'<LinkIncremental '..if_config_and_platform() ..'>%s</LinkIncremental>'
					,premake.esc(cfginfo.name)
					,tostring(premake.config.isincrementallink(cfg)))
		end
	end


	local function ignore_import_lib(cfg,cfginfo)
		if cfg.kind == "SharedLib" then
			local shouldIgnore = "false"
			if cfg.flags.NoImportLib then shouldIgnore = "true" end
			 _p(2,'<IgnoreImportLibrary '..if_config_and_platform() ..'>%s</IgnoreImportLibrary>'
					,premake.esc(cfginfo.name),shouldIgnore)
		end
	end


	local function intermediate_and_out_dirs(prj)
		_p(1,'<PropertyGroup>')
			_p(2,'<_ProjectFileVersion>10.0.30319.1</_ProjectFileVersion>')

			for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
				local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
				_p(2,'<OutDir '..if_config_and_platform() ..'>%s\\</OutDir>'
						, premake.esc(cfginfo.name),premake.esc(cfg.buildtarget.directory) )

				if cfg.platform == "Xbox360" then
					_p(2,'<OutputFile '..if_config_and_platform() ..'>$(OutDir)%s</OutputFile>'
							, premake.esc(cfginfo.name),cfg.buildtarget.name )
				end						
				
				---------------------------------------------------------------------------
				--Blueside Customizing
				--일반적으로 objdir를 자동으로 값을 붙여 넣는 것을 방지 하기 위해서, objdir값이 없을때는 Default로, objdir이 있으면 objdir 그 자체를 넣는다.
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-change-objdir
				----------------------------------------------------------------------------
				if cfg.objdir then  --황제현 추가 if 로직 추가(값이 없거나 비면 원래대로 platform과 프로젝트명으로 넣고, 만약 값 있으면 위와같이 objdir 그대로 간다
						_p(2,'<IntDir '..if_config_and_platform() ..'>%s\\</IntDir>' 
							, premake.esc(cfginfo.name), premake.esc(cfg.objdir)) 
				else
				_p(2,'<IntDir '..if_config_and_platform() ..'>%s\\</IntDir>'
						, premake.esc(cfginfo.name), premake.esc(cfg.objectsdir)) 
				end
				---------------------------------------------------------------------------
				--/End of Blueside Customizing
				----------------------------------------------------------------------------	
							
				_p(2,'<TargetName '..if_config_and_platform() ..'>%s</TargetName>'
						,premake.esc(cfginfo.name),path.getbasename(cfg.buildtarget.name))
						

				ignore_import_lib(cfg,cfginfo)
				incremental_link(cfg,cfginfo)
				if cfg.flags.NoManifest then
				_p(2,'<GenerateManifest '..if_config_and_platform() ..'>false</GenerateManifest>'
						,premake.esc(cfginfo.name))
				end
			end

		_p(1,'</PropertyGroup>')
	end

	local function runtime(cfg)
		local runtime
		local flags = cfg.flags
		if premake.config.isdebugbuild(cfg) then
			runtime = iif(flags.StaticRuntime and not flags.Managed, "MultiThreadedDebug", "MultiThreadedDebugDLL")
		else
			runtime = iif(flags.StaticRuntime and not flags.Managed, "MultiThreaded", "MultiThreadedDLL")
		end
		return runtime
	end

	local function precompiled_header(cfg)
      	if not cfg.flags.NoPCH and cfg.pchheader then
			_p(3,'<PrecompiledHeader>Use</PrecompiledHeader>')
			_p(3,'<PrecompiledHeaderFile>%s</PrecompiledHeaderFile>', cfg.pchheader)
		else
			_p(3,'<PrecompiledHeader></PrecompiledHeader>')
		end
	end

	local function preprocessor(indent,cfg)
		if #cfg.defines > 0 then
			_p(indent,'<PreprocessorDefinitions>%s;%%(PreprocessorDefinitions)</PreprocessorDefinitions>'
				,premake.esc(table.concat(cfg.defines, ";")))
		else
			_p(indent,'<PreprocessorDefinitions></PreprocessorDefinitions>')
		end
	end

	local function include_dirs(indent,cfg)
		if #cfg.includedirs > 0 then
			_p(indent,'<AdditionalIncludeDirectories>%s;%%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>'
					,premake.esc(path.translate(table.concat(cfg.includedirs, ";"), '\\')))
		end
	end
	
	---------------------------------------------------------------------------
	--Blueside Customizing
	--for forceincludedirs keyword && flags.NoStdInc
	--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-Setting-PS3-Keyword&action=edit&redlink=1
	----------------------------------------------------------------------------
	local function force_include_dirs(indent,cfg)
		if #cfg.forceincludedirs > 0 then
			_p(indent,'<ForcedIncludeFiles>%s;%%(ForcedIncludeFiles)</ForcedIncludeFiles>'
					,premake.esc(path.translate(table.concat(cfg.forceincludedirs, ";"), '\\')))
		end
	end
	
	local function nostdinc(cfg)
		if cfg.flags.NoStdInc then
			_p(3,'<NoStdInc>true</NoStdInc>')
		end
	end
	---------------------------------------------------------------------------
	--/End of Blueside Customizing
	----------------------------------------------------------------------------	

	local function resource_compile(cfg)
		_p(2,'<ResourceCompile>')
			preprocessor(3,cfg)
			include_dirs(3,cfg)
		_p(2,'</ResourceCompile>')

	end

	local function exceptions(cfg)
		if cfg.flags.NoExceptions then
			_p(2,'<ExceptionHandling>false</ExceptionHandling>')
		elseif cfg.flags.SEH then
			_p(2,'<ExceptionHandling>Async</ExceptionHandling>')
		--SEH is not required for Managed and is implied
		end
	end

	local function rtti(cfg)
		if cfg.flags.NoRTTI and not cfg.flags.Managed then
			_p(3,'<RuntimeTypeInfo>false</RuntimeTypeInfo>')
		end
	end

	local function wchar_t_buildin(cfg)
		if cfg.flags.NativeWChar then
			_p(3,'<TreatWChar_tAsBuiltInType>true</TreatWChar_tAsBuiltInType>')
		elseif cfg.flags.NoNativeWChar then
			_p(3,'<TreatWChar_tAsBuiltInType>false</TreatWChar_tAsBuiltInType>')
		end
	end

	local function sse(cfg)
		if cfg.flags.EnableSSE then
			_p(3,'<EnableEnhancedInstructionSet>StreamingSIMDExtensions</EnableEnhancedInstructionSet>')
		elseif cfg.flags.EnableSSE2 then
			_p(3,'<EnableEnhancedInstructionSet>StreamingSIMDExtensions2</EnableEnhancedInstructionSet>')
		end
	end

	local function floating_point(cfg)
	     if cfg.flags.FloatFast then
			_p(3,'<FloatingPointModel>Fast</FloatingPointModel>')
		elseif cfg.flags.FloatStrict and not cfg.flags.Managed then
			_p(3,'<FloatingPointModel>Strict</FloatingPointModel>')
		end
	end


	local function debug_info(cfg)
	--
	--	EditAndContinue /ZI
	--	ProgramDatabase /Zi
	--	OldStyle C7 Compatable /Z7
	--
		local debug_info = ''
		if cfg.flags.Symbols then
			if cfg.platform == "x64"
				or cfg.flags.Managed
				or premake.config.isoptimizedbuild(cfg.flags)
				or cfg.flags.NoEditAndContinue
			then
					debug_info = "ProgramDatabase"
			else
				debug_info = "EditAndContinue"
			end
		end
		if debug_info == "" then
		else
			_p(3,'<DebugInformationFormat>%s</DebugInformationFormat>',debug_info)
		end
	end

	local function minimal_build(cfg)
		if premake.config.isdebugbuild(cfg) and not cfg.flags.NoMinimalRebuild then
			_p(3,'<MinimalRebuild>true</MinimalRebuild>')
		else
			_p(3,'<MinimalRebuild>false</MinimalRebuild>')
		end
	end

	local function compile_language(cfg)
		if cfg.language == "C" then
			_p(3,'<CompileAs>CompileAsC</CompileAs>')
		end
	end

	local function vs10_clcompile(cfg)
		_p(2,'<ClCompile>')
		
				---------------------------------------------------------------------------
				--Blueside Customizing
				--for force_include_dirs
				--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-Setting-PS3-Keyword&action=edit&redlink=1
				----------------------------------------------------------------------------
				force_include_dirs(3,cfg)--Blueside Customizing

		if #cfg.buildoptions > 0 then
			_p(3,'<AdditionalOptions>%s %%(AdditionalOptions)</AdditionalOptions>',
					table.concat(premake.esc(cfg.buildoptions), " "))
		end

			_p(3,'<Optimization>%s</Optimization>',optimisation(cfg))
			
			---------------------------------------------------------------------------
			--Blueside Customizing
			--for OptimizationLevel
			--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-Setting-PS3-Keyword&action=edit&redlink=1
			----------------------------------------------------------------------------
			_p(3,'<OptimizationLevel>%s</OptimizationLevel>',optilevel(cfg))

			
			include_dirs(3,cfg)
			---------------------------------------------------------------------------
			--Blueside Customizing
			--for nostdinc
			--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-Setting-PS3-Keyword&action=edit&redlink=1
			----------------------------------------------------------------------------
			nostdinc(cfg) --Blueside Customizing
			preprocessor(3,cfg)
			minimal_build(cfg)

			
		if  not premake.config.isoptimizedbuild(cfg.flags) then
			if not cfg.flags.Managed  then
				---------------------------------------------------------------------------
				--Blueside Customizing
				--for RuntimeCheck
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-BasicRuntimeCheck_keyword
				----------------------------------------------------------------------------
				if not cfg.flags.RuntimeCheck then  --Blueside option /2012.03.19
				_p(3,'<BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>')
				end
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------
			end

			if cfg.flags.ExtraWarnings then
				_p(3,'<SmallerTypeCheck>true</SmallerTypeCheck>')
			end
		else
			_p(3,'<StringPooling>true</StringPooling>')
		end

			_p(3,'<RuntimeLibrary>%s</RuntimeLibrary>', runtime(cfg))

			---------------------------------------------------------------------------
			--Blueside Customizing
			--for Removing FunctionLevelLinking
			--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-BasicRuntimeCheck_keyword
			----------------------------------------------------------------------------
			--_p(3,'<FunctionLevelLinking>true</FunctionLevelLinking>')
			---------------------------------------------------------------------------
			--/End of Blueside Customization
			----------------------------------------------------------------------------

			precompiled_header(cfg)

		if cfg.flags.ExtraWarnings then
			_p(3,'<WarningLevel>Level4</WarningLevel>')
		else
			_p(3,'<WarningLevel>Level3</WarningLevel>')
		end

		if cfg.flags.FatalWarnings then
			_p(3,'<TreatWarningAsError>true</TreatWarningAsError>')
		end

			exceptions(cfg)
			rtti(cfg)
			wchar_t_buildin(cfg)
			sse(cfg)
			floating_point(cfg)
			debug_info(cfg)
			
			
		---------------------------------------------------------------------------
		--Blueside Customizing
		--for Don't use  ProgramDataBaseFileName
		--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-removePDBpart
		----------------------------------------------------------------------------
		--if cfg.flags.Symbols then
		--	_p(3,'<ProgramDataBaseFileName>$(OutDir)%s.pdb</ProgramDataBaseFileName>'
		--		, path.getbasename(cfg.buildtarget.name))
		--end
		---------------------------------------------------------------------------
		--/End of Blueside Customization
		----------------------------------------------------------------------------

		---------------------------------------------------------------------------
		--Blueside Customizing
		--for Flags-OutputObject keyword 
		--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-Flags-OutputObject
		----------------------------------------------------------------------------
		if cfg.flags.NoMultiCompile then
			
		else
			_p(3,' <MultiProcessorCompilation>true</MultiProcessorCompilation>')
		end
		
		if cfg.platform=="PS3" then
			_p(3,'<GenerateDebugInformation>%s</GenerateDebugInformation>', tostring(cfg.flags.Symbols ~= nil))
		end
		
		if cfg.flags.OutputObject then
			_p(3,'<AssemblerListingLocation>$(IntDir)%%(RelativeDir)</AssemblerListingLocation>')
			_p(3,'<ObjectFileName>$(IntDir)%%(RelativeDir)</ObjectFileName>')
			_p(3,'<XMLDocumentationFileName>$(IntDir)%%(RelativeDir)</XMLDocumentationFileName>')
		end
		---------------------------------------------------------------------------
		--/End of Blueside Customization
		----------------------------------------------------------------------------

		if cfg.flags.NoFramePointer then
			_p(3,'<OmitFramePointers>true</OmitFramePointers>')
		end

			compile_language(cfg)

		_p(2,'</ClCompile>')
	end


	local function event_hooks(cfg)
		if #cfg.postbuildcommands> 0 then
		    _p(2,'<PostBuildEvent>')
				_p(3,'<Command>%s</Command>',premake.esc(table.implode(cfg.postbuildcommands, "", "", "\r\n")))
			_p(2,'</PostBuildEvent>')
		end

		if #cfg.prebuildcommands> 0 then
		    _p(2,'<PreBuildEvent>')
				_p(3,'<Command>%s</Command>',premake.esc(table.implode(cfg.prebuildcommands, "", "", "\r\n")))
			_p(2,'</PreBuildEvent>')
		end
		
			---------------------------------------------------------------------------
			--Blueside Customizing
			--for custombuildcommands keyword && custombuildoutputs keyword
			--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-custombuild-commands-outputs&action=edit&redlink=1
			----------------------------------------------------------------------------
		    _p(2,'<CustomBuildStep>')
				if #cfg.custombuildcommands> 0 then
					_p(3,'<Command>%s</Command>',premake.esc(table.implode(cfg.custombuildcommands, "", "", "\r\n")))
				else
					_p(3,'<Command></Command>')
				end
				if #cfg.custombuildoutputs> 0 then
					_p(3,'<Outputs>%s;%%(Outputs)</Outputs>',table.concat(premake.esc(cfg.custombuildoutputs), ';'))
				else
					_p(3,'<Outputs>%%(Outputs)</Outputs>')
				end
			_p(2,'</CustomBuildStep>')
			---------------------------------------------------------------------------
			--/End of Blueside Customizing
			----------------------------------------------------------------------------			
		

		if #cfg.prelinkcommands> 0 then
		    _p(2,'<PreLinkEvent>')
				_p(3,'<Command>%s</Command>',premake.esc(table.implode(cfg.prelinkcommands, "", "", "\r\n")))
			_p(2,'</PreLinkEvent>')
		end
	end

	local function additional_options(indent,cfg)
		if #cfg.linkoptions > 0 then
				_p(indent,'<AdditionalOptions>%s %%(AdditionalOptions)</AdditionalOptions>',
					table.concat(premake.esc(cfg.linkoptions), " "))
		end
	end

	local function link_target_machine(index,cfg)
		local platforms = {x32 = 'MachineX86', x64 = 'MachineX64'}
		if platforms[cfg.platform] then
			_p(index,'<TargetMachine>%s</TargetMachine>', platforms[cfg.platform])
		end
	end

	local function item_def_lib(cfg, prj)
       -- The Xbox360 project files are stored in another place in the project file.
		if cfg.kind == 'StaticLib' and cfg.platform ~= "Xbox360" then
			_p(1,'<Lib>')
				if string.find( cfg.buildtarget.name, "NULL" ) then
					_p(2,'<OutputFile></OutputFile>')
				else
					_p(2,'<OutputFile>$(OutDir)%s</OutputFile>',cfg.buildtarget.name)
				end
				additional_options(2,cfg)
				link_target_machine(2,cfg)

				---------------------------------------------------------------------------
				--Blueside Customizing
				--I add some line for AdditionalLibraryDirectory and Dependencies when libdir, link keyword are used in "StaticLib" 
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-Modifying_lib_keyword
				----------------------------------------------------------------------------
				vc2010.additionalDependencies2(cfg, prj) 
				if #cfg.libdirs > 0 then 
				_p(3,'<AdditionalLibraryDirectories>%s;%%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>', 
						premake.esc(path.translate(table.concat(cfg.libdirs, ';'), '\\')))  
			    end
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------
				
				---------------------------------------------------------------------------
				--Blueside Customizing
				--I add that keyword to ignorelib keyword for 
				-- Configuration - Linker/Labrarian - Ignore Specific Default Lib list
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-ignorelib
				----------------------------------------------------------------------------
				if #cfg.ignorelib > 0 then  
				_p(3,'<IgnoreSpecificDefaultLibraries>%s;%%(IgnoreSpecificDefaultLibraries)</IgnoreSpecificDefaultLibraries>',
						table.concat(premake.esc(cfg.ignorelib), ';')) 
				end
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------
				
				
				---------------------------------------------------------------------------
				--Blueside Customizing
				--I add that keyword to moduledef keyword for 
				--Configuration - Linker/Labrarian - Module Definition File list
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-moduledef
				----------------------------------------------------------------------------
				if #cfg.moduledef > 0 then
				_p(3,'<ModuleDefinitionFile>%s;</ModuleDefinitionFile>',
						table.concat(premake.esc(cfg.moduledef), ';'))
				end
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------
			 
			_p(1,'</Lib>')
		end
	end



	local function import_lib(cfg)
		--Prevent the generation of an import library for a Windows DLL.
		if cfg.kind == "SharedLib" then
			local implibname = cfg.linktarget.fullpath
			_p(3,'<ImportLibrary>%s</ImportLibrary>',iif(cfg.flags.NoImportLib, cfg.objectsdir .. "\\" .. path.getname(implibname), implibname))
		end
	end


--
-- Generate the <Link> element and its children.
--

	function vc2010.link(cfg)
		_p(2,'<Link>')
		_p(3,'<SubSystem>%s</SubSystem>', iif(cfg.kind == "ConsoleApp", "Console", "Windows"))
		_p(3,'<GenerateDebugInformation>%s</GenerateDebugInformation>', tostring(cfg.flags.Symbols ~= nil))

		if premake.config.isoptimizedbuild(cfg.flags) then
			_p(3,'<EnableCOMDATFolding>true</EnableCOMDATFolding>')
			_p(3,'<OptimizeReferences>true</OptimizeReferences>')
		end

		if cfg.kind ~= 'StaticLib' then
			vc2010.additionalDependencies(cfg)
			
			if string.find( cfg.buildtarget.name, "NULL" ) then
				_p(2,'<OutputFile></OutputFile>')
			else
				_p(3,'<OutputFile>$(OutDir)%s</OutputFile>', cfg.buildtarget.name)
			end

			if #cfg.libdirs > 0 then
				_p(3,'<AdditionalLibraryDirectories>%s;%%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>',
						premake.esc(path.translate(table.concat(cfg.libdirs, ';'), '\\')))
			end

			if vc2010.config_type(cfg) == 'Application' and not cfg.flags.WinMain and not cfg.flags.Managed then
				_p(3,'<EntryPointSymbol>mainCRTStartup</EntryPointSymbol>')
			end

			import_lib(cfg)
			link_target_machine(3,cfg)
			additional_options(3,cfg)
			
			---------------------------------------------------------------------------
			--Blueside Customizing
			--I add that keyword to ignorelib keyword for 
			-- Configuration - Linker/Labrarian - Ignore Specific Default Lib list
			--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-ignorelib
			----------------------------------------------------------------------------
			if #cfg.ignorelib > 0 then
				_p(3,'<IgnoreSpecificDefaultLibraries>%s;%%(IgnoreSpecificDefaultLibraries)</IgnoreSpecificDefaultLibraries>',
						table.concat(premake.esc(cfg.ignorelib), ';'))
			end
			---------------------------------------------------------------------------
			--/End of Blueside Customization
			----------------------------------------------------------------------------
			 
			---------------------------------------------------------------------------
			--Blueside Customizing
			--I add that keyword to moduledef keyword for 
			--Configuration - Linker/Labrarian - Module Definition File list
			--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-moduledef
			----------------------------------------------------------------------------
			 if #cfg.moduledef > 0 then
				_p(3,'<ModuleDefinitionFile>%s;</ModuleDefinitionFile>',
						table.concat(premake.esc(cfg.moduledef), ';')) 
			end
			---------------------------------------------------------------------------
			--/End of Blueside Customization
			----------------------------------------------------------------------------
		end

		_p(2,'</Link>')
	end


--
-- Generate the <Link/AdditionalDependencies> element, which links in system
-- libraries required by the project (but not sibling projects; that's handled
-- by an <ItemGroup/ProjectReference>).
--

	function vc2010.additionalDependencies(cfg)
		local links = premake.getlinks(cfg, "system", "fullpath")
		if #links > 0 then
			_p(3,'<AdditionalDependencies>%s;%%(AdditionalDependencies)</AdditionalDependencies>',
						table.concat(links, ";"))
		end
	end
	
	---------------------------------------------------------------------------
	-- Blueside Customizing
	-- Fix error - getrelative between PremakeScriptfolder and item  to   ProjectFolder and item
	-- Made a "fullpath_project"  
	-- refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-link_relativepath
	----------------------------------------------------------------------------
	function vc2010.additionalDependencies2(cfg, prj)
		local links = premake.getlinks(cfg, "system", "fullpath_project", prj)
		if #links > 0 then
			_p(3,'<AdditionalDependencies>%s;%%(AdditionalDependencies)</AdditionalDependencies>',
						table.concat(links, ";"))
		end
	end
	---------------------------------------------------------------------------
	--/End of Blueside Customization
	---------------------------------------------------------------------------

	local function item_definitions(prj)
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			_p(1,'<ItemDefinitionGroup ' ..if_config_and_platform() ..'>'
					,premake.esc(cfginfo.name))
				vs10_clcompile(cfg)
				resource_compile(cfg)
				item_def_lib(cfg, prj)
				vc2010.link(cfg)
				event_hooks(cfg)
			_p(1,'</ItemDefinitionGroup>')


		end
	end



--
-- Retrieve a list of files for a particular build group, one of
-- "ClInclude", "ClCompile", "ResourceCompile", and "None".
--

	function vc2010.getfilegroup(prj, group)
		local sortedfiles = prj.vc2010sortedfiles
		if not sortedfiles then
			sortedfiles = {
				ClCompile = {},
				ClInclude = {},
				None = {},
				ResourceCompile = {},
			}

			for file in premake.project.eachfile(prj) do
				if path.iscppfile(file.name) then
					table.insert(sortedfiles.ClCompile, file)
				elseif path.iscppheader(file.name) then
					table.insert(sortedfiles.ClInclude, file)
				elseif path.isresourcefile(file.name) then
					table.insert(sortedfiles.ResourceCompile, file)
				else
					table.insert(sortedfiles.None, file)
				end
			end

			-- Cache the sorted files; they are used several places
			prj.vc2010sortedfiles = sortedfiles
		end

		return sortedfiles[group]
	end


--
-- Write the files section of the project file.
--

	function vc2010.files(prj)
		vc2010.simplefilesgroup(prj, "ClInclude")
		vc2010.compilerfilesgroup(prj)
		vc2010.simplefilesgroup(prj, "None")
		vc2010.simplefilesgroup(prj, "ResourceCompile")
		--vc2010.ttincludegroup(prj, "None")
	end
	
	---------------------------------------------------------------------------
	--Blueside Customizing
	--for ttinclude keyword
	--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-ttinclude-importproject&action=edit&redlink=1
	----------------------------------------------------------------------------
	function existTToutputfile( prj,  srcTTfilename )
		local srcfilename = path.getbasename( srcTTfilename )
		local roofflag= true
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			if roofflag == true then
				for _, ttfileoutput in ipairs(cfg.ttincludeoutput) do
					ttfilename = path.getbasename( ttfileoutput )
					if ttfilename == srcfilename then
						return ttfileoutput
					end
				end
				roofflag = false
			end
		end
		return false
	end
	
	function existTTfile( prj,  filename )
		local srcfilename = path.getname( filename )
		local roofflag= true
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			if roofflag == true then
				for _, ttfileoutput in ipairs(cfg.ttincludeoutput) do
					ttfilename = path.getname( ttfileoutput )
					if ttfilename == srcfilename then
						return true
					end
				end
				roofflag = false
			end
		end
		return false
	end
	
	function getTTfile( pathfile )
		pathfile= string.gsub(pathfile, "%.(%w+)", ".tt") 
		return pathfile
	end
	
	function vc2010.ttincludegroup(prj, section)
			_p(1,'<ItemGroup>')
					local roofflag= true
					for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
							cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
					if roofflag == true then
						for _, ttfile in ipairs(cfg.ttinclude) do
									_p(4, '<None Include= \"%s" />', path.translate(ttfile) )
						end
						for _, ttfileoutput in ipairs(cfg.ttincludeoutput) do
									_p(4, '<None Include= \"%s" >', path.translate( getTTfile(ttfileoutput) , "\\") )
										_p(5, '<Generator>TextTemplatingFileGenerator</Generator>' )
										_p(5, '<LastGenOutput>%s</LastGenOutput>' ,  path.translate(ttfileoutput) )
									_p(4, '</None>' )
						end
						roofflag = false
					end
						
					end
			_p(1,'</ItemGroup>')
		
	end
	
	
	function vc2010.importproject(prj)
		local roofflag= true
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
					if roofflag == true then
						for _, projectfile in ipairs(cfg.importproject) do
									_p(4, '<Import Project= \"%s\" />', path.translate(projectfile) )
						end
						roofflag = false
					end
						
		end
	end
	---------------------------------------------------------------------------
	--/End of Blueside Customization
	----------------------------------------------------------------------------	
	
	---------------------------------------------------------------------------
	--Blueside Customizing
	--for TransformOnBuild Flag keyword
	--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-TransformOnBuild
	----------------------------------------------------------------------------
	function vc2010.transformonbuild(prj)
		local roofflag= true
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
				if cfg.flags.TransformOnBuild and roofflag == true then
					_p(3, '<PropertyGroup>' )
					_p(4, '<TransformOnBuild>true</TransformOnBuild>' )
					_p(4, '<TransformOutOfDateOnly>true</TransformOutOfDateOnly>' )
					_p(3, '</PropertyGroup>' )
					roofflag = false
				end
		end
	end
	---------------------------------------------------------------------------
	--/End of Blueside Customization
	----------------------------------------------------------------------------	

	function vc2010.simplefilesgroup(prj, section)
		local files = vc2010.getfilegroup(prj, section)
		if #files > 0  then
			_p(1,'<ItemGroup>')
			for _, file in ipairs(files) do
				---------------------------------------------------------------------------
				--Blueside Customizing
				--for buildexclude keyword
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-buildexcludes
				--for ttinclude keyword
				--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-ttinclude-importproject&action=edit&redlink=1
				----------------------------------------------------------------------------
				if existTTfile( prj, file.name ) == true then
					--ttfile 이 존재하면 찍지 않는다
				else
					_p(2,'<%s Include=\"%s\">', section, path.translate(file.name, "\\")) 
						for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
							local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
							for _, exclude in ipairs(cfg.buildexcludes) do
								if (string.find(file.name, exclude)) then 
										_p(4, '<ExcludedFromBuild Condition= \"\'$(Configuration)|$(Platform)\' == \'%s\' ' .. '\">true</ExcludedFromBuild>', cfginfo.name)
								end
							end
						end
						
						--.tt file
						if path.getextension( file.name) == ".tt" then
							outputname = existTToutputfile( prj, file.name )
							if  outputname == false then
							
							else
									_p(5, '<Generator>TextTemplatingFileGenerator</Generator>' )
									_p(5, '<LastGenOutput>%s</LastGenOutput>' ,  path.translate( outputname  ) )
							end
						end
						
						_p(2,'</%s>', section)
						
				end
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------	
			end
				
				if section == "ClInclude" then
					local roofflag= true
					for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
						cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
						if roofflag == true then
							for _, ttfileoutput in ipairs(cfg.ttincludeoutput) do
									if path.getextension( ttfileoutput ) == ".h" then
										_p(4, '<ClInclude Include= \"%s" >', path.translate( ttfileoutput , "\\") )
											_p(5, '<AutoGen>True</AutoGen>' )
											_p(5, '<DesignTime>True</DesignTime>' )
											_p(5, '<DependentUpon>%s</DependentUpon>' ,  path.translate( getTTfile(ttfileoutput) ) )
											
											
											---------------------------------------------------------------------------
											--Blueside Customizing
											--for buildexclude keyword
											--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-buildexcludes
											----------------------------------------------------------------------------
											for _, cfginfo2 in ipairs(prj.solution.vstudio_configs) do
												local cfg2 = premake.getconfig(prj, cfginfo2.src_buildcfg, cfginfo2.src_platform)
													for _, exclude in ipairs(cfg2.buildexcludes) do
															dash_filename= string.gsub(ttfileoutput, "-", "_dash_")  --황제현 add
															dash_excludename= string.gsub(exclude, "-", "_dash_")
														--if (string.find(file.name, exclude)) then   --아래로 수정, dash '-'에 대한 처리
														if(string.find(dash_filename, dash_excludename)) then
																_p(4, '<ExcludedFromBuild Condition= \"\'$(Configuration)|$(Platform)\' == \'%s\' ' .. '\">true</ExcludedFromBuild>', cfginfo2.name)
														end
													end
											end
											---------------------------------------------------------------------------
											--/End of Blueside Customization
											----------------------------------------------------------------------------
											
											
										_p(4, '</ClInclude>' )
									end
							end
							roofflag = false
						end
					end
				end
			
			
			_p(1,'</ItemGroup>')
		end
	end
	
	


	function vc2010.compilerfilesgroup(prj)
	
	
		local configs = prj.solution.vstudio_configs
		local files = vc2010.getfilegroup(prj, "ClCompile")
		if #files > 0  then
			local config_mappings = {}
			for _, cfginfo in ipairs(configs) do
				local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
				if cfg.pchheader and cfg.pchsource and not cfg.flags.NoPCH then
					config_mappings[cfginfo] = path.translate(cfg.pchsource, "\\")
				end
			end

			_p(1,'<ItemGroup>')
			for _, file in ipairs(files) do
			
			if existTTfile( prj, file.name ) == true then
					--ttfile 이 존재하면 찍지 않는다
			else
			
				local translatedpath = path.translate(file.name, "\\")
				_p(2,'<ClCompile Include=\"%s\">', translatedpath)
				for _, cfginfo in ipairs(configs) do
					if config_mappings[cfginfo] and translatedpath == config_mappings[cfginfo] then
						_p(3,'<PrecompiledHeader '.. if_config_and_platform() .. '>Create</PrecompiledHeader>', premake.esc(cfginfo.name))
						config_mappings[cfginfo] = nil  --only one source file per pch
					end
				end
				
				---------------------------------------------------------------------------
				--Blueside Customizing
				--for fileprecompile, nofileprecompile keyword 
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-fileprecompile
				----------------------------------------------------------------------------
				for _, cfginfo in ipairs(configs) do
					local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
					for _, precompile in ipairs(cfg.fileprecompile) do 
								if(string.find(file.name, precompile)) then
										_p(4, '<PrecompiledHeader>Create</PrecompiledHeader>')
								end
					end
				end
				
				
				for _, cfginfo in ipairs(configs) do
					local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
					for _, precompile in ipairs(cfg.nofileprecompile) do 
								if(string.find(file.name, precompile)) then
										_p(4, '<PrecompiledHeader>NotUsing</PrecompiledHeader>')
								end
					end
				end
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------
				
				---------------------------------------------------------------------------
				--Blueside Customizing
				--for buildexclude keyword
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-buildexcludes
				----------------------------------------------------------------------------
				for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
					local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
						for _, exclude in ipairs(cfg.buildexcludes) do
								dash_filename= string.gsub(file.name, "-", "_dash_")  --황제현 add
								dash_excludename= string.gsub(exclude, "-", "_dash_")
							--if (string.find(file.name, exclude)) then   --아래로 수정, dash '-'에 대한 처리
							if(string.find(dash_filename, dash_excludename)) then
									_p(4, '<ExcludedFromBuild Condition= \"\'$(Configuration)|$(Platform)\' == \'%s\' ' .. '\">true</ExcludedFromBuild>', cfginfo.name)
							end
						end
				end
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------	
				
				
				_p(2,'</ClCompile>')
			end
			end
			
			---------------------------------------------------------------------------
			--Blueside Customizing
			--for ttinclude keyword
			--refer http://mediawiki.blueside.co.kr/index.php5?title=Premake-structure-ttinclude-importproject&action=edit&redlink=1
			----------------------------------------------------------------------------
			local roofflag= true
					for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
						cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
						if roofflag == true then
							for _, ttfileoutput in ipairs(cfg.ttincludeoutput) do
									if path.getextension( ttfileoutput ) == ".cpp" then
										_p(4, '<ClCompile Include= \"%s" >', path.translate( ttfileoutput , "\\") )
											_p(5, '<AutoGen>True</AutoGen>' )
											_p(5, '<DesignTime>True</DesignTime>' )
											_p(5, '<DependentUpon>%s</DependentUpon>' ,  path.translate( getTTfile(ttfileoutput) ) )
											
											
											---------------------------------------------------------------------------
											--Blueside Customizing
											--for buildexclude keyword
											--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-buildexcludes
											----------------------------------------------------------------------------
											for _, cfginfo2 in ipairs(prj.solution.vstudio_configs) do
												local cfg2 = premake.getconfig(prj, cfginfo2.src_buildcfg, cfginfo2.src_platform)
													for _, exclude in ipairs(cfg2.buildexcludes) do
															dash_filename= string.gsub(ttfileoutput, "-", "_dash_")  --황제현 add
															dash_excludename= string.gsub(exclude, "-", "_dash_")
														--if (string.find(file.name, exclude)) then   --아래로 수정, dash '-'에 대한 처리
														if(string.find(dash_filename, dash_excludename)) then
																_p(4, '<ExcludedFromBuild Condition= \"\'$(Configuration)|$(Platform)\' == \'%s\' ' .. '\">true</ExcludedFromBuild>', cfginfo2.name)
														end
													end
											end
											---------------------------------------------------------------------------
											--/End of Blueside Customization
											----------------------------------------------------------------------------
											
											
											
										_p(4, '</ClCompile>' )
									end
							end
							roofflag = false
						end
					end
			---------------------------------------------------------------------------
			--/End of Blueside Customization
			----------------------------------------------------------------------------			
			
			_p(1,'</ItemGroup>')
		end
	end


--
-- Output the VC2010 project file header
--

	function vc2010.header(targets)
		io.eol = "\r\n"
		_p('<?xml version="1.0" encoding="utf-8"?>')

		local t = ""
		if targets then
			t = ' DefaultTargets="' .. targets .. '"'
		end

		_p('<Project%s ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">', t)
	end


--
-- Output the VC2010 C/C++ project file
--

	function premake.vs2010_vcxproj(prj)
		io.indent = "  "
		vc2010.header("Build")

			vs2010_config(prj)
			vs2010_globals(prj)

			_p(1,'<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.Default.props" />')

			for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
				local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
				vc2010.configurationPropertyGroup(cfg, cfginfo)
			end

			_p(1,'<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.props" />')
			
			import_userprops(prj)

			--check what this section is doing
			_p(1,'<ImportGroup Label="ExtensionSettings">')
			_p(1,'</ImportGroup>')


			import_props(prj)

			--what type of macros are these?
			_p(1,'<PropertyGroup Label="UserMacros" />')

			intermediate_and_out_dirs(prj)

			item_definitions(prj)

			vc2010.files(prj)
			vc2010.projectReferences(prj)

			_p(1,'<Import Project="$(VCTargetsPath)\\Microsoft.Cpp.targets" />')
			_p(1,'<ImportGroup Label="ExtensionTargets">')
			_p(1,'</ImportGroup>')
			
			vc2010.importproject(prj)
			vc2010.transformonbuild(prj)

		_p('</Project>')
	end


--
-- Generate the list of project dependencies.
--

	function vc2010.projectReferences(prj)
		local deps = premake.getdependencies(prj)
		if #deps > 0 then
			_p(1,'<ItemGroup>')
			for _, dep in ipairs(deps) do
				---------------------------------------------------------------------------
				--Blueside Customizing
				--local deppath = path.getrelative(prj.solution.location, vstudio.projectfile(dep))
				--아래와 같이 수정했음 왜냐하면 solution 디렉토리와 Dependencies projects 들과 상대경로 비교하면 못찾기에 아래와 같이 해당프로젝트와 디펜던시의 상대경로로 수정
				--refer http://mediawiki.blueside.co.kr/index.php5/Premake-structure-dependency%EC%83%81%EB%8C%80%EA%B2%BD%EB%A1%9C%EC%88%98%EC%A0%95
				----------------------------------------------------------------------------
				local deppath = path.getrelative(prj.location, vstudio.projectfile(dep))
				---------------------------------------------------------------------------
				--/End of Blueside Customization
				----------------------------------------------------------------------------
				_p(2,'<ProjectReference Include=\"%s\">', path.translate(deppath, "\\")) 
				_p(3,'<Project>{%s}</Project>', dep.uuid)
				_p(2,'</ProjectReference>')
			end
			_p(1,'</ItemGroup>')
		end
	end


--
-- Generate the .vcxproj.user file
--

	function vc2010.debugdir(cfg)
		if cfg.debugdir then
			_p('    <LocalDebuggerWorkingDirectory>%s</LocalDebuggerWorkingDirectory>', path.translate(cfg.debugdir, '\\'))
			_p('    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>')
		end
		if cfg.debugargs then
			_p('    <LocalDebuggerCommandArguments>%s</LocalDebuggerCommandArguments>', table.concat(cfg.debugargs, " "))
		end
	end

	function vc2010.debugenvs(cfg)
		if cfg.debugenvs and #cfg.debugenvs > 0 then
			_p(2,'<LocalDebuggerEnvironment>%s%s</LocalDebuggerEnvironment>',table.concat(cfg.debugenvs, "\n")
					,iif(cfg.flags.DebugEnvsInherit,'\n$(LocalDebuggerEnvironment)','')
				)
			if cfg.flags.DebugEnvsDontMerge then
				_p(2,'<LocalDebuggerMergeEnvironment>false</LocalDebuggerMergeEnvironment>')
			end
		end
	end

	function premake.vs2010_vcxproj_user(prj)
		io.indent = "  "
		vc2010.header()
		for _, cfginfo in ipairs(prj.solution.vstudio_configs) do
			local cfg = premake.getconfig(prj, cfginfo.src_buildcfg, cfginfo.src_platform)
			_p('  <PropertyGroup '.. if_config_and_platform() ..'>', premake.esc(cfginfo.name))
			vc2010.debugdir(cfg)
			vc2010.debugenvs(cfg)
			_p('  </PropertyGroup>')
		end
		_p('</Project>')
	end



