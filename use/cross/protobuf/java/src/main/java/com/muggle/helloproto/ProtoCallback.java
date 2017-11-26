package com.muggle.helloproto;

import com.google.protobuf.Message;

public interface ProtoCallback {
    void OnMessage(Message msg);
}
