cmake_minimum_required(VERSION 3.8)
project(fst)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp worker/worker.cpp worker/worker.h worker/clientmanager/client/client.cpp worker/clientmanager/client/client.h worker/clientmanager/client/request/request.cpp worker/clientmanager/client/request/request.h worker/clientmanager/clientmanager.cpp worker/clientmanager/clientmanager.h)
add_executable(fst ${SOURCE_FILES})