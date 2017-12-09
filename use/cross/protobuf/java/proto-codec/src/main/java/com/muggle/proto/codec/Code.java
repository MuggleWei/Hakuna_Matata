package com.muggle.proto.codec;

import com.google.protobuf.Message;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Code {
    public static final String version = "0.0.1";

    public byte[] Serialize(Message msg) {
        return Serialize(msg, 0);
    }

    public byte[] Serialize(Message msg, int flag) {
        switch (flag) {
            case 0: {
                return Serialize0(msg);
            }
        }
        return null;
    }

    private byte[] Serialize0(Message msg) {
        /*
         |    int32_t    |             int32_t             |      16 byte     |     int32_t    |   char*   |    void*   |
	     |     flag      |            total_len            |   version info   |    name_len    |   name    |    data    |
	     |               | include itself and without flag |                  |                |           |            |
	     */
        byte[] bytes = msg.toByteArray();
        String msgFullName = msg.getDescriptorForType().getFullName();
        int nameLen = msgFullName.length();
        int totalLen = 4 + 16 + 4 + nameLen + bytes.length;

        ByteBuffer byteBuffer = ByteBuffer.allocate(totalLen + 4);
        byteBuffer.order(ByteOrder.BIG_ENDIAN);

        // flag
        byteBuffer.putInt(0);

        // total len
        byteBuffer.putInt(totalLen);

        // version info
        byte[] versionBytesPrepared = new byte[16];
        byte[] versionBytes = version.getBytes();
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
        byteBuffer.put(msgFullName.getBytes());

        // data
        byteBuffer.put(bytes);

        return byteBuffer.array();
    }
}
