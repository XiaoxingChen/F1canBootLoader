cmake_minimum_required(VERSION 2.8)
aux_source_directory(. DIR_STARTUP)
add_library(stdstart ${DIR_STARTUP})
set_target_properties(startup PROPERTIES LINKER_LANGUAGE ASM)
