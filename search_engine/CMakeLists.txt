cmake_minimum_required(VERSION 3.15)
project(SearchEngineProject)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Create a library target for WebPage
add_library(WebPage
    WebPage.h
    WebPage.cpp
)

# Create a library target for SearchEngine
add_library(SearchEngine
    SearchEngine.h
    SearchEngine.cpp
)

# Link WebPage library to SearchEngine library
target_link_libraries(SearchEngine PRIVATE WebPage)

# Create an executable target for search_engine
add_executable(search_engine SearchEngineMain.x.cpp)

# Link SearchEngine library to search_engine executable
target_link_libraries(search_engine PRIVATE SearchEngine)