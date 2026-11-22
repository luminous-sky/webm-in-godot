#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>

#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <webm/webm_parser.h>
#include "../decode_handler_vpx.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "video_stream_vpx.hpp"


using namespace godot;


VideoStreamPlaybackVpx::VideoStreamPlaybackVpx() {
    playing = false;
    texture.instantiate();

    Ref<Image> img = Image::create_empty(1920, 1080, false, Image::FORMAT_RGBA8);
    texture -> set_image(img);

    print_line("Texture image dimensions: ", texture -> get_image() -> get_size());
}

VideoStreamPlaybackVpx::~VideoStreamPlaybackVpx() {
    print_line("Decoded data exists: ", (decodedData != nullptr));

    if (file != nullptr) file -> unreference();
    if (decodedData != nullptr) delete[] decodedData;
}

void VideoStreamPlaybackVpx::set_file(const String &filename) {
    // When VideoStreamVpx is initially made, it calls set_file with a blank filename
    if (filename.length() == 0) return;

    print_line("Loading WEBM file");

    file = FileAccess::open(filename, FileAccess::READ);

    print_line("File loaded: ", !(file.is_null()));
    ERR_FAIL_COND_MSG(file.is_null(), "Failed to open '" + filename + "'.");

    print_line("Creating WEBM reader");

    reader = FileAccessReader{file};
}


// Assumes that another frame is available in the parser
void VideoStreamPlaybackVpx::load_next_frame() {
    if (decodedData != nullptr) {
        delete[] decodedData;
        decodedData = nullptr;
    }

    print_line("Decoding frame");
    
    readerStatus = parser.Feed(&callback, &reader);
    DecodeHandlerVpx *decoder = (DecodeHandlerVpx *) callback.decoder;

    decodedData = decoder -> imgToRgbBuf(decoder -> getLastFrame(), &decodedDataLen, &decodedWidth, &decodedHeight);
}

Ref<Texture2D> VideoStreamPlaybackVpx::_get_texture() const {
    return texture;
}


void VideoStreamPlaybackVpx::_bind_methods() {}

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
    if (!playing) return;
    playing = false;

    print_line("Rendering frame");
    
    PackedByteArray packedData{};
    // for (unsigned int i = 0; i < decodedDataLen; i++) {
    //     packedData.append(decodedData[i]);
    // }
    for (unsigned int i = 0; i < 1920*1080*4; i++) {
        if (i % 4 == 3) {
            packedData.append(255);
        } else {
            packedData.append(0);
        }
    }

    print_line(1920, "x", 1080, " -> ", packedData[0], " ", packedData[1], " ", packedData[2], " ", packedData[3]);

    // Ref<Image> img = Image::create_from_data(decodedWidth, decodedHeight, false, Image::FORMAT_RGBA8, packedData);
    Ref<Image> img = Image::create_from_data(1920, 1080, false, Image::FORMAT_RGBA8, packedData);
    texture -> update(img);
}



VideoStreamVpx::VideoStreamVpx() {}
VideoStreamVpx::~VideoStreamVpx() {}
