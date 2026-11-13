#ifndef READ_HANDLER_HPP
#define READ_HANDLER_HPP

#include <webm/callback.h>
using namespace webm;

class ReadHandler : public Callback {
    public:
        ReadHandler();
        virtual ~ReadHandler();

        Status OnInfo(const ElementMetadata &metadata, const Info &info) override;
        Status OnTrackEntry(const ElementMetadata &metadata, const TrackEntry &track_entry) override;
};

#endif
