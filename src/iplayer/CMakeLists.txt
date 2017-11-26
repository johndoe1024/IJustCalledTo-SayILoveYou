set(IPLAYER_VERSION "1.0")

add_executable(iplayer
               cli_ui.h
               cli_ui.cpp
               core.h
               core.cpp
               i_player_control.h
               i_track_io.h
               i_track_provider.h
               i_user_interface.h
               log.h
               decoder.h
               decoder.cpp
               fs_track_provider.h
               fs_track_provider.cpp
               main.cpp
               player_control.h
               player_control.cpp
               playlist.h
               playlist.cpp
               track_location.h
               track_location.cpp
               track_info.h
               utils/scope_guard.h
               utils/exec_queue.h
               utils/exec_queue.cpp
               )

target_compile_features(iplayer PRIVATE cxx_std_14)
#target_compile_options(iplayer PRIVATE -Wall)
target_compile_definitions(iplayer PRIVATE
                           #IPLAYER_ENABLE_LOG
                           IPLAYER_VERSION="${IPLAYER_VERSION}")
target_link_libraries(iplayer PRIVATE pthread)
