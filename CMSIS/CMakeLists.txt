cmake_minimum_required(VERSION 2.8)
aux_source_directory(./CM3/CoreSupport DIR_CMSIS)
aux_source_directory(./CM3/DeviceSupport/ST/STM32F10x DIR_CMSIS)
add_library(libcmsis ${DIR_CMSIS})