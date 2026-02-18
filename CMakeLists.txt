cmake_minimum_required(VERSION 4.1)
project(gbemu C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(gbemu
        cpu.c
        cpu.h
        main.c
        mmu.c
        mmu.h
        rom.c
        rom.h)
