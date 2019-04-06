; example2.nsi
;
; This script is based on example1.nsi, but it remember the directory, 
; has uninstall support and (optionally) installs start menu shortcuts.
;
; It will install example2.nsi into a directory that the user selects,

;--------------------------------

; The name of the installer
Name "Map"

; The file to write
OutFile "Map.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Map

; Registry key to check for directory (so if you install again, it will 
; overwrite the old one automatically)
InstallDirRegKey HKLM "Software\Map" "Install_Dir"

; Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------

; Pages

;Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------

; The stuff to install
Section "Map (required)"

  SectionIn RO
  
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File "Release\Map.exe"
  File "\QtSDK\mingw\bin\mingwm10.dll"
  File "\QtSDK\mingw\bin\libgcc_s_dw2-1.dll"
  File "\QtSDK\Desktop\Qt\4.7.4\mingw\bin\QtCore4.dll"
  File "\QtSDK\Desktop\Qt\4.7.4\mingw\bin\QtGui4.dll"
  File "\QtSDK\Desktop\Qt\4.7.4\mingw\bin\QtXml4.dll"
  File "\QtSDK\Desktop\Qt\4.7.4\mingw\bin\QtNetwork4.dll"
  
  ; Splitter tool
  File "..\TiffSplit\TiffSplit\bin\Release\TiffSplit.exe"
  File "..\TiffSplit\TiffSplit\bin\Release\BitMiracle.LibTiff.NET.dll"
  File "..\TiffSplit\TiffSplit\bin\Release\BitMiracle.LibTiff.NET.xml"
  
  ; Write the installation path into the registry
  WriteRegStr HKLM SOFTWARE\Map "Install_Dir" "$INSTDIR"
  
  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Map" "DisplayName" "NSIS Map"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Map" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Map" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Map" "NoRepair" 1
  WriteUninstaller "uninstall.exe"
  
SectionEnd

; Optional section (can be disabled by the user)
Section "Start Menu Shortcuts"

  CreateDirectory "$SMPROGRAMS\Map"
  CreateShortCut "$SMPROGRAMS\Map\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\Map\Map.lnk" "$INSTDIR\Map.exe" "" "$INSTDIR\Map.exe" 0
  CreateShortCut "$SMPROGRAMS\Map\TAB Importer.lnk" "$INSTDIR\TiffSplit.exe" "" "$INSTDIR\TiffSplit.exe" 0
  
SectionEnd

;--------------------------------

; Uninstaller

Section "Uninstall"
  
  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Map"
  DeleteRegKey HKLM SOFTWARE\NSIS_Map

  ; Remove files and uninstaller
  Delete $INSTDIR\Map.exe
  Delete $INSTDIR\uninstall.exe
  
  Delete $INSTDIR\mingwm10.dll
  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\QtCore4.dll
  Delete $INSTDIR\QtGui4.dll
  Delete $INSTDIR\QtXml4.dll
  Delete $INSTDIR\QtNetwork4.dll
  
  ; Splitter tool
  Delete $INSTDIR\TiffSplit.exe
  Delete $INSTDIR\BitMiracle.LibTiff.NET.dll
  Delete $INSTDIR\BitMiracle.LibTiff.NET.xml

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Map\*.*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\Map"
  RMDir "$INSTDIR"

SectionEnd
