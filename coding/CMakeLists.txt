cmake_minimum_required (VERSION 3.20)

project (lbgs)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

#common
set(LBGS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(CMAKE_BUILD_TYPE RelWithDebInfo)
SET(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE) #不加这个CMAKE_BUILD_TYPE不起作用
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(EXECUTABLE_OUTPUT_PATH ${LBGS_ROOT_DIR}/build/exe)		#二进制执行文件生成路径[所有服务器生成的文件都放一起]
set(LIBRARY_OUTPUT_PATH_BASE_DIR ${LBGS_ROOT_DIR}/build/library) #库生成根路径
set(CONFIG_BASE_DIR ${LBGS_ROOT_DIR}/config/)

if (WIN32)
	add_definitions(-DWIN_OS)
	set(EXE_DIR ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
	set(3RD_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/include)
	set(SERVERCOMMON_LIB ${LBGS_ROOT_DIR}/build/library/servercommon/${CMAKE_BUILD_TYPE})
	set(STATIC_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-windows/lib)
	set(SHARED_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-windows/bin)
elseif(UNIX)
	add_definitions(-DLINUX_OS)
	set(EXE_DIR ${EXECUTABLE_OUTPUT_PATH})
	set(3RD_PATH ${LBGS_ROOT_DIR}/3rd/x64-linux/include)
	set(SERVERCOMMON_LIB ${LBGS_ROOT_DIR}/build/library/servercommon)
	set(STATIC_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-linux/lib)
endif()

#宏定义
add_definitions(
-DCODE_ROOT_PATH=\"${LBGS_ROOT_DIR}\"
-DLOG_ROOT_PATH=\"${LBGS_ROOT_DIR}/build/log\"
-DCONFIG_BASE_DIR=\"${CONFIG_BASE_DIR}\"

#common
-DHANDLER=private
)

message("----- If you wanna re-cmake,please delete[${LBGS_ROOT_DIR}/build/CMakeCache.txt],otherwise the result may be incorrect")
message("---------------Please make sure you first compile the dependency library in compilation project---------------")
message("---------------you can set add_subdirectory order---------------")
message("CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
message("LBGS_ROOT_DIR : ${LBGS_ROOT_DIR}")
message("3RD_PATH : ${3RD_PATH}")
message("STATIC_RC_LIB_PATH_3RD : ${STATIC_RC_LIB_PATH_3RD}")
message("SHARED_RC_LIB_PATH_3RD : ${SHARED_RC_LIB_PATH_3RD}")
message("EXE_DIR : ${EXE_DIR}")
message("CONFIG_BASE_DIR : ${CONFIG_BASE_DIR}")
message("LIBRARY_OUTPUT_PATH_BASE_DIR : ${LIBRARY_OUTPUT_PATH_BASE_DIR}")


add_subdirectory(./servercommon)
add_subdirectory(./gateserver)

#param传入 导入模拟客户端工程
if (WIN32)
	if(OPEN_SIMULATION_CLIENT)
		
		#qt5
		set(Qt5_INCLUDE_DIR ${3RD_QT5_PATH})
		set(Qt5_DIR ${LBGS_ROOT_DIR}/3rd/x64-windows/share/cmake/Qt5)
		set(CMAKE_AUTOMOC ON)
		set(CMAKE_AUTOUIC ON)

		set(3RD_QT5_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/include/qt5)

		add_subdirectory(./simulationclient)
		message("OPEN_SIMULATION_CLIENT : ${OPEN_SIMULATION_CLIENT}")
	endif()
endif()

