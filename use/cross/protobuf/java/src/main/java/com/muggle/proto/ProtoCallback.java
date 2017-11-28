package com.muggle.proto;

import com.google.protobuf.Message;

public interface ProtoCallback {
    void OnMessage(Message msg);
}
