# Using OpenCV with gcc and CMake
1. Create a CMake file(CMakeLists.txt).

Example:
```
cmake_minimum_required(VERSION 2.8)
project( DisplayImage )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( DisplayImage DisplayImage.cpp )
target_link_libraries( DisplayImage ${OpenCV_LIBS} )
```

2. Generate executable.

Example:
```
cd <code_directory>
cmake .
make
```
Alternatively, using gcc command,
```
cd <code_directory>
cmake .
gcc <file_name>.cpp -lstdc++ `pkg-config --libs --cflags opencv` -o <output_name>
```

Link: https://docs.opencv.org/3.3.0/db/df5/tutorial_linux_gcc_cmake.html
