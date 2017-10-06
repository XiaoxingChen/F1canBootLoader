cmake_minimum_required(VERSION 2.8)
aux_source_directory(. DIR_LOG)
add_library(logger ${DIR_LOG})