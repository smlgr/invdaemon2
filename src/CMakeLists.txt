add_executable(invdaemon2
        config.h
        invdaemon2.c invdaemon2.h
        log.c log.h
        ui.c ui.h
        cfg.c cfg.h)

target_link_libraries(invdaemon2
        LINK_PUBLIC
        -pthread
        -lm)
