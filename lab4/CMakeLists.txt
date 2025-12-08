cmake_minimum_required(VERSION 3.10)
project(lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_library(lib1 SHARED
    lib1/PiLeibniz.cpp
    lib1/BubbleSort.cpp
)
target_include_directories(lib1 PUBLIC include)

add_library(lib2 SHARED
    lib2/PiWallis.cpp
    lib2/QuickSort.cpp
)
target_include_directories(lib2 PUBLIC include)

# статическая линковка
add_executable(Program1 prog1/main.cpp)
target_link_libraries(Program1 lib1)  # выбрали навсегда 1
target_include_directories(Program1 PUBLIC include)

# динамическая 
add_executable(Program2
    prog2/main.cpp
    prog2/DynamicLoader.cpp
)
target_include_directories(Program2 PUBLIC include)
target_link_libraries(Program2)  # нафинг не линкуем статически

# копируем DLL рядом с Program2
add_custom_command(TARGET Program2 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:lib1>
        $<TARGET_FILE_DIR:Program2>/lib1.dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:lib2>
        $<TARGET_FILE_DIR:Program2>/lib2.dll
)