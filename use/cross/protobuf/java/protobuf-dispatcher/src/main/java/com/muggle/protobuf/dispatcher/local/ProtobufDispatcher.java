package com.muggle.protobuf.dispatcher.local;

import com.google.protobuf.Message;

import java.util.HashMap;
import java.util.Map;

public class ProtobufDispatcher {
    private Map<String, ProtobufCallback> callbacks = new HashMap<>();
    private ProtobufCallback defaultCallback;

    public void Register(Message message, ProtobufCallback callback) {
        String msgTypeName = message.getDescriptorForType().getFullName();
        callbacks.put(msgTypeName, callback);
    }

    public void RegisterDefault(ProtobufCallback callback) {
        defaultCallback = callback;
    }

    public void OnMessage(Message msg) {
        ProtobufCallback callback = callbacks.get(msg.getDescriptorForType().getFullName());
        if (callback != null) {
            callback.OnMessage(msg);
        } else if (defaultCallback != null) {
            defaultCallback.OnMessage(msg);
        }
    }
}
