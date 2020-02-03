set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenSSL REQUIRED)

message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

add_executable(invdaemon2
        config.h
        invdaemon2.c invdaemon2.h
        utils.c utils.h
        log.c log.h
        ui.c ui.h
        cfg.c cfg.h
        http.c http.h
        queue.c queue.h)

target_link_libraries(invdaemon2
        LINK_PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
        m)
