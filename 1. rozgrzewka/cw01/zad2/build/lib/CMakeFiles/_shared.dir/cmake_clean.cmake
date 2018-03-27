file(REMOVE_RECURSE
  "lib_shared.pdb"
  "lib_shared.so"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/_shared.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
