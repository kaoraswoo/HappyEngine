------------------------------------------------------------------------------------------------------------------------
--Projects.lua 
-- Project Declaration
-- made by hjh84
current_file =  "Projects_HappyEngine.lua"
------------------------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------
--  Set Solution Directory "Root/HappyEngine"
----------------------------------------------------------------------------------------
SetSolutionDirectory("HappyEngine")

----------------------------------------------------------------------------------------
--  Library Area
----------------------------------------------------------------------------------------
EngineLibrary("HEngine")
	Set_Uuid( "All", "3DA849A9-E07F-CB9E-AA1F-F798AD13BB61" )
	Add_Include( "Win32" , {  g_final_project_folder , g_final_project_folder.."\\..\\External\\boost" , g_final_project_folder.."\\..\\External\\DirectXSDK\\Include" } )
	Add_Defines( "Debug|Win32" , {"WIN32",	"_DEBUG",	"_LIB"	})
	Add_Defines( "Release|Win32" , {"WIN32",	"NDEBUG",	"_LIB"	})
	Add_Link( "Win32" , { "odbc32", "odbccp32", "d3d9", "d3dx9", "winmm" } )
	Add_Lib_Dir( "Win32", { g_final_project_folder.."\\..\\External\\DirectXSDK\\Lib\\x86" , g_final_project_folder.."\\..\\External\\boost\\lib"})
	
EmptyProject("D3DContent")
EmptyProject("ManagedHEngine")
CSharp_Application("HControl")
CSharp_Application("D3DHost")
CSharp_Application("TestWinForm")
