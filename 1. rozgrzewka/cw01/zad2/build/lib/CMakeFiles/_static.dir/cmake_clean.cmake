file(REMOVE_RECURSE
  "lib_static.pdb"
  "lib_static.a"
)

# Per-language clean rules from dependency scanning.
foreach(lang C)
  include(CMakeFiles/_static.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
