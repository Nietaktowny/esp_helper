add_subdirectory(test_tcp_server)

include(FetchContent)
FetchContent_Declare(
        Unity
        GIT_REPOSITORY https://github.com/ThrowTheSwitch/Unity.git
        GIT_TAG        v2.5.2
)
set(UNITY_EXTENSION_FIXTURE ON CACHE BOOL "Enable Unity Fixture Extension")
set(UNITY_EXTENSION_MEMORY  ON CACHE BOOL "Enable Unity Memory Extension")
FetchContent_MakeAvailable(Unity)

add_compile_definitions(Unity PUBLIC "UNIT_TEST")

include(CTest)