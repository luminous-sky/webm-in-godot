#ifndef DECODE_HANDLER_VPX_HPP
#define DECODE_HANDLER_VPX_HPP

#include <cstdio>
#include <vpx/vpx_codec.h>

#include "decode_handler.hpp"

enum VPX_CODEC {
    VP9,
};

class DecodeHandlerVpx : public DecodeHandler {
    public:
        DecodeHandlerVpx(VPX_CODEC codec);
        virtual ~DecodeHandlerVpx();

        virtual bool decodeFrame(const unsigned char *data, unsigned int numData) override;
    
    private:
        vpx_codec_ctx_t context{};
        vpx_codec_iface_t *interface{};

        vpx_image_t *lastImage{};

        // Output file for testing purposes
        FILE *outputFile;
};

#endif