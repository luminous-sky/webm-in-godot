#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <webm/webm_parser.h>
#include "../decode_handler_vpx.hpp"
#include "video_stream_vpx.hpp"


using namespace godot;


VideoStreamPlaybackVpx::VideoStreamPlaybackVpx() {
    playing = false;
}

VideoStreamPlaybackVpx::~VideoStreamPlaybackVpx() {
    if (decodedData) delete[] decodedData;
}

void VideoStreamPlaybackVpx::set_file(const String &filename) {
    // When VideoStreamVpx is initially made, it calls set_file with a blank filename
    if (filename.length() == 0) return;

    file = FileAccess::open(filename, FileAccess::READ);

    if (file.is_null()) {
        ERR_FAIL_COND_MSG(file.is_null(), "Failed to open '" + filename + "'.");
    }

    reader = FileAccessReader{file};
}


void VideoStreamPlaybackVpx::load_next_frame() {
    if (decodedData) {
        delete[] decodedData;
        decodedData = nullptr;
    }
    if (readerStatus.completed_ok() || !readerStatus.ok()) return;


    readerStatus = parser.Feed(&callback, &reader);
    DecodeHandlerVpx *decoder = (DecodeHandlerVpx *) callback.decoder;
    
    decodedData = decoder -> imgToRgbBuf(decoder -> getLastFrame(), &decodedDataLen, &decodedWidth, &decodedHeight);
}


void VideoStreamPlaybackVpx::_bind_methods() {
    // ClassDB::bind_method(D_METHOD("is_enabled"), &VideoStreamVpx::is_enabled);
}

void VideoStreamPlaybackVpx::_play() {
    playing = true;
}

void VideoStreamPlaybackVpx::_stop() {
    playing = false;
}

bool VideoStreamPlaybackVpx::_is_playing() const {
    return playing;
}


void VideoStreamPlaybackVpx::_update(double delta) {
    if (file.is_null() || file -> eof_reached()) return;
    if (!playing) return;

    if (readerStatus.completed_ok() || !readerStatus.ok()) return;
    load_next_frame();

    if (!decodedData) return;
    
    PackedByteArray packedData{};
    for (unsigned int i = 0; i < decodedDataLen; i++) {
        packedData.append(decodedData[i]);
    }

    Ref<Image> img = Image::create_from_data(decodedWidth, decodedHeight, false, Image::FORMAT_RGBA8, packedData);
    Ref<Texture2D> texture = _get_texture();
    texture -> draw(img -> get_rid(), Vector2{0, 0});

    playing = false;
}



VideoStreamVpx::VideoStreamVpx() {}
VideoStreamVpx::~VideoStreamVpx() {}
