cmake_minimum_required(VERSION 2.8)
add_library(startup startup_stm32f10x_hd.S)
set_target_properties(startup PROPERTIES LINKER_LANGUAGE ASM)
