GSOpenGL
========

Minimal CMake project demonstrating OpenGL 4.6 with GLFW, GLAD, and ImGui using CMake's FetchContent.

Build (PowerShell):

```powershell
mkdir build; cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
```

Or use the default generator:

```powershell
mkdir build; cd build
cmake ..
cmake --build .
```

Notes:
- Requires CMake 3.14+ for FetchContent and a compiler supporting C++17.
- Requests OpenGL 4.6 core profile via GLFW hints.

Using LLVM/Clang on Windows
---------------------------

If you have LLVM installed at `C:\Program Files\LLVM\bin` (clang/clang-cl), you can force CMake to use `clang-cl` when configuring. Example from PowerShell:

```powershell
# Use clang-cl explicitly (single configure)
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_C_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe" -DCMAKE_CXX_COMPILER="C:/Program Files/LLVM/bin/clang-cl.exe"
cmake --build build --config Release
```

The `CMakeLists.txt` included also tries to auto-detect `clang-cl` at `C:/Program Files/LLVM/bin/clang-cl.exe` and set it as the compiler if CMake hasn't been given compilers explicitly.

