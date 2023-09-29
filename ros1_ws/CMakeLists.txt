cmake_minimum_required(VERSION 2.8.3)
project(misc_codes)

find_package(catkin REQUIRED COMPONENTS
    roscpp
)

catkin_package()

include_directories( include
    ${catkin_INCLUDE_DIRS}
)

add_execute(parrot src/publisher_subscriber_demo.cpp)

target_link_libraries(parrot ${catkin_LIBRARIES})