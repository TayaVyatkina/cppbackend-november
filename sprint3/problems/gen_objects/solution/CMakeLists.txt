cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
find_package(Boost 1.78.0 COMPONENTS log log_setup program_options)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(game_server
	src/main.cpp
	src/Application/application.h
	src/Application/application.cpp
	src/Application/game_session.h
	src/Application/game_session.cpp
	src/Application/player.h
	src/Application/player.cpp
	src/Application/tokens.h
	src/Application/tokens.cpp
	src/Logic/command.h
	src/Logic/command.cpp
	src/Logic/error_handler.h
	src/Logic/error_handler.cpp
	src/Logic/file_system.cpp
    src/Logic/file_system.h
	src/Logic/json_loader.h
	src/Logic/json_loader.cpp
	src/Logic/json_handler.h
	src/Logic/json_handler.cpp
	src/Logic/boost_json.cpp
	src/Logic/tag_invoke_handler.cpp
	src/Logic/tag_invoke_handler.h
	src/Logic//logger.h
	src/Logic/logger.cpp
	src/Logic/logging_handler.cpp
	src/Logic/logging_handler.h
	src/Model/constdef.h
	src/Model/dog.h
	src/Model/dog.cpp
	src/Model/game.h
	src/Model/game.cpp
	src/Model/map.cpp
	src/Model/map.h
	src/Model/road.h
	src/Model/road.cpp
	src/Model/road_layout.h
	src/Model/road_layout.cpp
	src/Model/typedef.h
	src/Model/typedef.cpp
	src/Model/loot_generator.h
	src/Model/loot_generator.cpp
	src/Model/lost_obj.h
	src/Model/lost_obj.cpp
	src/Logic/sdk.h
	src/Logic/tagged.h
	src/Logic/utils.h
	src/Logic/utils.cpp
	src/Server/api_request_handler.h
	src/Server/api_request_handler_proxy.h
	src/Server/static_file_request_handler.h
	src/Server/static_file_request_handler_proxy.h
	src/Server/request_handler.h
	src/Server/request_handler_unit.h
	src/Server/request_handler.cpp
	src/Server/http_server.cpp
	src/Server/http_server.h
	src/Logic/ticker.h
	src/Logic/ticker.cpp

)

target_include_directories(game_server PRIVATE
	src
	src/Application
	src/Logic
	src/Model
	src/Server

)
target_link_libraries(game_server PRIVATE CONAN_PKG::catch2) 
target_link_libraries(game_server PRIVATE Threads::Threads Boost::log Boost::log_setup Boost::program_options)

# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)
