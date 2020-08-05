function(set_project_warnings project_name)
  option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" FALSE)
  set(CLANG_WARNINGS
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a parent context
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      #-Wconversion # warn on type conversions that may lose data
      #-Wsign-conversion # warn on sign conversions
      #-Wnull-dereference # warn if a null dereference is detected
      #-Wdouble-promotion # warn if float is implicit promoted to double
      #-Wformat=2 # warn on security issues around functions that format output (ie printf)
  )

  if(WARNINGS_AS_ERRORS)
    set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
  endif()

  set(GCC_WARNINGS
      ${CLANG_WARNINGS}
      -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
      -Wduplicated-cond # warn if if / else chain has duplicated conditions
      -Wduplicated-branches # warn if if / else branches have duplicated code
      -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
  )

  target_compile_options(${project_name} PRIVATE ${GCC_WARNINGS} ${CLANG_WARNINGS})

endfunction()
