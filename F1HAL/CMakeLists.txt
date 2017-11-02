cmake_minimum_required(VERSION 2.8)
aux_source_directory(. DIR_HAL)
add_library(haldrv ${DIR_HAL})
target_link_libraries(haldrv stddrv cmsis)
