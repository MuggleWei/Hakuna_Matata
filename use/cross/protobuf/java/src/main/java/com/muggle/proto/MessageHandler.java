package com.muggle.proto;

import com.google.protobuf.DynamicMessage;
import com.google.protobuf.InvalidProtocolBufferException;
import com.google.protobuf.Message;

import java.util.HashMap;
import java.util.Map;

public class MessageHandler {

    private Map<String, ProtoCallback> callbacks;
    private Map<String, Message> messageMap;
    private ProtoCallback defaultCallback;
    protected Codec codec;

    public MessageHandler() {
        callbacks = new HashMap<>();
        messageMap = new HashMap<>();
        codec = new Codec();
    }

    public <E extends Message> void Register(E msg, ProtoCallback callback) {
        String msgTypeName = msg.getDescriptorForType().getFullName();
        callbacks.put(msgTypeName, callback);
        messageMap.put(msgTypeName, msg);
    }

    public void RegisterDefault(ProtoCallback callback) {
        defaultCallback = callback;
    }

    public void OnMessage(Message msg) {
        ProtoCallback callback = callbacks.get(msg.getDescriptorForType().getFullName());
        if (callback != null) {
            callback.OnMessage(msg);
        } else if (defaultCallback != null) {
            defaultCallback.OnMessage(msg);
        }
    }

    public void OnMessage(String name, byte[] bytes) {
        Message message = null;
        try {
            Message msgProtoType = messageMap.get(name);
            if (msgProtoType != null) {
                if (bytes == null) {
                    message = msgProtoType.newBuilderForType().build();
                } else {
                    message = msgProtoType.getParserForType().parseFrom(bytes);
                }
                OnMessage(message);
            } else {
                DynamicMessage dynamicMessage = codec.Deserialize(name, bytes);
                if (dynamicMessage != null) {
                    // NOTE: this can only use default callback, cause there
                    // have no inheritance relationship between DynamicMessage
                    // and GeneratedMessageV3
                    defaultCallback.OnMessage(dynamicMessage);
                }
            }
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }
    }
}
