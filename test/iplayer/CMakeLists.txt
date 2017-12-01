add_library(iplayer_test_lib STATIC

            # Mock allowing to replace true cli by test controlled code. It will
            # call in a dedicated thread the function:
            #
            # void test_ui_main(IPlayerControl*)
            #
            # Define this function your test to setup a complete iplayer
            # environment (ex: monkey_test.cpp)
            cli_ui_mock.cpp
            test_ui_main.h

            ${IPLAYER_SRC_DIR}/iplayer/core.h
            ${IPLAYER_SRC_DIR}/iplayer/core.cpp
            ${IPLAYER_SRC_DIR}/iplayer/i_player_control.h
            ${IPLAYER_SRC_DIR}/iplayer/i_track_io.h
            ${IPLAYER_SRC_DIR}/iplayer/i_track_provider.h
            ${IPLAYER_SRC_DIR}/iplayer/i_user_interface.h
            ${IPLAYER_SRC_DIR}/iplayer/log.h
            ${IPLAYER_SRC_DIR}/iplayer/decoder.h
            ${IPLAYER_SRC_DIR}/iplayer/decoder.cpp
            ${IPLAYER_SRC_DIR}/iplayer/fs_track_provider.h
            ${IPLAYER_SRC_DIR}/iplayer/fs_track_provider.cpp
            ${IPLAYER_SRC_DIR}/iplayer/main.cpp
            ${IPLAYER_SRC_DIR}/iplayer/player_control.h
            ${IPLAYER_SRC_DIR}/iplayer/player_control.cpp
            ${IPLAYER_SRC_DIR}/iplayer/playlist.h
            ${IPLAYER_SRC_DIR}/iplayer/playlist.cpp
            ${IPLAYER_SRC_DIR}/iplayer/track_location.h
            ${IPLAYER_SRC_DIR}/iplayer/track_info.h
            ${IPLAYER_SRC_DIR}/iplayer/track_info.cpp
            ${IPLAYER_SRC_DIR}/iplayer/utils/scope_guard.h
            ${IPLAYER_SRC_DIR}/iplayer/utils/exec_queue.h
            ${IPLAYER_SRC_DIR}/iplayer/utils/exec_queue.cpp
            )

target_compile_features(iplayer_test_lib PUBLIC cxx_std_14)
target_compile_definitions(iplayer_test_lib PUBLIC
                           #IPLAYER_ENABLE_LOG
                           IPLAYER_VERSION="${IPLAYER_VERSION}")
target_link_libraries(iplayer_test_lib INTERFACE pthread)

link_libraries(iplayer_test_lib)

# test playlist add/remove/remove dup...
add_executable(playlist_test playlist_test.cpp)
add_test(NAME playlist_test COMMAND playlist_test)

# test concurrency
add_executable(monkey_test monkey_test.cpp)
add_test(NAME monkey_test COMMAND monkey_test)
