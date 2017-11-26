package com.muggle.helloproto;

import com.google.protobuf.Message;

import java.util.HashMap;
import java.util.Map;

public class Dispatcher {

    private Map<String, ProtoCallback> callbacks;

    public Dispatcher() {
        callbacks = new HashMap<>();
    }

    public void Register(String msgTypeName, ProtoCallback callback) {
        callbacks.put(msgTypeName, callback);
    }

    public void OnMessage(Message msg) {
        ProtoCallback callback = callbacks.get(msg.getClass().getName());
        if (callback != null) {
            callback.OnMessage(msg);
        }
    }
}
