package com.muggle.proto.handler;

import com.google.protobuf.Message;

public interface ByteProtoCallback {
    void OnMessage(Message msg);
}
