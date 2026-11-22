#include <algorithm>
#include <cstring>
#include <vpx/vpx_codec.h>
#include <vpx/vpx_decoder.h>
#include <vpx/vp8dx.h>
#include <vpx/vpx_image.h>

#include "decode_handler_vpx.hpp"


DecodeHandlerVpx::DecodeHandlerVpx(VPX_CODEC codec) {
    switch (codec) {
        case VP9:
            interface = vpx_codec_vp9_dx();
            break;
    }

    vpx_codec_dec_init(&context, interface, nullptr, 0);
}


DecodeHandlerVpx::~DecodeHandlerVpx() {
    vpx_codec_destroy(&context);
}


vpx_image_t *DecodeHandlerVpx::getLastFrame() {
    return lastImage;
}


// Returns success
bool DecodeHandlerVpx::decodeFrame(const unsigned char *data, unsigned int numData) {
    vpx_codec_err_t error;
    if ((error = vpx_codec_decode(&context, data, numData, nullptr, 0))) {
        return false;
    }

    vpx_codec_iter_t iterator{};
    vpx_image_t *img = vpx_codec_get_frame(&context, &iterator);
    while (img) {
        lastImage = img;
        img = vpx_codec_get_frame(&context, &iterator);
    }

    return true;
}

// Returns a buffer containing the image's YUV data
// This buffer needs to be deleted manually by the caller using delete[]
const unsigned char *DecodeHandlerVpx::imgToYuvBuf(const vpx_image_t *img, unsigned int *bufSize) {
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

// Returns a buffer containing the image's RGB data
// This buffer needs to be deleted manually by the caller using delete[]
const unsigned char *DecodeHandlerVpx::imgToRgbBuf(const vpx_image_t *img, unsigned int *bufSize, unsigned int *bufWidth, unsigned int *bufHeight) {
    // Skip handling high bit depth formats (16-bit buffers)
    if (img -> fmt & VPX_IMG_FMT_HIGHBITDEPTH) return nullptr;
    
    // Only handle NV12 formats (interlaced UV planes)
    if (img -> fmt == VPX_IMG_FMT_NV12) return nullptr;


    // RGBA8 format
    *bufWidth = img -> d_w;
    *bufHeight = img -> d_h;
    *bufSize = 4 * (*bufWidth) * (*bufHeight);
    unsigned char *rgbBuf = new unsigned char[*bufSize];
    unsigned char *cur = rgbBuf;

    unsigned char *yPlane = img -> planes[0];
    unsigned char *uPlane = img -> planes[1];
    unsigned char *vPlane = img -> planes[2];

    int yStride = img -> stride[0];
    int uStride = img -> stride[1];
    int vStride = img -> stride[2];

    for (unsigned int imgY = 0; imgY < img -> d_h; imgY++) {        
        for (unsigned int imgX = 0; imgX < img -> d_w; imgX++) {
            // UV planes are interlaced, so they have half the data
            int y = yPlane[imgY * yStride + imgX];
            int u = uPlane[(imgY >> 1) * uStride + (imgX >> 1)];
            int v = vPlane[(imgY >> 1) * vStride + (imgX >> 1)];

            // BT.709 constants
            double r_f = (1.164*(y - 16) + 1.596*(v - 128));
            double g_f = (1.164*(y - 16) - 0.391*(u - 128) - 0.813*(v - 128));
            double b_f = (1.164*(y - 16) + 2.018*(u - 128));

            unsigned char r = (unsigned char) std::clamp(r_f, 0.0, 255.0);
            unsigned char g = (unsigned char) std::clamp(g_f, 0.0, 255.0);
            unsigned char b = (unsigned char) std::clamp(b_f, 0.0, 255.0);
            unsigned char a = 255;

            cur[0] = r;
            cur[1] = g;
            cur[2] = b;
            cur[3] = a;

            cur += 4;
        }
    }


    const unsigned char *final = rgbBuf;
    return final;
}
