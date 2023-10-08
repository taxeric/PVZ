cmake_minimum_required(VERSION 3.26)
project(PVZ)

set(CMAKE_CXX_STANDARD 17)
set(EasyX_DIR D:\\c++easyx)
include_directories(${EasyX_DIR}/include)
link_directories(${EasyX_DIR}/lib64)

link_libraries(libEasyX.a libgdi32.a libole32.a)

add_executable(PVZ main.cpp
        m_local_resources.h
        models/Land.h
        models/SunshineBall.h
        models/Zombie.h
        models/Bullet.h
        models/GameStatus.h)
