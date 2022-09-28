set(CODE_FILE 
main.cpp
./src/mainwindowobj.h
./src/mainwindowobj.cpp
./src/client.h
./src/client.cpp
)

add_definitions(
-DUI_FILE_DIR=\"./ui/\"
)


set(BIN_NAME simulationclient)

#The project directory is the same as the file directory
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#Additional directories -- Header file lookup
include_directories(
	./
	../
	../include
	../servercommon
	${3RD_PATH}
	${3RD_QT5_PATH}
)

#Add library search directory
link_directories(
	${STATIC_RC_LIB_PATH_3RD}
	${SHARED_RC_LIB_PATH_3RD}
	${SERVERCOMMON_LIB}
)

add_executable(${BIN_NAME} 
${CODE_FILE}
)

find_package(Qt5 COMPONENTS core widgets gui network)
if(!Qt5_FOUND)
	message("-- Qt5 found fail!!")
endif()

#link static lib
target_link_libraries(${BIN_NAME} 
	servercommon
	#qt5
	Qt5core
	Qt5Widgets
	Qt5gui
	Qt5NetWork
	Qt5UiTools
)


