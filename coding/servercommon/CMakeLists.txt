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
./configmodule/gateserverconfig/gateserverconfig.h
./configmodule/gateserverconfig/gateserverconfig.cpp
./configmodule/proxyserverconfig/proxyserverconfig.h
./configmodule/proxyserverconfig/proxyserverconfig.cpp
./configmodule/cacheserverconfig/cacheserverconfig.h
./configmodule/cacheserverconfig/cacheserverconfig.cpp
./sysinfomodule/sysinfo.h
./sysinfomodule/sysinfo.cpp
./sysinfomodule/minidump/minidump.h
./sysinfomodule/minidump/minidump.cpp
./msgmodule/msgcommondef.h
./msgmodule/proxyservermsghandler.h
./msgmodule/proxyservermsghandler.cpp
./msgmodule/singletoproxymsghandler.h
./msgmodule/singletoproxymsghandler.cpp
./msgmodule/innerserverheartmsg.h
./commontool/commontool.h
./commontool/commontool.cpp
./commontool/msgtool/msgtool.h
./commontool/msgtool/msgtool.cpp
./timermodule/timer.h
./timermodule/timer.cpp
./queuemodule/safequeue.hpp
)

set(LIB_NAME servercommon)

source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${CODE_FILE})

#set lib output path
set(LIBRARY_OUTPUT_PATH ${LIBRARY_OUTPUT_PATH_BASE_DIR}/${LIB_NAME})

#Additional directories -- Header file lookup
include_directories(
	./
	../
	${3RD_PATH}
)

#Add library search directory
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
	boost_thread-vc140-mt
	boost_filesystem-vc140-mt
	spdlog
	fmt
	libssl
	libcrypto
	bcrypt
	dbghelp
	)

elseif(UNIX)
	#link static lib
	target_link_libraries(${LIB_NAME} 
	libspdlog.a
	libfmt.a
	libssl.a
	libcrypto.a
	libbcrypt.a
	libboost_filesystem.a
	)
endif()


