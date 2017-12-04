set(IPLAYER_VERSION "1.0")

add_executable(iplayer
               cli_ui.h
               cli_ui.cpp
               core.h
               core.cpp
               dummy_decoder.h
               dummy_decoder.cpp
               dummy_track_provider.h
               dummy_track_provider.cpp
               fs_track_provider.h
               fs_track_provider.cpp
               i_decoder.h
               i_player_control.h
               i_track_io.h
               i_track_provider.h
               i_user_interface.h
               main.cpp
               player_control.h
               player_control.cpp
               playlist.h
               playlist.cpp
               track_location.h
               track_info.h
               track_info.cpp
               track_provider_resolver.h
               track_provider_resolver.cpp
               utils/exec_queue.h
               utils/exec_queue.cpp
               utils/file_mapping.h
               utils/file_mapping.cpp
               utils/log.h
               utils/scope_guard.h
               )

if (OPTION_IPLAYER_ENABLE_LOG)
  set(IPLAYER_ENABLE_LOG "IPLAYER_ENABLE_LOG")
endif()

target_compile_definitions(iplayer PRIVATE
                           ${IPLAYER_ENABLE_LOG}
                           IPLAYER_VERSION="${IPLAYER_VERSION}")
target_compile_options(iplayer PRIVATE -Wall)
target_compile_features(iplayer PRIVATE cxx_std_14)
target_link_libraries(iplayer PRIVATE pthread)


# libmad
if (OPTION_IPLAYER_DECODER_MAD)
  target_sources(iplayer PRIVATE mad_decoder.h mad_decoder.cpp)
  target_compile_definitions(iplayer PRIVATE
                             IPLAYER_DECODER_MAD)
  target_link_libraries(iplayer PRIVATE mad pulse-simple pulse)
endif()


# gstreamer
if (OPTION_IPLAYER_DECODER_GSTREAMER)
  find_package(PkgConfig)
  pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.4)
  pkg_check_modules(GLIB REQUIRED glib-2.0>=2.0)

  target_sources(iplayer PRIVATE
                 gstreamer_decoder.h
                 gstreamer_decoder.cpp)

  target_include_directories(iplayer PRIVATE
                             ${GST_INCLUDE_DIRS}
                             ${GLIB_INCLUDES_DIRS})

  target_link_libraries(iplayer PRIVATE gstreamer-1.0 glib-2.0 gobject-2.0)
endif()
