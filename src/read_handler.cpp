#include <iostream>

#include <webm/callback.h>
#include <webm/element.h>
#include <webm/id.h>
#include <webm/dom_types.h>
#include <webm/status.h>

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


Status ReadHandler::HandleBlock(const Block &block) {
    next_frame_video = false;
    next_frame_audio = false;
    
    if (video_found) {
        if (block.track_number == video_track.track_number.value()) {
            next_frame_video = true;
            return Status(Status::kOkCompleted);
        }
    }
    
    if (audio_found) {
        if (block.track_number == audio_track.track_number.value()) {
            next_frame_audio = true;
            return Status(Status::kOkCompleted);
        }
    }
    
    // At least one track must have been found
    return Status(Status::kInvalidElementValue);
}

Status ReadHandler::OnBlockBegin(const ElementMetadata &metadata, const Block &block, Action *action) {
    return HandleBlock(block);
}

Status ReadHandler::OnSimpleBlockBegin(const ElementMetadata &metadata, const SimpleBlock &simple_block, Action *action) {
    return HandleBlock(simple_block);
}


Status ReadHandler::OnFrame(const FrameMetadata& metadata, Reader* reader, uint64_t* bytes_remaining) {
    // After pausing, the first "frame" can have 0 bytes
    if (*bytes_remaining == 0) return Status(Status::kOkCompleted);
    
    if (next_frame_video) {
        num_video_frames++;
    } else if (next_frame_audio) {
        num_audio_frames++;
    }
    
    return Skip(reader, bytes_remaining);
}
