#pragma once

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
