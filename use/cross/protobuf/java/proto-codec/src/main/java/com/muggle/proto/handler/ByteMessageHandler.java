package com.muggle.proto.handler;

import com.google.protobuf.Message;
import com.muggle.proto.codec.ByteDecode;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class ByteMessageHandler {

    private Map<String, ByteProtoCallback> callbacks;
    private ByteProtoCallback defaultCallback;

    ByteDecode decode;

    public ByteMessageHandler(String descFileName) {
        decode = new ByteDecode();
        callbacks = new HashMap<>();

        if (descFileName != null && !descFileName.isEmpty()) {
            decode.AddDesc(descFileName);
        }
    }

    public void Register(Message msg, ByteProtoCallback callback) {
        decode.AddProtoType(msg);
        String msgTypeName = msg.getDescriptorForType().getFullName();
        callbacks.put(msgTypeName, callback);
    }

    public void RegisterDefault(ByteProtoCallback callback) {
        defaultCallback = callback;
    }

    public void OnMessage(byte[] bytes) {
        List<Message> messageList = decode.Deserialize(bytes);
        for (Message msg : messageList) {
            OnMessage(msg);
        }
    }

    public void OnMessage(Message msg) {
        ByteProtoCallback callback = callbacks.get(msg.getDescriptorForType().getFullName());
        if (callback != null) {
            callback.OnMessage(msg);
        } else if (defaultCallback != null) {
            defaultCallback.OnMessage(msg);
        }
    }
}
