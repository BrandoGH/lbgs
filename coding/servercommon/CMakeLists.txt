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

set(LIB_NAME servercommon)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#输出路径设置
set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH_BASE_DIR}/${LIB_NAME})

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

add_library(${LIB_NAME} STATIC 
	${CODE_FILE}
)

if (WIN32)
	#link static lib
	target_link_libraries(${LIB_NAME} 
	spdlog
	fmt
	libssl
	libcrypto
	)

elseif(UNIX)
	#link static lib
	target_link_libraries(${LIB_NAME} 
	libspdlog.a
	libfmt.a
	libssl.a
	libcrypto.a
	)
endif()


