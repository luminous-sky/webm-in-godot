#ifndef DECODE_HANDLER_HPP
#define DECODE_HANDLER_HPP

// Abstract class
class DecodeHandler {
    public:
        virtual ~DecodeHandler() = default;

        virtual bool decodeFrame(const unsigned char *data, unsigned int numData) = 0;
};

#endif