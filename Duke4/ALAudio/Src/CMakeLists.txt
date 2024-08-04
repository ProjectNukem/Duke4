project(ALAudio)
add_library(ALAudio SHARED)

set(SOURCE_FILES
    ALAudio.cpp
    ALAudioAPI.cpp
	ALAudioBuild.cpp
	ALAudioSoundInstance.cpp
	ALAudioSubsystem.cpp)

source_group("Src" FILES ${SOURCE_FILES})
target_sources(ALAudio PRIVATE ${SOURCE_FILES})

target_link_directories(ALAudio PRIVATE ../Lib)
target_link_libraries(ALAudio PRIVATE Engine winmm)
target_link_libraries(ALAudio PRIVATE optimized OpenAL32 debug OpenAL32d)
target_link_libraries(ALAudio PRIVATE optimized ALURE32-static debug ALURE32-staticd)
target_link_libraries(ALAudio PRIVATE optimized ogg debug oggd)
target_link_libraries(ALAudio PRIVATE optimized libxmp debug libxmpd)
target_link_libraries(ALAudio PRIVATE optimized vorbis debug vorbisd)
target_link_libraries(ALAudio PRIVATE optimized vorbisenc debug vorbisencd)
target_link_libraries(ALAudio PRIVATE optimized vorbisfile debug vorbisfiled)
target_link_libraries(ALAudio PRIVATE optimized sndfile debug sndfiled)
target_link_libraries(ALAudio PRIVATE optimized mpg123 debug mpg123d)
target_link_libraries(ALAudio PRIVATE optimized FLAC debug FLACd)

target_include_directories(ALAudio PRIVATE
    ${CMAKE_SOURCE_DIR}/Core/Inc
    ${CMAKE_SOURCE_DIR}/Engine/Inc
    ../Inc
    ${CMAKE_SOURCE_DIR}/Render/Src
    ${CMAKE_SOURCE_DIR}/Engine/Src)

target_compile_definitions(ALAudio PRIVATE UNICODE _UNICODE "ALAUDIO_API=__declspec(dllexport)")
