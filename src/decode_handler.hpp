#ifndef DECODE_HANDLER_HPP
#define DECODE_HANDLER_HPP

// Abstract class
class DecodeHandler {
    public:
        bool isInitialized{false};

        virtual ~DecodeHandler() = default;

        virtual bool decodeFrame(const unsigned char *data, unsigned int numData) = 0;
};

#endif