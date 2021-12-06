include(CMakeFindDependencyMacro)
find_dependency(ZLIB)
find_dependency(OpenSSL)
include("${CMAKE_CURRENT_LIST_DIR}/unofficial-libmysql-targets.cmake")
