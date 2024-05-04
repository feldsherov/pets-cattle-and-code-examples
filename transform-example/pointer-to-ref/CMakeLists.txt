set(LLVM_LINK_COMPONENTS
  FrontendOpenMP
  Support
  )

add_clang_executable(pointer-to-ref
  PointerToRef.cpp
  )

clang_target_link_libraries(pointer-to-ref
  PRIVATE
  clangAST
  clangASTMatchers
  clangBasic
  clangTransformer
  clangFrontend
  clangTooling
  clangToolingRefactoring
  )
