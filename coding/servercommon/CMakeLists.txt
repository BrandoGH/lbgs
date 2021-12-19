set(CODE_FILE 
./basedef.h
./boostmodule/basedef.h
./boostmodule/signalcommunication.h
./logmodule/log.h
./logmodule/log.cpp
./logmodule/logdef.h
./configmodule/configinterface.h
./configmodule/configinterface.cpp
./configmodule/configmanager.h
./configmodule/configmanager.cpp
./sysinfomodule/sysinfo.h
./sysinfomodule/sysinfo.cpp
./msgmodule/msgcommondef.h
./commontool/commontool.h
./commontool/commontool.cpp
./commontool/msgtool/msgtool.h
./commontool/msgtool/msgtool.cpp
)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#输出路径设置
set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH_BASE_DIR}/servercommon)

#附加目录 头文件查找
include_directories(
	./
	../
	${3RD_PATH}
)

#添加库搜索目录
link_directories(
	${STATIC_RC_LIB_PATH_3RD}
	${SHARED_RC_LIB_PATH_3RD}
	${SERVERCOMMON_LIB}
)

add_library(servercommon STATIC 
	${CODE_FILE}
)

if (WIN32)
	#link shared lib
	foreach(
	shared_name
	spdlog
	fmt
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
	target_link_libraries(servercommon 
	boost_thread-vc140-mt
	#log
	spdlog
	fmt
	)

elseif(UNIX)
	#link static lib
	target_link_libraries(servercommon 
	libspdlog.a
	libfmt.a
	)
endif()


