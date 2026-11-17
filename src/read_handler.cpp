#include <webm/callback.h>
#include <webm/element.h>
#include <webm/id.h>
#include <webm/dom_types.h>
#include <webm/status.h>

#include "decode_handler_vpx.hpp"
#include "read_handler.hpp"


using namespace webm;


ReadHandler::ReadHandler() {}
ReadHandler::~ReadHandler() {
    if (decoder) delete decoder;
}


Status ReadHandler::OnInfo(const ElementMetadata &metadata, const Info &info) {
    video_len_ms = info.duration.value();
    return Status(Status::kOkCompleted);
}

Status ReadHandler::OnTrackEntry(const ElementMetadata &metadata, const TrackEntry &track_entry) {
    Element<Video> videoElement = track_entry.video;
    if (!video_found && videoElement.is_present()) {
        video_track = track_entry;
        video_found = true;

        if (video_track.codec_id.value() == "V_VP9") {
            decoder = new DecodeHandlerVpx(VP9);
        }
    }

    Element<Audio> audioElement = track_entry.audio;
    if (!audio_found && audioElement.is_present()) {
        audio_track = track_entry;
        audio_found = true;
    }

    return Status(Status::kOkCompleted);
}


Status ReadHandler::HandleBlockBegin(const Block &block) {
    // Wait to decode the next frame
    if (blockDecoded) {
        blockDecoded = false;
        return Status(Status::kOkPartial);
    }

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
    return HandleBlockBegin(block);
}

Status ReadHandler::OnSimpleBlockBegin(const ElementMetadata &metadata, const SimpleBlock &simple_block, Action *action) {
    return HandleBlockBegin(simple_block);
}


Status ReadHandler::OnFrame(const FrameMetadata &metadata, Reader *reader, uint64_t *bytes_remaining) {    
    // After pausing, the first "frame" can have 0 bytes
    if (*bytes_remaining == 0) return Status(Status::kOkCompleted);
    
    if (next_frame_video) {
        num_video_frames++;
        
        // Decode one frame
        if (decoder) {
            unsigned int bufLen = *bytes_remaining;
            unsigned char *buf = new unsigned char[bufLen];
            unsigned char *cur = buf;
            
            uint64_t dataRead = 0;
            Status readerStatus;
            do {
                readerStatus = reader -> Read(*bytes_remaining, cur, &dataRead);
                *bytes_remaining -= dataRead;
                cur += dataRead;
            } while (readerStatus.code == Status::kOkPartial);

            const unsigned char *data = buf;
            decoder -> decodeFrame(data, bufLen);
            delete[] buf;

            blockDecoded = true;
            return Status(Status::kOkCompleted);
        }
    }
    
    if (next_frame_audio) {
        num_audio_frames++;
    }
    
    return Skip(reader, bytes_remaining);
}
