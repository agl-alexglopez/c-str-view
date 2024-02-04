file (GLOB PROJ_C_FILES 
  ${CMAKE_SOURCE_DIR}/str_view/*.c 
  ${CMAKE_SOURCE_DIR}/str_view/*.h 
  ${CMAKE_SOURCE_DIR}/tests/*.c
  ${CMAKE_SOURCE_DIR}/tests/*.h
  ${CMAKE_SOURCE_DIR}/samples/*.c
  ${CMAKE_SOURCE_DIR}/samples/*.h
  ${CMAKE_SOURCE_DIR}/str_view/util/*.c
  ${CMAKE_SOURCE_DIR}/str_view/util/*.h
)

add_custom_target (format "clang-format" -i ${PROJ_C_FILES}  COMMENT "Formatting source code...")

foreach (tidy_target ${PROJ_C_FILES})
  get_filename_component (basename ${tidy_target} NAME)
  get_filename_component (dirname ${tidy_target} DIRECTORY)
  get_filename_component (basedir ${dirname} NAME)
  set (tidy_target_name "${basedir}__${basename}")
  set (tidy_command clang-tidy --quiet -p=${PROJECT_BINARY_DIR} ${tidy_target})
  add_custom_target (tidy_${tidy_target_name} ${tidy_command})
  list (APPEND PROJ_TIDY_TARGETS tidy_${tidy_target_name})
endforeach (tidy_target)

add_custom_target (tidy DEPENDS ${PROJ_TIDY_TARGETS})
