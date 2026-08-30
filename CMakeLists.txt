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
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
)

target_link_directories(${PROJECT_NAME} PRIVATE
)

target_link_libraries(${PROJECT_NAME} PRIVATE
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /wd4996)
endif()