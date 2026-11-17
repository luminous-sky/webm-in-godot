#ifndef DECODE_HANDLER_VPX_HPP
#define DECODE_HANDLER_VPX_HPP

#include <vpx/vpx_codec.h>

#include "decode_handler.hpp"

enum VPX_CODEC {
    VP9,
};

class DecodeHandlerVpx : public DecodeHandler {
    public:
        DecodeHandlerVpx(VPX_CODEC codec);
        virtual ~DecodeHandlerVpx();

        vpx_image_t *getLastFrame();
        virtual bool decodeFrame(const unsigned char *data, unsigned int numData) override;

        const unsigned char *imgToYuvBuf(const vpx_image_t *img, unsigned int *bufSize);
        const unsigned char *imgToRgbBuf(const vpx_image_t *img, unsigned int *bufSize, unsigned int *bufWidth, unsigned int *bufHeight);
    private:
        vpx_codec_ctx_t context{};
        vpx_codec_iface_t *interface{};

        vpx_image_t *lastImage{};
};

#endif