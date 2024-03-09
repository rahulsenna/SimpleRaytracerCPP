@echo off
set "workspaceFolder=%CD%"

set Name=SimpleRaytracer

if not exist bin mkdir bin

set Definitions=
@REM set CommonCompilerFlags=%Definitions% /nologo /FC /WX /W4 /wd4127 /wd4100 /wd4089 /wd4068 /wd4505 /diagnostics:column /wd4456 /wd4201 /wd4100 /wd4505 /wd4189 /wd4457 ^
set CommonCompilerFlags=%Definitions% /nologo /EHsc /FC /diagnostics:column /wd4456 /wd4201 /wd4100 /wd4505 /wd4189 /wd4457 ^
/MTd /Oi /Ox /GR- /Gm- /EHa- /Zi
@REM /MTd /Oi /Ox /GR- /Gm- /EHa- /Zi

set CommonLinkerFlags=-incremental:no -opt:ref user32.lib gdi32.lib winmm.lib

cl /std:c++latest %CommonCompilerFlags% /Fobin\%Name%.obj  /Febin\%Name%.exe src\main.cpp /link %CommonLinkerFlags%
@REM cl /std:c++latest %CommonCompilerFlags% /Fd%workspaceFolder%\bin\%Name%2.pdb   /Fe%workspaceFolder%\bin\%Name%.exe %workspaceFolder%\src\main.cpp %workspaceFolder%\src\Model.cpp /link %CommonLinkerFlags%
