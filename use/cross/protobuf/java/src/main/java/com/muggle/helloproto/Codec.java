package com.muggle.helloproto;

import com.google.protobuf.*;

import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.HashMap;
import java.util.Map;

public class Codec {

    private static final String version = "0.0.1";
    private static final int nameLenLimit = 128;

    private Map<String, Descriptors.Descriptor> descriptors;

    public Codec() {
        String fileName = "proto/desc/proto.desc";
        descriptors = new HashMap<>();

        ClassLoader classloader = Thread.currentThread().getContextClassLoader();
        try (InputStream inputStream = classloader.getResourceAsStream(fileName)) {
            DescriptorProtos.FileDescriptorSet descriptorSet = DescriptorProtos.FileDescriptorSet.parseFrom(inputStream);
            for (DescriptorProtos.FileDescriptorProto fdp : descriptorSet.getFileList()) {
                Descriptors.FileDescriptor desc = Descriptors.FileDescriptor.buildFrom(fdp, new Descriptors.FileDescriptor[]{});
                for (Descriptors.Descriptor descriptor : desc.getMessageTypes()) {
                    String className = descriptor.getFullName();
                    this.descriptors.put(className, descriptor);
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        } catch (Descriptors.DescriptorValidationException e) {
            e.printStackTrace();
        }
    }

    public byte[] Serialize(Message msg) {
        /*
         |          int32_t               |      16 byte     |     int32_t    |   char*   |    void*   |
	     |   total_len(include itself)    |   version info   |    name_len    |   name    |    data    |
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

    public Message Deserialize(byte[] bytes) {
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
            return null;
        }

        // name
        byte[] nameBytes = new byte[nameLen];
        byteBuffer.get(nameBytes);

        // get descriptor
        String name = null;
        try {
            name = new String(nameBytes, "utf-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }
        Descriptors.Descriptor desc = getProtoDescriptor(name);

        // parse
        AbstractMessage msg = null;
        int dataLen = totalLen - (4 + 16 + 4 + nameLen);
        try {
            if (dataLen > 0) {
                byte[] data = new byte[dataLen];
                byteBuffer.get(data);
                msg = DynamicMessage.parseFrom(desc, data);
            } else {
                msg = DynamicMessage.newBuilder(desc).build();
            }
        } catch (InvalidProtocolBufferException e) {
            e.printStackTrace();
        }

        return msg;
    }

    private Descriptors.Descriptor getProtoDescriptor(String messageName) {
        return descriptors.get(messageName);
    }
}
