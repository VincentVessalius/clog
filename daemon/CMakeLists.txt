include_directories(${PROJECT_SOURCE_DIR})

set(TARGETNAME "clog_test")

aux_source_directory(. DIR_SRCS)
add_executable(${TARGETNAME} ${DIR_SRCS} main_test/main_test.cpp)

target_link_libraries(${TARGETNAME} pthread vinceutil)

#install(TARGETS ${TARGETNAME}
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib
#        RUNTIME DESTINATION bin)