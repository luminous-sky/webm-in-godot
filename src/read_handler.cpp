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
    video_len_ms = info.duration.value();
    return Status(Status::kOkCompleted);
}

Status ReadHandler::OnTrackEntry(const ElementMetadata &metadata, const TrackEntry &track_entry) {
    Element<Video> videoElement = track_entry.video;
    if (!video_found && videoElement.is_present()) {
        video_track = track_entry;
        video_found = true;
    }

    Element<Audio> audioElement = track_entry.audio;
    if (audioElement.is_present()) {
        audio_track = track_entry;
        audio_found = true;
    }

    return Status(Status::kOkCompleted);
}

Status ReadHandler::OnFrame(const FrameMetadata& metadata, Reader* reader, uint64_t* bytes_remaining) {
    // After pausing, the first "frame" can have 0 bytes
    if (*bytes_remaining == 0) return Status(Status::kOkCompleted);

    num_frames++;
    return Skip(reader, bytes_remaining);
}
