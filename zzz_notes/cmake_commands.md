cmake -B build -G "Visual Studio 18 2026" -DCMAKE_PREFIX_PATH="F:/installed_tools/raylib/raylib/install_debug/"  -DBUILD_EXAMPLES=ON  -DQUAD_BUILD_UI=ON -DQUAD_BUILD_TESTS=OFF -DRAY_LIB_GUI_WIN_FOLDER="F:/installed_tools/raylib/raygui/src"

cmake --build build --config Debug --parallel

Remove-Item -Recurse -Force build