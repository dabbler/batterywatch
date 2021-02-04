
#define MyManufacturer   "Peregrine"
#define MyDevFolder  	 "C:/proj/BatteryWatch/"
#define MinGWFolder      "C:\Qt\5.6\mingw49_32"
#define QtFolder         "C:\Qt\5.6\mingw49_32\bin"

#define MyAppName		"Battery Watch"
#define MyAppExeName	"BatteryWatch"
#define MyVersion		"30"

[Setup]
AppID={{05AB9398-9399-4AF6-A8E8-1797987A97BE}}
AppName={#MyAppName}
AppVerName={#MyAppName}
AppPublisher=Peregrine
AppPublisherURL=http://www.Peregrine-llc.com/
AppVersion={#MyVersion}
DefaultDirName={commonpf}\Battery Watch
DefaultGroupName={#MyAppName}
UninstallDisplayIcon={app}\{#MyAppExeName}.exe
Compression=lzma
;Compression=none
SolidCompression=yes
OutputBaseFilename=Setup-{#MyAppExeName}-{#MyVersion}
OutputDir=.
VersionInfoVersion={#MyVersion}
VersionInfoDescription={#MyAppName}(Qt 5.13.0)

[Dirs]
Name: "{app}";

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}.exe"; WorkingDir: "{app}"
Name: "{commonstartup}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}.exe"
Name: "{group}\Uninstall"; Filename: "{uninstallexe}"

[Files]
Source: "{#MyDevFolder}/out/paho-mqtt3c.dll"; Excludes: ".svn,media"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyDevFolder}/out/batterywatch.exe"; Excludes: ".svn,media"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
;;Source: "{#MinGWFolder}/plugins/platforms"; Excludes: ".svn,media"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
; Qt platforms
Source: "{#MinGWFolder}/plugins/platforms/qwindows.dll"; DestDir: "{app}/platforms";


[Run]
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent


