#ifndef VIDEO_STREAM_VPX_HPP
#define VIDEO_STREAM_VPX_HPP

#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/video_stream_playback.hpp>
#include <godot_cpp/classes/video_stream_player.hpp>
#include <godot_cpp/classes/video_stream.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/string.hpp>

#include "fileaccess_reader.hpp"
#include <webm/webm_parser.h>
#include "../read_handler.hpp"
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/wrapped.hpp>

#include <vpx/vpx_codec.h>


using namespace godot;


class VideoStreamPlaybackVpx : public VideoStreamPlayback {
    GDCLASS(VideoStreamPlaybackVpx, VideoStreamPlayback);

    private:
        bool playing{false};
        Ref<ImageTexture> texture{nullptr};

        Ref<FileAccess> file{nullptr};

        Status readerStatus{};
        FileAccessReader reader{};
        ReadHandler callback{};
        WebmParser parser{};

        const unsigned char *decodedData{nullptr};
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

        virtual Ref<Texture2D> _get_texture() const override;

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


#endif
