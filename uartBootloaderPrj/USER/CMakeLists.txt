cmake_minimum_required(VERSION 2.8)
aux_source_directory(. DIR_USER)
set(DIR_USER ${DIR_USER} ../../iap/uartIapDevice.cpp)
set(DIR_STDDRV
    ../../Libraries/src/misc.c
    ../../Libraries/src/stm32f10x_gpio.c
    ../../Libraries/src/stm32f10x_rcc.c
    ../../Libraries/src/stm32f10x_dma.c
    ../../Libraries/src/stm32f10x_usart.c
    ../../Libraries/src/stm32f10x_flash.c
)
add_library(stddrv ${DIR_STDDRV})
add_executable(app.elf ${DIR_USER})
target_link_libraries(app.elf iap stddrv logger cmsis haldrv)
add_bin_from_elf(app.bin app.elf)
