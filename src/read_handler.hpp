#ifndef READ_HANDLER_HPP
#define READ_HANDLER_HPP

#include <string>
#include <webm/callback.h>

#include "decode_handler.hpp"


using std::string, std::uint64_t;
using namespace webm;

class ReadHandler : public Callback {
    public:
        double video_len_ms{};
        unsigned int num_video_frames{};
        unsigned int num_audio_frames{};

        bool video_found{false};
        bool audio_found{false};
        TrackEntry video_track;
        TrackEntry audio_track;

        DecodeHandler *decoder;


        ReadHandler();
        virtual ~ReadHandler();

        virtual Status OnInfo(const ElementMetadata &metadata, const Info &info) override;
        virtual Status OnTrackEntry(const ElementMetadata &metadata, const TrackEntry &track_entry) override;

        virtual Status OnBlockBegin(const ElementMetadata &metadata, const Block &block, Action *action) override;
        virtual Status OnSimpleBlockBegin(const ElementMetadata &metadata, const SimpleBlock &simple_block, Action *action) override;

        virtual Status OnFrame(const FrameMetadata &metadata, Reader *reader, uint64_t *bytes_remaining) override;
        
    private:
        bool next_frame_video{false};
        bool next_frame_audio{false};

        bool blockDecoded{false};
        Status HandleBlockBegin(const Block &block);
};

#endif
