set(IPLAYER_VERSION "1.0")

add_executable(iplayer
               core.h
               core.cpp
               i_player_control.h
               i_user_interface.h
               log.h
               main.cpp
               decoder.h
               decoder.cpp
               playlist.h
               playlist.cpp
               track_location.h
               track_location.cpp
               player_control/player_control.h
               player_control/player_control.cpp
               ui/cli_ui.h
               ui/cli_ui.cpp
               utils/scope_guard.h
               utils/exec_queue.h
               utils/exec_queue.cpp
               )

target_compile_features(iplayer PRIVATE cxx_std_14)
#target_compile_options(iplayer PRIVATE -Wall)
target_compile_definitions(iplayer PRIVATE
                           IPLAYER_VERSION="${IPLAYER_VERSION}"
                           IPLAYER_ENABLE_LOG)
target_link_libraries(iplayer PRIVATE pthread)
