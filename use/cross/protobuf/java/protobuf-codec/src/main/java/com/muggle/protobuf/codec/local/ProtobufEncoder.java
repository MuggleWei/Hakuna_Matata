package com.muggle.protobuf.codec.local;

import com.google.protobuf.Message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.Charset;

public class ProtobufEncoder {

    public static final String version = "0.0.1";

    public byte[] encode(Message msg) {
        /*
         |      int32_t       |      16 byte     |     int32_t    |   char*   |    void*   |
	     |     total_len      |   version info   |    name_len    |   name    |    data    |
	     |   include itself   |                  |                |           |            |
	     */
        byte[] bytes = msg.toByteArray();
        String msgFullName = msg.getDescriptorForType().getFullName();
        int nameLen = msgFullName.length();
        int totalLen = 4 + 16 + 4 + nameLen + bytes.length;

        ByteBuffer byteBuffer = ByteBuffer.allocate(totalLen);
        byteBuffer.order(ByteOrder.BIG_ENDIAN);

        // total len
        byteBuffer.putInt(totalLen);

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
        byteBuffer.put(versionBytesPrepared);

        // name len
        byteBuffer.putInt(nameLen);

        // name
        byteBuffer.put(msgFullName.getBytes(Charset.forName("UTF-8")));

        // data
        byteBuffer.put(bytes);

        return byteBuffer.array();
    }
}
