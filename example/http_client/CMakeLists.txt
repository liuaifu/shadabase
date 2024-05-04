cmake_minimum_required(VERSION 3.12)
PROJECT(http_client)

aux_source_directory(. DIRSRCS)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

ADD_EXECUTABLE(${PROJECT_NAME} ${DIRSRCS})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
if(WIN32)
	set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost
	1.81.0
	REQUIRED
	COMPONENTS thread url
)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(${PROJECT_NAME} PRIVATE ${Boost_LIBRARIES})
endif(Boost_FOUND)

find_package(OpenSSL
	1.0
	REQUIRED
	COMPONENTS Crypto SSL
)
if(OPENSSL_FOUND)
	include_directories(${OPENSSL_INCLUDE_DIR})
	target_link_libraries(
		${PROJECT_NAME}
		PRIVATE
		OpenSSL::SSL
		OpenSSL::Crypto
	)
endif(OPENSSL_FOUND)

if(UNIX)
	target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
endif()