#include <iostream>

#include <webm/callback.h>
#include <webm/element.h>
#include <webm/id.h>
#include <webm/dom_types.h>

#include "read_handler.hpp"


using std::cout;
using namespace webm;


ReadHandler::ReadHandler() {}
ReadHandler::~ReadHandler() {}


Status ReadHandler::OnInfo(const ElementMetadata &metadata, const Info &info) {
    cout << "Duration (ms): " << info.duration.value() << "\n";

    return Status(Status::kOkCompleted);
}

Status ReadHandler::OnTrackEntry(const ElementMetadata &metadata, const TrackEntry &track_entry) {
    cout << "Codec: " << track_entry.codec_id.value() << "\n";

    Element<Video> videoElement = track_entry.video;
    if (videoElement.is_present()) {
        Video video = videoElement.value();

        cout << "Resolution: ";
        cout << video.display_width.value() << "x" << video.display_height.value() << "\n";
    }

    Element<Audio> audioElement = track_entry.audio;
    if (audioElement.is_present()) {
        Audio audio = audioElement.value();

        cout << "Audio Bit Depth: ";
        cout << audio.bit_depth.value() << "\n";

        cout << "Output Frequency: ";
        cout << audio.output_frequency.value() << "\n";
    }

    return Status(Status::kOkCompleted);
}
