add_library(SpargelEngine STATIC)
target_sources(SpargelEngine
  PUBLIC
    ecs/entity_manager.h
  PRIVATE
    ecs/entity_manager.cc
)
target_include_directories(SpargelEngine
  PUBLIC .
)
target_compile_features(SpargelEngine
  PUBLIC cxx_std_20
)
