cmake_minimum_required(VERSION 2.8)

project(uartBootloader)

IF(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Cross compiling only, Please use -DCMAKE_TOOLCHAIN_FILE=/PATH/TO/TOOLCHAIN_FILE.")
ENDIF(NOT CMAKE_CROSSCOMPILING)

#add_definitions(-DSTM32F1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -mcpu=cortex-m3 -mthumb -nostartfiles -fno-rtti -fno-exceptions -Os -ffunction-sections -fdata-sections -DUSE_MINI_PRINT")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ../../stm32_flash.ld -nostartfiles --specs=rdimon.specs -gc-sections ")

function(add_bin_from_elf bin elf)
    add_custom_target(${bin} ALL ${CMAKE_OBJCOPY} -Obinary ${elf} ${bin} DEPENDS ${elf})
endfunction(add_bin_from_elf)

include_directories(../Libraries/inc)
include_directories(../CMSIS/CM3/DeviceSupport/ST/STM32F10x)
include_directories(../CMSIS/CM3/CoreSupport)
include_directories(../F1HAL)
include_directories(../iap)
include_directories(../Logger)
include_directories(../utility)
include_directories(USER)

#add_subdirectory(../Libraries/src Lib)
add_subdirectory(../CMSIS CMSIS)
add_subdirectory(../Libraries/startup_gcc startup)
add_subdirectory(../F1HAL HAL)
add_subdirectory(../Logger Logger)
add_subdirectory(../iap iap)
add_subdirectory(USER)

#target_link_libraries(app.elf stddrv cmsis startup haldrv)
