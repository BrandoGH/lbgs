set(CODE_FILE 
main.cpp
./src/gateserver.h
./src/gateserver.cpp
./src/user.h
./src/user.cpp
./config/config.h
./config/config.cpp
)

#工程目录和文件目录一致
source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#附加目录 头文件查找
include_directories(
	./
	../
	../servercommon
	${3RD_PATH}
)

#添加库搜索目录
link_directories(
	${STATIC_RC_LIB_PATH_3RD}
	${SHARED_RC_LIB_PATH_3RD}
	${SERVERCOMMON_LIB}
)

add_executable(gateserver 
${CODE_FILE}
)

if (WIN32)
	#link shared lib
	foreach(
	shared_name
	#boost
	boost_thread-vc141-mt-x64-1_75
	boost_regex-vc141-mt-x64-1_75
	)
		if(EXISTS ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll)
			file(COPY ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll DESTINATION ${EXE_DIR})
		else()
			message("${SHARED_RC_LIB_PATH_3RD}/${shared_name}.dll not found")
		endif()

		if(EXISTS ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb)
			file(COPY ${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb DESTINATION ${EXE_DIR})
		else()
			message("${SHARED_RC_LIB_PATH_3RD}/${shared_name}.pdb not found")
		endif()
	endforeach()

	#link static lib
	target_link_libraries(gateserver 
	#servercommon
	servercommon
	#boost
	boost_thread-vc140-mt
	boost_regex-vc140-mt
	)

elseif(UNIX)
	#link static lib
	target_link_libraries(gateserver 
	#servercommon
	libservercommon.a
	#boost
	libboost_thread.a
	libboost_regex.a
	libspdlog.a
	libfmt.a
	)
endif()
