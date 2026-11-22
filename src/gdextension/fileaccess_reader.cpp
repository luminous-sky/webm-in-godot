#include <webm/file_reader.h>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/print_string.hpp>

#include "fileaccess_reader.hpp"

using webm::Status;
using godot::FileAccess, godot::Ref, godot::print_error, godot::String;
using std::size_t, std::uint8_t, std::uint64_t;


// Creates an empty reader without any data
FileAccessReader::FileAccessReader() {
    file = nullptr;
}

FileAccessReader::~FileAccessReader() {
    // Remove reference to file so that Godot can clean it up
    if (file != nullptr) file -> unreference();
}

// Creates an empty reader which will read from FileAccess
FileAccessReader::FileAccessReader(Ref<FileAccess> init_file) {
    file = init_file;
}


Status handleError(String msg) {
    print_error(msg);
    return Status(Status::kSeekFailed);
}

Status FileAccessReader::Read(size_t num_to_read, uint8_t* buffer, uint64_t* num_actually_read) {
    // Modified assertions from FileReader
    if (num_to_read <= 0) return handleError("FileAccessReader cannot read 0 bytes");
    if (buffer == nullptr) return handleError("FileAccessReader buffer is null");
    if (num_actually_read == nullptr) return handleError("FileAccessReader read count is null");
    
    
    if (file == nullptr || file -> eof_reached()) {
        *num_actually_read = 0;
        return Status(Status::kEndOfFile);
    }

    size_t actualToRead = num_to_read;
    
    uint64_t remaining = (file -> get_length()) - (file -> get_position());
    if (remaining <= 0) {
        *num_actually_read = 0;
        return Status(Status::kEndOfFile);
    }

    if (remaining < actualToRead) {
        actualToRead = remaining;
    }

    *num_actually_read = file -> get_buffer(buffer, actualToRead);

    if (*num_actually_read == num_to_read) {
        return Status(Status::kOkCompleted);
    } else {
        return Status(Status::kOkPartial);
    }
}

Status FileAccessReader::Skip(uint64_t num_to_skip, uint64_t* num_actually_skipped) {
    // Modified assertions from FileReader
    if (num_to_skip <= 0) return handleError("FileAccessReader cannot skip 0 bytes");
    if (num_actually_skipped == nullptr) return handleError("FileAccessReader skip count is null");


    if (file == nullptr || file -> eof_reached()) {
        *num_actually_skipped = 0;
        return Status(Status::kEndOfFile);
    }

    size_t actualToSkip = num_to_skip;
    
    uint64_t remaining = (file -> get_length()) - (file -> get_position());
    if (remaining < actualToSkip) {
        actualToSkip = remaining;
    }

    uint64_t initPos = file -> get_position();
    file -> seek(initPos + actualToSkip);
    *num_actually_skipped = (file -> get_position()) - initPos;

    if (*num_actually_skipped == num_to_skip) {
        return Status(Status::kOkCompleted);
    } else {
        return Status(Status::kOkPartial);
    }
}

uint64_t FileAccessReader::Position() const {
    return (file -> get_position());
}
