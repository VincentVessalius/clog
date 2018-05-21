include_directories(${PROJECT_SOURCE_DIR})

set(TARGETNAME "vinceutil")

aux_source_directory(. DIR_SRCS)
#set(DIR_SRCS Vin_Task.cpp Vin_Thread.cpp Vin_Exception.cpp log.cpp tools.cpp)

add_library(${TARGETNAME} STATIC ${DIR_SRCS} Vin_Endpoint.cpp ../include/Vin_Endpoint.h Vin_ClientSocket.cpp ../include/Vin_ClientSocket.h Vin_Socket.cpp ../include/Vin_Socket.h Vin_Epoller.cpp ../include/Vin_Epoller.h)

#install(TARGETS ${TARGETNAME}
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)