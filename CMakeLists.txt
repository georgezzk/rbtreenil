cmake_minimum_required(VERSION 3.20)
project(rbtreenil)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(SOURCES ${PROJECT_NAME}.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Release>:/O2>
        $<$<CONFIG:Release>:/Ob3>
        $<$<CONFIG:Release>:/Oi>
        $<$<CONFIG:Release>:/Ot>
        $<$<CONFIG:Release>:/GL>
        $<$<CONFIG:Release>:/arch:AVX2>
        $<$<CONFIG:Release>:/fp:fast>
        $<$<CONFIG:Release>:/Qvec-report:2>
        $<$<CONFIG:Release>:/Qpar-report:2>
        $<$<CONFIG:Release>:/d2cgsummary>
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Debug>:/Zi>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:/LTCG>
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF>
        $<$<CONFIG:Release>:/verbose:icf>
        $<$<CONFIG:Debug>:/DEBUG>
    )
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /wd4996)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE 
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-march=native>
        $<$<CONFIG:Release>:-mavx2>
        $<$<CONFIG:Release>:-flto>
        $<$<CONFIG:Release>:-funroll-loops>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
    )
    target_link_options(${PROJECT_NAME} PRIVATE
        $<$<CONFIG:Release>:-flto>
        $<$<CONFIG:Release>:-Wl,-O3>
        $<$<CONFIG:Debug>:-g>
    )
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wno-deprecated-declarations)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
)

target_link_directories(${PROJECT_NAME} PRIVATE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
)
