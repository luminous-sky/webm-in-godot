#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>

#include <webm/file_reader.h>
#include <webm/status.h>
#include <webm/webm_parser.h>

#include "read_handler.hpp"


using std::cout, std::string, std::ifstream;
using namespace webm;


int main(int argc, char **argv) {
    cout << "Hello, world!\n";

    if (argc != 2) {
        cout << "Usage:\n\t./out\tvideo_file\n";
        return 0;
    }


    FILE *video = fopen(argv[1], "rb");
    if (!video) {
        cout << "Failed to open " << argv[1] << ".\n";
        return 1;
    }

    FileReader reader{video};
    ReadHandler callback{};
    WebmParser parser{};

    Status status = parser.Feed(&callback, &reader);
    if (!status.completed_ok()) {
        cout << "Failed to parse file (" << status.code << ")\n";
        return 1;
    }


    if (callback.video_found) {
        cout << "Video Codec: " << (callback.video_track.codec_id).value() << "\n";

        Video videoElement = (callback.video_track.video).value();
        cout << "Resolution: ";
        cout << videoElement.display_width.value() << "x" << videoElement.display_height.value() << "\n";
    }

    if (callback.audio_found) {
        cout << "Audio Codec: " << (callback.audio_track.codec_id).value() << "\n";

        Audio audioElement = (callback.audio_track.audio).value();
        cout << "Channels: " << audioElement.channels.value() << "\n";
        cout << "Bit Depth: " << audioElement.bit_depth.value() << "\n";
        cout << "Sampling Frequency: " << audioElement.sampling_frequency.value() << "\n";
        cout << "Output Frequency: " << audioElement.output_frequency.value() << "\n";
    }

    cout << "Number of Video Frames: " << callback.num_video_frames << "\n";
    cout << "Number of Audio Frames: " << callback.num_audio_frames << "\n";

    return 0;
}
