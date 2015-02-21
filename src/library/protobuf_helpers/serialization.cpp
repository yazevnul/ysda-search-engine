#include "serialization.h"

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/message_lite.h>

#include <exception>

#include <cstdint>


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
    if (!input.ReadVarint32(&size)) {
        return false;
    }

    // Tell the stream not to read beyond that size.
    auto limit = input.PushLimit(size);

    // Parse the message.
    if (!message->MergePartialFromCodedStream(&input)) {
        return false;
    }

    if (!input.ConsumedEntireMessage()) {
        return false;
    }

    // Release the limit.
    input.PopLimit(limit);

    return true;
}

