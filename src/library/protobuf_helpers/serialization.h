#pragma once

#include <string>

namespace google {
    namespace protobuf {
        class MessageLite;

        namespace io {
            class ZeroCopyInputStream;
            class ZeroCopyOutputStream;
        }  // namespace io
    }  // namespace protobuf
}  // namespace google


void WriteDelimitedTo(
    const google::protobuf::MessageLite& message,
    google::protobuf::io::ZeroCopyOutputStream* raw_output
);

bool ReadDelimitedFrom(
    google::protobuf::io::ZeroCopyInputStream* raw_input,
    google::protobuf::MessageLite* message
);

void WriteDelimitedToFile(
const google::protobuf::MessageLite& message,
    const std::string& file_name
);

void ReadDelimitedFromFile(const std::string& file_name, google::protobuf::MessageLite& message);

template <typename T>
inline T ReadDelimitedFromFile(const std::string& file_name) {
    T value;
    ReadDelimitedFromFile(file_name, value);
    return value;
}
