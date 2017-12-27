package com.muggle.protobuf.codec.common;

import com.google.protobuf.*;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

public class Decoder {
    protected Map<String, Descriptors.Descriptor> descriptors = new HashMap<>();;
    protected Map<String, Message> messageMap = new HashMap<>();
    protected int totalLenLimit = 4096;
    protected int nameLenLimit = 128;

    public int getTotalLenLimit() {
        return totalLenLimit;
    }

    public void setTotalLenLimit(int totalLenLimit) {
        this.totalLenLimit = totalLenLimit;
    }

    public int getNameLenLimit() {
        return nameLenLimit;
    }

    public void setNameLenLimit(int nameLenLimit) {
        this.nameLenLimit = nameLenLimit;
    }

    public void AddDesc(String fileName) {
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

    public void AddProtoType(Message message) {
        String msgTypeName = message.getDescriptorForType().getFullName();
        messageMap.put(msgTypeName, message);
    }

    public Message Parse(String name, byte[] bytes) {
        // NOTE: there is no inheritance relationship between DynamicMessage
        // and GeneratedMessageV3
        Message message = null;
        try {
            Message msgProtoType = messageMap.get(name);
            if (msgProtoType != null) {
                if (bytes == null) {
                    message = msgProtoType.newBuilderForType().build();
                } else {
                    message = msgProtoType.getParserForType().parseFrom(bytes);
                }
            } else {
                message = CreateDynamicMessage(name, bytes);
            }
        } catch (InvalidProtocolBufferException e) {
            throw new IllegalArgumentException(DecodeError.FailedParseData);
        }

        return message;
    }

    public DynamicMessage CreateDynamicMessage(String name, byte[] data) {
        Descriptors.Descriptor desc = descriptors.get(name);
        if (desc == null) {
            throw new IllegalArgumentException(DecodeError.FailedParseName);
        }

        // parse
        DynamicMessage msg = null;
        try {
            if (data != null && data.length > 0) {
                msg = DynamicMessage.parseFrom(desc, data);
            } else {
                msg = DynamicMessage.newBuilder(desc).build();
            }
        } catch (InvalidProtocolBufferException e) {
            throw new IllegalArgumentException(DecodeError.FailedParseData);
        }

        return msg;
    }


}
