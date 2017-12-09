package com.muggle.proto.codec;

import com.google.protobuf.Message;

import java.io.UnsupportedEncodingException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.LinkedList;
import java.util.List;

public class ByteDecode extends Decode {

    public boolean IsSupportedVersion(byte[] version) {
        return true;
    }

    public List<Message> Deserialize(byte[] bytes) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(bytes.length);
        byteBuffer.order(ByteOrder.BIG_ENDIAN);
        byteBuffer.put(bytes);
        byteBuffer.clear();

        // flag
        List<Message> messageList = new LinkedList<>();
        while (true) {
            if (byteBuffer.remaining() < 4) {
                break;
            }

            MessageRet msg = Deserialize(byteBuffer);
            if (msg != null && msg.message != null) {
                messageList.add(msg.message);
            }
            if (msg.parseLen <= 0) {
                break;
            }
        }

        return messageList;
    }

    public MessageRet Deserialize(ByteBuffer byteBuffer) {
        MessageRet messageRet = null;
        int flag = byteBuffer.getInt();
        switch (flag) {
            case 0: {
                messageRet = Deserialize0(byteBuffer);
            }
            break;
            default: {
                messageRet = new MessageRet();
                messageRet.parseLen = ParseError_InvalidFlag;
            }
            break;
        }

        return messageRet;
    }

    private MessageRet Deserialize0(ByteBuffer byteBuffer) {
        MessageRet messageRet = new MessageRet();

        // readable bytes
        int readableBytes = byteBuffer.remaining();
        if (readableBytes < 4) {
            messageRet.parseLen = 0;
            return null;
        }

        // total len
        int totalLen = byteBuffer.getInt();
        if (totalLen > readableBytes) {
            messageRet.parseLen = ParseError_LenBeyondLimit;
            return null;
        }
        if (totalLen > getTotalLenLimit()) {
            messageRet.parseLen = ParseError_LenBeyondLimit;
            return null;
        }
        if (totalLen < 4 + 16 + 4) {
            messageRet.parseLen = ParseError_LenTooSmall;
            return null;
        }

        // version
        byte[] versionBytesPrepared = new byte[16];
        byteBuffer.get(versionBytesPrepared);
        if (!IsSupportedVersion(versionBytesPrepared)) {
            messageRet.parseLen = ParseError_NotSupportedVersion;
            return null;
        }

        // name len
        int nameLen = byteBuffer.getInt();
        if (nameLen > getNameLenLimit()) {
            messageRet.parseLen = ParseError_NameLenBeyondLimit;
            return null;
        }
        if (nameLen <= 0) {
            messageRet.parseLen = ParseError_NameLenTooSmall;
            return null;
        }
        if (totalLen < 4 + 16 + 4 + nameLen) {
            messageRet.parseLen = ParseError_LenTooSmall;
            return null;
        }

        // name
        byte[] nameBytes = new byte[nameLen];
        byteBuffer.get(nameBytes);
        String name = null;
        try {
            name = new String(nameBytes, "utf-8");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
            messageRet.parseLen = ParseError_FailedParseName;
            return null;
        }

        // data
        int dataLen = totalLen - (4 + 16 + 4 + nameLen);
        if (dataLen > 0) {
            byte[] data = new byte[dataLen];
            byteBuffer.get(data);
            messageRet.message = Parse(name, data);
        } else if (dataLen == 0) {
            messageRet.message = Parse(name, null);
        } else {
            messageRet.parseLen = ParseError_InvalidDataLen;
            return null;
        }

        if (messageRet.message == null) {
            messageRet.parseLen = ParseError_FailedParseData;
        } else {
            messageRet.parseLen = totalLen;
        }

        return messageRet;
    }

}
