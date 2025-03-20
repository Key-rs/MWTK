cmake_minimum_required(VERSION 3.22)

macro(FIND_INCLUDE_DIR result curdir)									#定义函数,2个参数:存放结果result；指定路径curdir；
    file(GLOB_RECURSE children "${curdir}/*.hpp" "${curdir}/*.h" )		#遍历获取{curdir}中*.hpp和*.h文件列表
    set(dirlist "")														#定义dirlist中间变量，并初始化
    foreach(child ${children})											#for循环
        string(REGEX REPLACE "(.*)/.*" "\\1" LIB_NAME ${child})			#字符串替换,用/前的字符替换/*h
        if(IS_DIRECTORY ${LIB_NAME})									#判断是否为路径
            list (FIND dirlist ${LIB_NAME} list_index)					#去重，查找dirlist中是否有${LIB_NAME}指定的值，可以区分字符串相同数子后缀不同的路径：例如/app/test_1和/app/test_2
            if(${list_index} LESS 0)									#若没找到则代表列表中没有该路径
                LIST(APPEND dirlist ${LIB_NAME})						#将合法的路径加入dirlist变量中
            endif()														#结束判断
        endif()
    endforeach()														#结束for循环
    set(${result} ${dirlist})											#dirlist结果放入result变量中
endmacro()

# 查找所有源文件和头文件
file(GLOB_RECURSE JUSTFW_SOURCES
  "lib/*.c"
  "lib/*.h"
  "interface/*.c"
  "interface/*.h"
  "modules/*.c"
  "modules/*.h"
  "sys/*.c"
  "sys/*.h"
  "app/*.c"
  "app/*.h"
)

add_definitions(-DARM_MATH_CM4)

# 打印 INCLUDES 变量
# message(STATUS "JUSTFW_SOURCES: ${JUSTFW_SOURCES}")

# include_directories(${INCLUDES})
include_directories(interface)

# 如果有其他依赖库，可以在这里链接
# target_link_libraries(justFW PRIVATE other_library)

add_library(justFW STATIC ${JUSTFW_SOURCES})

# 链接 stm32cubemx 库
target_link_libraries(justFW PRIVATE stm32cubemx)


# 查找并添加 include 目录
FIND_INCLUDE_DIR(INCLUDES modules)
target_include_directories(justFW PUBLIC ${INCLUDES})
# message(STATUS "INCLUDES: ${INCLUDES}")

FIND_INCLUDE_DIR(INCLUDES lib)
target_include_directories(justFW PUBLIC ${INCLUDES})
# message(STATUS "INCLUDES: ${INCLUDES}")

FIND_INCLUDE_DIR(INCLUDES sys)
target_include_directories(justFW PUBLIC ${INCLUDES})

FIND_INCLUDE_DIR(INCLUDES config)
target_include_directories(justFW PUBLIC ${INCLUDES})


FIND_INCLUDE_DIR(INCLUDES app)
target_include_directories(justFW PUBLIC ${INCLUDES})

target_include_directories(justFW PUBLIC interface)
# target_include_directories(justFW PRIVATE config)