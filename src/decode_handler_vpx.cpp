#include <cstdio>
#include <cstring>
#include <iostream>
#include <vpx/vpx_codec.h>
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>
#include <vpx/vpx_image.h>

#include "decode_handler_vpx.hpp"

using std::cout;


DecodeHandlerVpx::DecodeHandlerVpx(VPX_CODEC codec) {
    switch (codec) {
        case VP9:
            interface = vpx_codec_vp9_dx();
            break;
    }

    vpx_codec_dec_init(&context, interface, nullptr, 0);

    outputFile = fopen("output_yuv_images", "wb");
}


DecodeHandlerVpx::~DecodeHandlerVpx() {
    if (outputFile) fclose(outputFile);
    vpx_codec_destroy(&context);
}


const unsigned char *imgToYuvBuf(const vpx_image_t *img, unsigned int *bufSize);

// Returns success
bool DecodeHandlerVpx::decodeFrame(const unsigned char *data, unsigned int numData) {
    if (vpx_codec_decode(&context, data, numData, nullptr, 0)) {
        cout << "Error when decoding frame.\n";
        return false;
    }

    vpx_codec_iter_t iterator{};
    vpx_image_t *img = vpx_codec_get_frame(&context, &iterator);
    while (img) {
        // Write to file for testing
        unsigned int bufSize{};
        const unsigned char *buf = imgToYuvBuf(img, &bufSize);
        fwrite(buf, bufSize, 1, outputFile);
        delete[] buf;

        img = vpx_codec_get_frame(&context, &iterator);
    }

    return true;
}

// Returns a buffer containing the image's YUV data
// This buffer needs to be deleted manually by the caller using delete[]
const unsigned char *imgToYuvBuf(const vpx_image_t *img, unsigned int *bufSize) {
    // High bit depth formats have 16-bit buffers
    int pixelDensity = (img -> fmt & VPX_IMG_FMT_HIGHBITDEPTH) + 1;

    // Chroma shift changes the UV plane sizes
    unsigned int baseW = img -> d_w;
    unsigned int baseH = img -> d_h;

    unsigned int shiftW = (baseW+1) >> (img -> x_chroma_shift);
    unsigned int shiftH = (baseH+1) >> (img -> y_chroma_shift);


    // Buffer size:
    // Y -> baseWidth * baseHeight
    // U -> shiftedWidth * shiftedHeight
    // V -> shiftedWidth * shiftedHeight
    *bufSize = baseW*baseH + 2*shiftW*shiftH;

    unsigned char *buf = new unsigned char[*bufSize];
    unsigned char *cur = buf;


    for (int i = 0; i < 3; i++) {
        unsigned char *plane = img -> planes[i];
        int stride = img -> stride[i];

        // UV planes use the shifted dimensions
        unsigned int planeWidth = baseW;
        unsigned int planeHeight = baseH;

        if (i > 0) {
            planeWidth = shiftW;
            planeHeight = shiftH;

            // NV12 UV plane is written together
            if (img -> fmt == VPX_IMG_FMT_NV12) {
                if (i > 1) break;
                if (i == 1) planeWidth *= 2;
            }
        }

        for (unsigned int y = 0; y < planeHeight; y++) {
            unsigned int len = pixelDensity*planeWidth;

            memcpy(cur, plane, len);
            cur += len;

            plane += stride;
        }
    }

    const unsigned char *final = buf;
    return final;
}
