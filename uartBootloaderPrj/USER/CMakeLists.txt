cmake_minimum_required(VERSION 2.8)
aux_source_directory(. DIR_USER)
add_executable(app.elf ${DIR_USER})
add_bin_from_elf(app.bin app.elf)