cmake_minimum_required(VERSION 3.15)
project(mat267cas)

include(GenerateG3A)

fxconv_declare_assets(src/icon.png WITH_METADATA)

add_executable(mat267cas
  src/main.c
  src/eval.c
  src/compute.c
  src/ui.c
  src/fields.c
)

target_compile_options(mat267cas PRIVATE -Wall -Os)
target_link_libraries(mat267cas Gint::Gint m)

generate_g3a(mat267cas OUTPUT "MAT267CAS.g3a"
  NAME "MAT267CAS"
  ICON src/icon.png
)
