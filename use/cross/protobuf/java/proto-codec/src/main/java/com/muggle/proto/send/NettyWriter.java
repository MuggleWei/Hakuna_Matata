package com.muggle.proto.send;

import com.google.protobuf.Message;
import com.muggle.proto.codec.Code;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;

public class NettyWriter {

    private Code code = new Code();

    public void write(ChannelHandlerContext ctx, Message msg) {
        ctx.write(Serialize(ctx, msg));
    }

    public void writeAndFlush(ChannelHandlerContext ctx, Message msg) {
        ctx.writeAndFlush(Serialize(ctx, msg));
    }

    private ByteBuf Serialize(ChannelHandlerContext ctx, Message msg) {
        byte[] bytes = code.Serialize(msg);
        ByteBuf byteBuf = ctx.alloc().buffer(bytes.length);
        byteBuf.writeBytes(bytes);
        return byteBuf;
    }
}
