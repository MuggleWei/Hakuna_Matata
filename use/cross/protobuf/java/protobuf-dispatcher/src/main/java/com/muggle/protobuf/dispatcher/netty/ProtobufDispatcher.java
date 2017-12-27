package com.muggle.protobuf.dispatcher.netty;

import com.google.protobuf.Message;
import io.netty.channel.ChannelHandlerContext;

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

    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        ProtobufCallback callback = callbacks.get(msg.getDescriptorForType().getFullName());
        if (callback != null) {
            callback.OnMessage(ctx, msg);
        } else if (defaultCallback != null) {
            defaultCallback.OnMessage(ctx, msg);
        }
    }
}
