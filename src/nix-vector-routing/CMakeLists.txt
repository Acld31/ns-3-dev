build_lib(
  LIBNAME nix-vector-routing
  SOURCE_FILES helper/nix-vector-helper.cc
               model/nix-vector-routing.cc
               model/custom-parent-vector.cc
  HEADER_FILES helper/nix-vector-helper.h
               model/nix-vector-routing.h
               model/custom-parent-vector.h
  LIBRARIES_TO_LINK ${libinternet}
  TEST_SOURCES test/nix-test.cc
)
