cmake_minimum_required(VERSION 2.8)

project(uartBootloader)

IF(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Cross compiling only, Please use -DCMAKE_TOOLCHAIN_FILE=/PATH/TO/TOOLCHAIN_FILE.")
ENDIF(NOT CMAKE_CROSSCOMPILING)

#add_definitions(-DSTM32F1)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -mcpu=cortex-m3 -mthumb -nostartfiles -DSTM32F10X_HD")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ../../stm32_flash.ld -nostartfiles --specs=rdimon.specs")

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

add_subdirectory(../Libraries)
add_subdirectory(../Libraries/CMSIS)
add_subdirectory(../Libraries/startup_gcc)
add_subdirectory(../F1HAL)
add_subdirectory(../Logger)
add_subdirectory(user)
