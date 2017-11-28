package com.muggle.proto;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Receiver {
    protected MessageHandler messageHandler;
    private static final int nameLenLimit = 128;

    public Receiver() {
        messageHandler = new MessageHandler();
    }

    public void Parse(byte[] bytes) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(bytes.length);
        byteBuffer.order(ByteOrder.BIG_ENDIAN);
        byteBuffer.put(bytes);
        byteBuffer.clear();

        // total len
        int totalLen = byteBuffer.getInt();

        // version
        byte[] versionBytesPrepared = new byte[16];
        byteBuffer.get(versionBytesPrepared);

        // name len
        int nameLen = byteBuffer.getInt();
        if (nameLen > nameLenLimit) {
            return;
        }

        // name
        byte[] nameBytes = new byte[nameLen];
        byteBuffer.get(nameBytes);
        String name = null;
        try {
            name = new String(nameBytes, "utf-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            return;
        }

        int dataLen = totalLen - (4 + 16 + 4 + nameLen);
        if (dataLen > 0) {
            byte[] data = new byte[dataLen];
            byteBuffer.get(data);
            messageHandler.OnMessage(name, data);
        } else {
            messageHandler.OnMessage(name, null);
        }
    }

}
