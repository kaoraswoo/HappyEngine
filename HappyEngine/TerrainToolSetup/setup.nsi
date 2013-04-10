Name "Terrain Tool(WPF) with HappyEngine(C++)"

# General Symbol Definitions
!define REGKEY "SOFTWARE\$(^Name)"
!define VERSION 0.0.0.1
!define COMPANY "Hwang Je Hyun"
!define URL http://blog.naver.com/kaoraswoo
!define PRODUCT_DIR_NAME "HappyEngineTool"
!define PRODUCT_EXE_NAME "D3DHost"
!define PRODUCT_NAME "HappyEngineTool"
!define PRODUCT_WEB_SITE "http://blog.naver.com/kaoraswoo"

# MUI Symbol Definitions
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install-colorful.ico"
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall-colorful.ico"
!define MUI_UNFINISHPAGE_NOAUTOCLOSE

# Included files
!include Sections.nsh
!include MUI2.nsh
!include FileAssociation.nsh

# Installer pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

# Installer languages
!insertmacro MUI_LANGUAGE English

# Installer attributes
OutFile Happy_TerrainTool_Setup.exe
InstallDir "$PROGRAMFILES\HappyEngine\TerrainTool"
CRCCheck on
XPStyle on
Icon "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
SilentInstall normal
VIProductVersion ${VERSION}
VIAddVersionKey ProductName "Happy Terrain Tool"
VIAddVersionKey ProductVersion "${VERSION}"
VIAddVersionKey CompanyName "${COMPANY}"
VIAddVersionKey CompanyWebsite "${URL}"
VIAddVersionKey FileVersion "${VERSION}"
VIAddVersionKey FileDescription ""
VIAddVersionKey LegalCopyright ""
InstallDirRegKey HKLM "${REGKEY}" Path
UninstallIcon "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
SilentUnInstall normal

# This define should be provided externally to the makensis tool
# If it is not there we take this as default, for testing
!ifndef Happy_DIR
    !define Happy_DIR "E:\HappyEngine"
!endif

# Installer sections
Section -Main SEC0000
    SetOutPath $INSTDIR
    SetOverwrite on
	SetOutPath $INSTDIR\bin\HTool_Release
    File ${Happy_DIR}\bin\HTool_Release\*

    SetOutPath $INSTDIR\Data
    File /r /x *.svn /x *.tcinf /x *.dep ${Happy_DIR}\Data\*    

	SetOutPath $INSTDIR\bin\HTool_Release

    WriteRegStr HKLM "${REGKEY}\Components" Main 1

    # associate object files to our tool
    
    # add windows start menu icons
    CreateDirectory "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}"
    CreateShortcut "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}\${PRODUCT_NAME}.lnk "$INSTDIR\bin\HTool_Release\${PRODUCT_EXE_NAME}.exe"
    WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
    CreateShortcut "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
    CreateShortCut "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
 
SectionEnd

Section -post SEC0002
    WriteRegStr HKLM "${REGKEY}" Path $INSTDIR
    SetOutPath $INSTDIR
    WriteUninstaller $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayName "$(^Name)"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayVersion "${VERSION}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" Publisher "${COMPANY}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" URLInfoAbout "${URL}"
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" DisplayIcon $INSTDIR\uninstall.exe
    WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" UninstallString $INSTDIR\uninstall.exe
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoModify 1
    WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)" NoRepair 1
SectionEnd

# Macro for selecting uninstaller sections
!macro SELECT_UNSECTION SECTION_NAME UNSECTION_ID
    Push $R0
    ReadRegStr $R0 HKLM "${REGKEY}\Components" "${SECTION_NAME}"
    StrCmp $R0 1 0 next${UNSECTION_ID}
    !insertmacro SelectSection "${UNSECTION_ID}"
    GoTo done${UNSECTION_ID}
next${UNSECTION_ID}:
    !insertmacro UnselectSection "${UNSECTION_ID}"
done${UNSECTION_ID}:
    Pop $R0
!macroend

# Uninstaller sections
Section /o -un.Main UNSEC0000
    #RmDir /r /REBOOTOK $INSTDIR
    DeleteRegValue HKLM "${REGKEY}\Components" Main

    #Deassociate file extensions
    
    #remove windows start menu icons
    Delete    "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}\Uninstall.lnk"
    Delete    "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}\Website.lnk"
    Delete    "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}\${PRODUCT_NAME}.lnk"
    RMDir /r  "$SMPROGRAMS\HappyEngine\${PRODUCT_DIR_NAME}"
    RMDir     "$SMPROGRAMS\HappyEngine"
    
SectionEnd

Section -un.post UNSEC0002
    DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\$(^Name)"
    Delete /REBOOTOK $INSTDIR\uninstall.exe
    DeleteRegValue HKLM "${REGKEY}" Path
    DeleteRegKey /IfEmpty HKLM "${REGKEY}\Components"
    DeleteRegKey /IfEmpty HKLM "${REGKEY}"
SectionEnd

# Installer functions
Function .onInstSuccess
FunctionEnd

Function .onInit
    InitPluginsDir
FunctionEnd

# Uninstaller functions
Function un.onInit
    ReadRegStr $INSTDIR HKLM "${REGKEY}" Path
    !insertmacro SELECT_UNSECTION Main ${UNSEC0000}
FunctionEnd
