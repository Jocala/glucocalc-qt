[Setup]
AppName=Glucocalc
AppVersion=2.0
AppPublisher=jocala
AppPublisherURL=https://jocala.com
AppId={{com.jocala.glucocalc}}
DefaultDirName={autopf}\Glucocalc
DefaultGroupName=Glucocalc
OutputDir=C:\Users\jeff\build-glucocalc
OutputBaseFilename=glucocalc-2.0-Windows-Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern
SetupIconFile=C:\source\glucocalc\resources\AppIcon.ico
UninstallDisplayIcon={app}\glucocalc.exe
LicenseFile=C:\source\glucocalc\LICENSE
VersionInfoVersion=2.0
VersionInfoCompany=jocala
VersionInfoDescription=Glucose/HbA1c calculator
VersionInfoProductName=Glucocalc

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "C:\Users\jeff\build-glucocalc\glucocalc.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\source\glucocalc\resources\AppIcon.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Glucocalc"; Filename: "{app}\glucocalc.exe"; IconFilename: "{app}\AppIcon.ico"
Name: "{group}\Uninstall Glucocalc"; Filename: "{uninstallexe}"
Name: "{autodesktop}\Glucocalc"; Filename: "{app}\glucocalc.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\glucocalc.exe"; Description: "{cm:LaunchProgram,Glucocalc}"; Flags: nowait postinstall skipifsilent
