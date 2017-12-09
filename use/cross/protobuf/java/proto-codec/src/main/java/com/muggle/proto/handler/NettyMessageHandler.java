package com.muggle.proto.handler;

import com.google.protobuf.Message;
import com.muggle.proto.codec.MessageRet;
import com.muggle.proto.codec.NettyDecode;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;

import java.util.HashMap;
import java.util.Map;

public class NettyMessageHandler {

    private Map<String, NettyProtoCallback> callbacks;
    private NettyProtoCallback defaultCallback;

    NettyDecode decode;

    public NettyMessageHandler(String descFileName) {
        decode = new NettyDecode();
        callbacks = new HashMap<>();

        if (descFileName != null && !descFileName.isEmpty()) {
            decode.AddDesc(descFileName);
        }
    }

    public void Register(Message msg, NettyProtoCallback callback) {
        decode.AddProtoType(msg);
        String msgTypeName = msg.getDescriptorForType().getFullName();
        callbacks.put(msgTypeName, callback);
    }

    public void RegisterDefault(NettyProtoCallback callback) {
        defaultCallback = callback;
    }

    public void OnMessage(ChannelHandlerContext ctx, ByteBuf in) {
        while (in.isReadable()) {
            MessageRet messageRet = decode.Deserialize(in);
            if (messageRet != null && messageRet.message != null) {
                OnMessage(ctx, messageRet.message);
            }
        }
    }

    public void OnMessage(ChannelHandlerContext ctx, Message msg) {
        NettyProtoCallback callback = callbacks.get(msg.getDescriptorForType().getFullName());
        if (callback != null) {
            callback.OnMessage(ctx, msg);
        } else if (defaultCallback != null) {
            defaultCallback.OnMessage(ctx, msg);
        }
    }

    public void UseDefaultCallback() {
        defaultCallback = new DefaultCallback();
    }
}
