package com.muggle.protobuf.codec.local;

import com.google.protobuf.*;
import com.muggle.protobuf.codec.common.DecodeError;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.*;

public class ProtobufDecoder extends com.muggle.protobuf.codec.common.Decoder {

    public List<Message> decode(byte[] bytes) throws IllegalFormatException {
        ByteBuffer byteBuffer = ByteBuffer.allocate(bytes.length);
        byteBuffer.order(ByteOrder.BIG_ENDIAN);
        byteBuffer.put(bytes);
        byteBuffer.clear();

        List<Message> messageList = new LinkedList<>();
        while (true) {
            if (byteBuffer.remaining() < 4) {
                break;
            }

            Message msg = DecodeSingle(byteBuffer);
            if (msg != null) {
                messageList.add(msg);
            } else {
                break;
            }
        }

        return messageList;
    }

    private Message DecodeSingle(ByteBuffer byteBuffer) throws IllegalFormatException {
        // readable bytes
        int readableBytes = byteBuffer.remaining();
        if (readableBytes < 4) {
            return null;
        }

        // total len
        int totalLen = byteBuffer.getInt();
        if (totalLen > readableBytes) {
            return null;
        }
        if (totalLen > getTotalLenLimit()) {
            throw new IllegalArgumentException(DecodeError.LenBeyondLimit);
        }
        if (totalLen < 4 + 16 + 4) {
            throw new IllegalArgumentException(DecodeError.LenTooSmall);
        }

        // version
        byte[] versionBytesPrepared = new byte[16];
        byteBuffer.get(versionBytesPrepared);
        if (!IsSupportedVersion(versionBytesPrepared)) {
            throw new IllegalArgumentException(DecodeError.UnsupportedVersion);
        }

        // name len
        int nameLen = byteBuffer.getInt();
        if (nameLen > getNameLenLimit()) {
            throw new IllegalArgumentException(DecodeError.NameLenBeyondLimit);
        }
        if (nameLen <= 0) {
            throw new IllegalArgumentException(DecodeError.NameLenTooSmall);
        }
        if (totalLen < 4 + 16 + 4 + nameLen) {
            throw new IllegalArgumentException(DecodeError.LenTooSmall);
        }

        // name
        byte[] nameBytes = new byte[nameLen];
        byteBuffer.get(nameBytes);
        String name = null;
        try {
            name = new String(nameBytes, "utf-8");
        } catch (UnsupportedEncodingException e) {
            throw new IllegalArgumentException(DecodeError.UnsupportedNameEncoding);
        }

        // data
        Message message = null;
        int dataLen = totalLen - (4 + 16 + 4 + nameLen);
        if (dataLen > 0) {
            byte[] data = new byte[dataLen];
            byteBuffer.get(data);
            message = Parse(name, data);
        } else if (dataLen == 0) {
            message = Parse(name, null);
        }

        if (message == null){
            throw new IllegalArgumentException(DecodeError.FailedParseData);
        }

        return message;
    }

    private boolean IsSupportedVersion(byte[] version) {
        return true;
    }
}
