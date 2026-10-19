add_executable(example_lambda_visitor lambda_visitor.cpp)
set_target_properties(example_lambda_visitor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
target_link_libraries(example_lambda_visitor PRIVATE msd_poly_map)
