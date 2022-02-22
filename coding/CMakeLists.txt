cmake_minimum_required (VERSION 3.20)

set(PROJECT_NAME lbgs)
project (${PROJECT_NAME})

message("-- current system info: ${CMAKE_SYSTEM_NAME} : ${CMAKE_SYSTEM_VERSION}")


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-std=c++14 \
-pthread \
"
)
message("-- CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")

#cmake 参数设置的自定义变量
#{  -- start
#通用
#ENABLE_DEBUG_LOG		ON/OFF			是否开启调试日志，如果开启，日志将在屏幕打出
#ENABLE_UNITEST			ON/OFF			是否导入单元测试工程

#windows
#OPEN_SIMULATION_CLIENT		ON/OFF		是否导入模拟客户端工程

#linux
#ENABLE_DEBUG_MODE			ON/OFF		是否开启可调式模式
#}  -- end

#common cmake
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CONFIGURATION_TYPES ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE) #不加这个CMAKE_BUILD_TYPE不起作用
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(LBGS_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(EXECUTABLE_OUTPUT_PATH ${LBGS_ROOT_DIR}/build/exe)		#二进制执行文件生成路径[所有服务器生成的文件都放一起]
set(LIBRARY_OUTPUT_PATH_BASE_DIR ${LBGS_ROOT_DIR}/build/library) #库生成根路径
set(CONFIG_BASE_DIR ${LBGS_ROOT_DIR}/config/)

if (WIN32)
	message("-- MSVC_VERSION : ${MSVC_VERSION}")
	add_definitions(-DWIN_OS)
	#visual studio优化选项，release也可调试【本质上复用了debug的值】【win 开发机默认可调试】
	#如果是 visual studio 2022 添加代码可热更选项
	if(MSVC_VERSION EQUAL 1930)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} /ZI /JMC")
		message("-- visual studio version is 2022, support hot reload !!")
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG}")
	endif()
	set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
	set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")

	set(EXE_DIR ${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE})
	set(3RD_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/include)
	set(SERVERCOMMON_LIB ${LBGS_ROOT_DIR}/build/library/servercommon/${CMAKE_BUILD_TYPE})
	set(STATIC_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-windows/lib)
	set(SHARED_RC_LIB_PATH_3RD ${LBGS_ROOT_DIR}/3rd/x64-windows/bin)

	#拷贝需要的动态库到可执行文件夹下【只适用于windows】
	file(GLOB DLL_PATH ${SHARED_RC_LIB_PATH_3RD}/*)
	file(COPY ${DLL_PATH} DESTINATION ${EXE_DIR})
	message("-- 3rd dll has copy to ${EXE_DIR}")

	file(GLOB BAT_SCRIPT_START_SERVER ${LBGS_ROOT_DIR}/coding/startserver.bat)
	file(COPY ${BAT_SCRIPT_START_SERVER} DESTINATION ${EXE_DIR})
	message("-- ${BAT_SCRIPT_START_SERVER} has copy to ${EXE_DIR}")

	file(GLOB BAT_SCRIPT_VSGEN ${LBGS_ROOT_DIR}/coding/vsgen.bat)
	file(COPY ${BAT_SCRIPT_VSGEN} DESTINATION ${LBGS_ROOT_DIR}/build)
	message("-- ${BAT_SCRIPT_VSGEN} script has copy to ${LBGS_ROOT_DIR}/build")

elseif(UNIX)
	add_definitions(-DLINUX_OS)
	#服务器按需设置,开启debug可调试，默认最大优化
	if(ENABLE_DEBUG_MODE)
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG}")
		set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_DEBUG}")
		set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
    endif()
	message("-- LINUX_LBGS_ENABLE_DEBUG_MODE : ${ENABLE_DEBUG_MODE}")

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
-DEXE_DIR=\"${EXE_DIR}\"

#common
-DHANDLER=private
-DSIGNALS=public
-DSLOTS=public
)

if(ENABLE_DEBUG_LOG)
	add_definitions(-DDLOG)
endif()
message("-- ENABLE_DEBUG_LOG : ${ENABLE_DEBUG_LOG}")


message("-- If you wanna re-cmake,please delete[${LBGS_ROOT_DIR}/build/CMakeCache.txt],otherwise the result may be incorrect")
message("-- Please make sure you first compile the dependency library in compilation project")
message("-- you can set add_subdirectory order")
message("-- CMAKE_BUILD_TYPE : ${CMAKE_BUILD_TYPE}")
message("-- LBGS_ROOT_DIR : ${LBGS_ROOT_DIR}")
message("-- 3RD_PATH : ${3RD_PATH}")
message("-- STATIC_RC_LIB_PATH_3RD : ${STATIC_RC_LIB_PATH_3RD}")
message("-- SHARED_RC_LIB_PATH_3RD : ${SHARED_RC_LIB_PATH_3RD}")
message("-- EXE_DIR : ${EXE_DIR}")
message("-- CONFIG_BASE_DIR : ${CONFIG_BASE_DIR}")
message("-- LIBRARY_OUTPUT_PATH_BASE_DIR : ${LIBRARY_OUTPUT_PATH_BASE_DIR}")
message("-- CMAKE_CXX_FLAGS_RELEASE : ${CMAKE_CXX_FLAGS_RELEASE}")
message("-- CMAKE_EXE_LINKER_FLAGS_RELEASE : ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
message("-- CMAKE_SHARED_LINKER_FLAGS_RELEASE : ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")

#顺序比较重要，先编译通用库
add_subdirectory(./servercommon)
add_subdirectory(./gateserver)
add_subdirectory(./proxyserver)

if(ENABLE_UNITEST)
	add_subdirectory(./test)
	message("-- ENABLE_UNITEST_PROJECT : ${ENABLE_UNITEST}")
endif()

#param传入 导入模拟客户端工程
if (WIN32)
	if(OPEN_SIMULATION_CLIENT)
		
		#qt5
		set(Qt5_INCLUDE_DIR ${3RD_QT5_PATH})
		set(Qt5_DIR ${LBGS_ROOT_DIR}/3rd/x64-windows/share/cmake/Qt5)
		set(CMAKE_AUTOMOC ON)
		set(CMAKE_AUTOUIC ON)
		set(3RD_QT5_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/include/qt5)
		#需要把qt的插件放到exe目录下，不然启动会有问题
		file(GLOB QT_PLUGIN_PATH ${LBGS_ROOT_DIR}/3rd/x64-windows/tools/qt5/bin/plugins/*)
		file(COPY ${QT_PLUGIN_PATH} DESTINATION ${EXE_DIR})

		add_subdirectory(./simulationclient)
		message("-- OPEN_SIMULATION_CLIENT : ${OPEN_SIMULATION_CLIENT}")
	endif()
endif()