; rlcl_installer.iss
[Setup]
AppName=RedLineChargeLimits
AppVersion=1.2
AppPublisher=Santos Santiago
AppUpdatesURL=https://github.com/santiagosantos08/RLCL/releases/
DefaultDirName={pf}\Redline\ChargeLimits
DefaultGroupName=RedLine
UninstallDisplayIcon={app}\rlcl.exe
UninstallDisplayName=UninstallRLCL
OutputDir=.
OutputBaseFilename=rlcl_setup
Compression=lzma
SolidCompression=yes
DisableProgramGroupPage=yes
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "rlcl.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "icon.ico"; DestDir: "{app}"; Flags: ignoreversion

[Run]
; Download and install the Lenovo CLI tool
Filename: "{cmd}"; Parameters: "/C powershell -Command ""Invoke-WebRequest -Uri https://download.lenovo.com/pccbbs/thinkvantage_en/metroapps/Vantage/ChargeThreshold/ChargeThreshold.exe -OutFile '{app}\\ChargeThreshold.exe'"""; StatusMsg: "Downloading Lenovo CLI tool..."; Flags: runhidden

; Launch app on install complete
Filename: "{app}\rlcl.exe"; Description: "Launch RLCL now"; Flags: postinstall skipifsilent

[Icons]
Name: "{group}\RLCL"; Filename: "{app}\rlcl.exe"; IconFilename: "{app}\icon.ico"
Name: "{commondesktop}\RLCL"; Filename: "{app}\rlcl.exe"; Tasks: desktopicon; IconFilename: "{app}\icon.ico"
Name: "{userstartup}\RLCL"; Filename: "{app}\rlcl.exe"; Tasks: runatboot; IconFilename: "{app}\icon.ico"

[Tasks]
Name: "desktopicon"; Description: "Create a &desktop shortcut"; GroupDescription: "Additional Options"
Name: "runatboot"; Description: "Launch RLCL at Windows startup"; GroupDescription: "Additional Options"

[Code]
function InitializeSetup(): Boolean;
begin
  Result := True;
end;