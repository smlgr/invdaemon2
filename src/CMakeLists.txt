set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)
message("Threads flags: ${CMAKE_THREAD_LIBS_INIT}")

find_package(OpenSSL REQUIRED)
message("OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
message("OpenSSL libraries: ${OPENSSL_LIBRARIES}")

add_executable(invdaemon
        config.h
        invdaemon.c invdaemon.h
        utils.c utils.h
        log.c log.h
        ui.c ui.h
        cfg.c cfg.h
        http.c http.h
        queue.c queue.h)

target_link_libraries(invdaemon
        LINK_PUBLIC
        OpenSSL::SSL
        OpenSSL::Crypto
        Threads::Threads
        m)
