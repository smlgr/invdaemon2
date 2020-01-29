add_executable(invdaemon2
        invdaemon2.c
        invdaemon2.h)

target_link_libraries(invdaemon2
        LINK_PUBLIC
        -pthread
        -lm)
