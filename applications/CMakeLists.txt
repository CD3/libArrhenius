cmake_minimum_required( VERSION 3.1 )

project(Arrenius-cli)
find_package( Boost REQUIRED COMPONENTS program_options )

add_executable( Arrhenius-cli Arrhenius-cli.cpp )
target_link_libraries( Arrhenius-cli libArrhenius::Arrhenius Boost::program_options )
set_property( TARGET Arrhenius-cli PROPERTY CXX_STANDARD 11 )

add_subdirectory( .. libArrhenius)
