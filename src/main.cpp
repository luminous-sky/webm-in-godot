#include <iostream>
#include <fstream>
#include <cstdio>
#include <string>

#include <webm/file_reader.h>
#include <webm/status.h>
#include <webm/webm_parser.h>

#include "read_handler.hpp"
#include "decode_handler_vpx.hpp"


using std::cout, std::string, std::ifstream;
using namespace webm;


int main(int argc, char **argv) {
    cout << "Hello, world!\n";

    if (argc < 2 || argc > 3) {
        cout << "Usage:\n\t./out\tvideo_file\tyuv_images_file (optional)\n";
        return 0;
    }


    FILE *video = fopen(argv[1], "rb");
    if (!video) {
        cout << "Failed to open " << argv[1] << ".\n";
        return 1;
    }

    FILE *yuvImages = nullptr;
    if (argc == 3) {
        yuvImages = fopen(argv[2], "wb");

        if (!yuvImages) {
            fclose(video);

            cout << "Failed to open " << argv[2] << ".\n";
            return 1;
        }
    }

    FileReader reader{video};
    ReadHandler callback{};
    WebmParser parser{};

    int count = 0;
    Status status{};
    do {
        status = parser.Feed(&callback, &reader);

        // Output status information on the first pass
        if (yuvImages && count == 0) {
            if (!callback.decoder || !(callback.decoder -> isInitialized)) {
                cout << "Failed to decode the video's encoded data.\n";
                count++;
            }
        }

        // Save first 64 frames
        if (yuvImages && count < 64 && callback.decoder && callback.decoder -> isInitialized) {
            DecodeHandlerVpx *decoderVpx = (DecodeHandlerVpx *) callback.decoder;
            decoderVpx -> dequeueNextFrame();
            
            unsigned int bufSize = 0;
            const unsigned char *buf = decoderVpx -> imgToYuvBuf(decoderVpx -> getLastFrame(), &bufSize);
            fwrite(buf, bufSize, 1, yuvImages);
            delete[] buf;

            count++;
        }
    } while (status.ok() && !status.completed_ok());

    // Status status = parser.Feed(&callback, &reader);
    if (!status.ok()) {
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
