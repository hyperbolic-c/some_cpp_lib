


find_package(absl
    REQUIRED
    PATHS ~/graDesign/abslSource/install
)

target_link_libraries(hello absl::strings)