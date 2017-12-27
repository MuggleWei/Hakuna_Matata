package com.muggle.protobuf.dispatcher.local;

import com.google.protobuf.Message;

public interface ProtobufCallback {
    void OnMessage(Message message);
}
