#ifndef VIDEO_STREAM_VPX_HPP
#define VIDEO_STREAM_VPX_HPP

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/classes/video_stream_player.hpp>
#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/string.hpp>

#include <webm/file_reader.h>
#include <webm/webm_parser.h>
#include "../read_handler.hpp"
#include "godot_cpp/classes/wrapped.hpp"

#include <vpx/vpx_codec.h>


using namespace godot;


class VideoStreamPlaybackVpx : public VideoStreamPlayback {
    GDCLASS(VideoStreamPlaybackVpx, VideoStreamPlayback);

    private:
        bool playing{false};

        FILE *file;

        Status readerStatus;
        FileReader reader;
        ReadHandler callback;
        WebmParser parser;

        const unsigned char *decodedData;
        unsigned int decodedDataLen;
        unsigned int decodedWidth;
        unsigned int decodedHeight;

        void load_next_frame();

    protected:
        static void _bind_methods();

    public:
        VideoStreamPlaybackVpx();
        virtual ~VideoStreamPlaybackVpx();

        void set_file(const String &filename);

        virtual void _play() override;
        virtual void _stop() override;

        virtual bool _is_playing() const override;

        virtual void _update(double delta) override;
};


class VideoStreamVpx : public VideoStream {
    GDCLASS(VideoStreamVpx, VideoStream);

    protected:
        static void _bind_methods() {};

    public:
        VideoStreamVpx();
        virtual ~VideoStreamVpx();

        virtual Ref<VideoStreamPlayback> _instantiate_playback() override {
            Ref<VideoStreamPlaybackVpx> playback = memnew(VideoStreamPlaybackVpx);
            playback -> set_file(get_file());

            return playback;
        }
};


class ResourceFormatLoaderVpx : public ResourceFormatLoader {
    GDCLASS(ResourceFormatLoaderVpx, ResourceFormatLoader);

    protected:
        static void _bind_methods() {};

    public:
        virtual Variant _load(const String &path, const String &original_path = "", bool use_sub_threads = false, int32_t cache_mode = CACHE_MODE_REUSE) const override;
        virtual PackedStringArray _get_recognized_extensions() const override;
        virtual bool _handles_type(const StringName &type) const override;
        virtual String _get_resource_type(const String &path) const override;
};

#endif
