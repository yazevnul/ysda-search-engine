#include "serialization.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message_lite.h>

#include <exception>
#include <cstdint>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


void WriteDelimitedTo(
    const google::protobuf::MessageLite& message,
    google::protobuf::io::ZeroCopyOutputStream* raw_output
){
    google::protobuf::io::CodedOutputStream output(raw_output);

    // Write the size.
    const int size = message.ByteSize();
    output.WriteVarint32(size);

    uint8_t* buffer = output.GetDirectBufferForNBytesAndAdvance(size);
    if (nullptr != buffer) {
        // Optimization:  The message fits in one buffer, so use the faster
        // direct-to-array serialization path.
        message.SerializeWithCachedSizesToArray(buffer);
    } else {
        // Slightly-slower path when the message is multiple buffers.
        message.SerializeWithCachedSizes(&output);
        if (output.HadError()) {
            throw std::runtime_error("failed to write delimiter protobuf message");
        }
    }
}


bool ReadDelimitedFrom(
    google::protobuf::io::ZeroCopyInputStream* rawInput,
    google::protobuf::MessageLite* message
){
    google::protobuf::io::CodedInputStream input(rawInput);

    // Read the size.
    uint32_t size;
    if (!input.ReadVarint32(&size))
        return false;

    // Tell the stream not to read beyond that size.
    auto limit = input.PushLimit(size);

    // Parse the message.
    if (!message->MergePartialFromCodedStream(&input))
        return false;

    if (!input.ConsumedEntireMessage())
        return false;

    // Release the limit.
    input.PopLimit(limit);

    return true;
}


void WriteDelimitedToFile(
    const google::protobuf::MessageLite& message,
    const std::string& file_name
){
    int fd = open(file_name.c_str(), O_WRONLY | O_CREAT, 0666);
    if (-1 == fd)
        throw std::runtime_error("open failed");

    try {
        google::protobuf::io::FileOutputStream output(fd);
        WriteDelimitedTo(message, &output);
    } catch (const std::exception& exc) {
        if (-1 == close(fd))
            throw std::runtime_error("close failed");

        throw exc;
    }

    if (-1 == close(fd))
        throw std::runtime_error("close failed");
}


void ReadDelimitedFromFile(const std::string& file_name, google::protobuf::MessageLite& message) {
    int fd = open(file_name.c_str(), O_RDONLY);
    if (-1 == fd)
        throw std::runtime_error("open failed");

    google::protobuf::io::FileInputStream input(fd);
    const bool return_value = ReadDelimitedFrom(&input, &message);

    if (-1 == close(fd))
        throw std::runtime_error("close failed");

    if (!return_value)
        throw std::runtime_error("failed to parse protobuf message");
}
