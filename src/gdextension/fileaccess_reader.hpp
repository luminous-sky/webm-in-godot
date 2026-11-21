#ifndef FILEACCESS_READER_HPP
#define FILEACCESS_READER_HPP

#include <webm/file_reader.h>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>

using webm::Reader, webm::Status;
using godot::FileAccess, godot::Ref;
using std::size_t, std::uint8_t, std::uint64_t;

class FileAccessReader : public Reader {
    public:
        // Creates an empty reader without any data
        FileAccessReader();

        // Creates an empty reader which will read from FileAccess
        FileAccessReader(Ref<FileAccess> init_file);

        virtual Status Read(size_t num_to_read, uint8_t* buffer, uint64_t* num_actually_read) override;
        virtual Status Skip(uint64_t num_to_skip, uint64_t* num_actually_skipped) override;

        virtual uint64_t Position() const override;
    
    private:
        Ref<FileAccess> file;
};

#endif
