set(base_SRCS
        StringPiece.cpp
        asyncLogging.cpp
        countDownLatch.cpp
        exception.cpp
        file.cpp
        logFile.cpp
        logStream.cpp
        logging.cpp
        thread.cpp
        threadPool.cpp
        timeStamp.cpp
        timeZone.cpp)

#message(STATUS "CMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}") # 其路径默认情况下是/usr/local

add_library(klib_base ${base_SRCS})
target_link_libraries(klib_base pthread ${Boost_LIBRARY})

add_library(klib_base_cc11 ${base_SRCS})
target_link_libraries(klib_base_cc11 pthread ${Boost_LIBRARY})
set_target_properties(klib_base_cc11 PROPERTIES COMPILE_FLAGS "-std=c++0x")  # 设置编译选项

# 参考学习: https://blog.csdn.net/yangfeng2014/article/details/50638601
# 执行make命令的时候无效只有在执行make install 命令的时候才有效的执行install(......)
install(TARGETS klib_base DESTINATION lib) # 其目的就是将库文件安装在${CMAKE_INSTALL_PREFIX}/lib 路径下面
install(TARGETS klib_base_cc11 DESTINATION lib) #

file(GLOB HEADFILES "*.h")  # 当前base 目录下全局搜索*.h 文件 并且放入变量HEADFILES 中
install(FILES ${HEADFILES} DESTINATION include/klib/base) # 普通文件安装

add_subdirectory(baseTest)