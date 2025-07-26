add_executable(toy toy.cpp)

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs core)

target_link_libraries(toy ${llvm_libs})

install(TARGETS toy
  RUNTIME DESTINATION bin
)