set(SRCS 
    Main.cpp
    Timer.cpp 
    Util.cpp 
)

include_directories(${PROJECT_SOURCE_DIR}/base)


add_executable(Kara ${SRCS})
target_link_libraries(Kara libserver_base)


add_subdirectory(base)
add_subdirectory(tests)