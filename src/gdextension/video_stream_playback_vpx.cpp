#include <cstring>
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
    load_next_frame();

    Ref<Image> img = Image::create_empty(decodedWidth, decodedHeight, false, Image::FORMAT_RGBA8);
    texture -> set_image(img);
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

    if (decoder -> getLastFrame() != nullptr) {
        decodedData = decoder -> imgToRgbBuf(decoder -> getLastFrame(), &decodedDataLen, &decodedWidth, &decodedHeight);
        hasVideo = true;
    }
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
    if (!hasVideo) return;
    if (!playing) return;

    print_line("Updating");

    // Placeholder FPS used
    videoTime += 30*delta;
    if (videoTime < 1.0) return;
    videoTime -= 1.0;
    

    print_line("Rendering frame");
    
    PackedByteArray packedData{};
    packedData.resize(decodedDataLen);
    memcpy(packedData.ptrw(), decodedData, decodedDataLen);

    print_line(decodedWidth, "x", decodedHeight, " -> ", packedData[0], " ", packedData[1], " ", packedData[2], " ", packedData[3]);

    Ref<Image> img = Image::create_from_data(decodedWidth, decodedHeight, false, Image::FORMAT_RGBA8, packedData);
    // Ref<Image> img = Image::create_from_data(1920, 1080, false, Image::FORMAT_RGBA8, packedData);
    texture -> update(img);

    // Load next frame
    if (readerStatus.completed_ok() || !readerStatus.ok()) {
        playing = false;
        return;
    }

    // Drop frames as needed
    unsigned int offset = (unsigned int) videoTime;
    videoTime -= offset;

    print_line("Dropping ", offset, " frames");

    callback.skipFrames(offset);
    load_next_frame();

    print_line("Frame: ", callback.num_video_frames);
}



VideoStreamVpx::VideoStreamVpx() {}
VideoStreamVpx::~VideoStreamVpx() {}
