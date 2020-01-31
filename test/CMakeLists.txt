enable_testing()

find_package(cmocka REQUIRED)

add_executable(test_http
        test_http.c
        test_http.h)

target_link_libraries(test_http
        ${CMOCKA_LIBRARY})

add_test(test_http test_http)
