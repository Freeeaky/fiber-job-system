set(FJS_DIR_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(FJS_DIR_INC "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(PROJECT_NAME "fjs")

project(${PROJECT_NAME})

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME}
                PRIVATE
                    "${FJS_DIR_SRC}/Counter.cpp"
                    "${FJS_DIR_SRC}/Fiber.cpp"
                    "${FJS_DIR_SRC}/Job.cpp"
                    "${FJS_DIR_SRC}/List.cpp"
                    "${FJS_DIR_SRC}/Manager.cpp"
                    "${FJS_DIR_SRC}/ManagerCallbacks.cpp"
                    "${FJS_DIR_SRC}/ManagerJobs.cpp"
                    "${FJS_DIR_SRC}/ManagerThreads.cpp"
                    "${FJS_DIR_SRC}/Queue.cpp"
                    "${FJS_DIR_SRC}/Thread.cpp"
                )

target_include_directories( ${PROJECT_NAME}
                            PUBLIC ${FJS_DIR_INC}
                            )

add_executable("FJS_Test"
                "${CMAKE_CURRENT_SOURCE_DIR}/test/main-test/Main.cpp")

target_link_libraries("FJS_Test"
                        ${PROJECT_NAME})