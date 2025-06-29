cmake_minimum_required(VERSION 3.31)
project(MIniProyecto_1)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(MIniProyecto_1
        environment.hh
        exp.cpp
        exp.h
        gencode.cpp
        gencode.hh
        imp_interpreter.cpp
        imp_interpreter.hh
        imp_type.cpp
        imp_type.hh
        imp_value.cpp
        imp_value.hh
        imp_value_visitor.hh
        main.cpp
        parser.cpp
        parser.h
        scanner.cpp
        scanner.h
        token.cpp
        token.h
        type_visitor.cpp
        type_visitor.hh
        visitor.cpp
        visitor.h)
