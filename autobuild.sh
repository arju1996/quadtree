cd vscodetestbuild
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
.\Release\quad.exe