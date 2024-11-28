cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.85.0 REQUIRED)
find_package(Boost 1.85.0 COMPONENTS log log_setup program_options)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


add_library(UserStaticLib STATIC
	src/Model/geom.h
	src/Model/collision_detector.cpp
	src/Model/collision_detector.h
	src/Model/constdef.h
	src/Model/dog.h
	src/Model/dog.cpp
	src/Model/dog_gather.h
	src/Model/dog_gather.cpp
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
	src/Logic/tag_invoke_handler.cpp
	src/Logic/tag_invoke_handler.h
)

target_include_directories(UserStaticLib PUBLIC 
	${ZLIB_INCLUDES}
	src/Model
	src/Logic
)
	
add_executable(game_server
	src/main.cpp
	src/Application/application.h
	src/Application/application.cpp
	src/Application/game_session.h
	src/Application/game_session.cpp
	src/Application/player.h
	src/Application/player_data_record.h
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
	src/Logic/logger.h
	src/Logic/logger.cpp
	src/Logic/logging_handler.cpp
	src/Logic/logging_handler.h
	src/Logic/lo_serialize.h
	src/Logic/lo_serialize.cpp
	src/Logic/player_serialize.h
	src/Logic/player_serialize.cpp
	src/Logic/game_session_serialize.h
	src/Logic/game_session_serialize.cpp
	src/Logic/dog_serialize.h
	src/Logic/dog_serialize.cpp
	src/Server/api_request_handler.h
	src/Server/api_request_handler_proxy.h
	src/Server/static_file_request_handler.h
	src/Server/static_file_request_handler_proxy.h
	src/Server/request_handler.h
	src/Server/request_handler_unit.h
	#src/Server/request_handler.cpp
	src/Server/http_server.cpp
	src/Server/http_server.h
	src/Logic/ticker.h
	src/Logic/ticker.cpp
	src/Logic/use_cases_impl.h
	src/Logic/use_cases_impl.cpp
	src/Logic/use_cases.h
	src/Logic/postgres.cpp
	src/Logic/postgres.h
	src/Logic/connection_pool.h
)

target_include_directories(game_server PRIVATE
	src
	src/Application
	src/Logic
	src/Server
)
target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost CONAN_PKG::libpq CONAN_PKG::libpqxx UserStaticLib)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake)

add_executable(game_server_tests
	tests/loot_generator_tests.cpp
)

target_link_libraries(game_server_tests PRIVATE CONAN_PKG::catch2 UserStaticLib)
catch_discover_tests(game_server_tests)

# Boost.Beast будет использовать std::string_view вместо boost::string_view
add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

add_compile_definitions(BOOST_THREAD_PROVIDES_FUTURE)