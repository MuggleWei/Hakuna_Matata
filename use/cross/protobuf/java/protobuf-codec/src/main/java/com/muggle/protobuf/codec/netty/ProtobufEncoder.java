package com.muggle.protobuf.codec.netty;

import com.google.protobuf.Message;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.MessageToByteEncoder;

import java.nio.charset.Charset;

public class ProtobufEncoder extends MessageToByteEncoder<Message> {

    public static final String version = "0.0.1";

    @Override
    protected void encode(ChannelHandlerContext channelHandlerContext, Message message, ByteBuf byteBuf) throws Exception {
        /*
         |      int32_t       |      16 byte     |     int32_t    |   char*   |    void*   |
	     |     total_len      |   version info   |    name_len    |   name    |    data    |
	     |   include itself   |                  |                |           |            |
	     */
        byte[] bytes = message.toByteArray();
        String msgFullName = message.getDescriptorForType().getFullName();
        int nameLen = msgFullName.length();
        int totalLen = 4 + 16 + 4 + nameLen + bytes.length;

        byteBuf.ensureWritable(totalLen);

        // total len
        byteBuf.writeInt(totalLen);

        // version info
        byte[] versionBytesPrepared = new byte[16];
        byte[] versionBytes = version.getBytes(Charset.forName("UTF-8"));
        int length = versionBytes.length >= 16 ? 15 : versionBytes.length;
        int i = 0;
        for (i = 0; i < length; ++i) {
            versionBytesPrepared[i] = versionBytes[i];
        }
        for (; i < 16; ++i) {
            versionBytesPrepared[i] = '\0';
        }
        byteBuf.writeBytes(versionBytesPrepared);

        // name len
        byteBuf.writeInt(nameLen);

        // name
        byteBuf.writeBytes(msgFullName.getBytes(Charset.forName("UTF-8")));

        // data
        byteBuf.writeBytes(bytes);
    }


}
