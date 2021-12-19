set(CODE_FILE 
main.cpp
./src/mainwidget.h
./src/mainwidget.cpp
./src/connectwidget/connectwidget.h
./src/connectwidget/connectwidget.cpp
./src/connectwidget/user.h
./src/connectwidget/user.cpp

)

#工程目录和文件目录一致
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#附加目录 头文件查找
include_directories(
	./
	../
	../servercommon
	${3RD_PATH}
	${3RD_QT5_PATH}
)

#添加库搜索目录
link_directories(
	${STATIC_RC_LIB_PATH_3RD}
	${SHARED_RC_LIB_PATH_3RD}
	${SERVERCOMMON_LIB}
)

add_executable(simulationclient 
${CODE_FILE}
)

find_package(Qt5 COMPONENTS core widgets gui network)
if(!Qt5_FOUND)
	message("-- Qt5 found fail!!")
endif()


#link shared lib
foreach(
	shared_name
	Qt5Core
	Qt5Widgets
	Qt5Gui
	icuin69
	harfbuzz
	icuuc69
	pcre2-16
	zstd
	freetype
	icudt69
	bz2
	brotlidec
	brotlicommon
)
	if(EXISTS ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll)
		file(COPY ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll DESTINATION ${EXE_DIR})
	else()
		message("-- ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll not found")
	endif()

	if(EXISTS ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb)
		file(COPY ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb DESTINATION ${EXE_DIR})
	else()
		message("-- ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb not found")
	endif()
endforeach()

#link static lib
target_link_libraries(simulationclient
	servercommon
	#qt5
	Qt5core
	Qt5Widgets
	Qt5gui
	Qt5NetWork
)


