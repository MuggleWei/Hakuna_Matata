package com.muggle.codec;

import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.ByteToMessageDecoder;

import java.util.List;

public class MessageDecode extends ByteToMessageDecoder {
    @Override
    protected void decode(ChannelHandlerContext ctx, ByteBuf in, List<Object> out) throws Exception {
        if (in.readableBytes() < 4) {
            return;
        }

        int len = in.getInt(0);
        if (in.readableBytes() < len + 4) {
            return;
        }

        len = in.readInt();
        byte bytes[] = new byte[len];
        in.readBytes(bytes);
        String str = new String(bytes, "UTF-8");

        out.add(str);
    }
}
