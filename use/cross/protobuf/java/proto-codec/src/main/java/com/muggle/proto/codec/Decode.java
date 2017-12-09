package com.muggle.proto.codec;

import com.google.protobuf.*;

import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

public class Decode extends Codec {
    public static final int ParseError_InvalidFlag = -1;
    public static final int ParseError_LenBeyondLimit = -2;
    public static final int ParseError_LenTooSmall = -3;
    public static final int ParseError_NameLenBeyondLimit = -4;
    public static final int ParseError_NameLenTooSmall = -5;
    public static final int ParseError_NotSupportedVersion = -6;
    public static final int ParseError_FailedParseName = -7;
    public static final int ParseError_InvalidDataLen = -8;
    public static final int ParseError_FailedParseData = -9;

    protected Map<String, Descriptors.Descriptor> descriptors;
    protected Map<String, Message> messageMap;

    public Decode() {
        descriptors = new HashMap<>();
        messageMap = new HashMap<>();
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

    public DynamicMessage CreateDynamicMessage(String name, byte[] data) {
        Descriptors.Descriptor desc = descriptors.get(name);
        if (desc == null) {
            return null;
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
            e.printStackTrace();
        }

        return msg;
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
            e.printStackTrace();
        }

        return message;
    }
}
