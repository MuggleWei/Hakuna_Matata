package com.muggle.proto.codec;

import com.google.protobuf.Message;
import io.netty.buffer.ByteBuf;

import java.io.UnsupportedEncodingException;

public class NettyDecode extends Decode {

    public boolean IsSupportedVersion(byte[] version) {
        return true;
    }

    public MessageRet Deserialize(ByteBuf byteBuf) {
        MessageRet messageRet = null;
        int flag = byteBuf.getInt(0);
        switch (flag) {
            case 0: {
                messageRet = Deserialize0(byteBuf);
            }
            break;
            default: {
                messageRet = new MessageRet();
                messageRet.parseLen = ParseError_InvalidFlag;
            }
        }

        return messageRet;
    }

    private MessageRet Deserialize0(ByteBuf byteBuf) {
        MessageRet messageRet = new MessageRet();

        // readable bytes
        int readableBytes = byteBuf.readableBytes();
        if (readableBytes < 8) {
            messageRet.parseLen = 0;
            return null;
        }

        // total len
        int totalLen = byteBuf.getInt(4);
        if (totalLen + 4 > readableBytes) {
            messageRet.parseLen = 0;
            return null;
        }
        int flag = byteBuf.readInt();
        totalLen = byteBuf.readInt();
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
        byteBuf.readBytes(versionBytesPrepared);
        if (!IsSupportedVersion(versionBytesPrepared)) {
            messageRet.parseLen = ParseError_NotSupportedVersion;
            return null;
        }

        // name len
        int nameLen = byteBuf.readInt();
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
        byteBuf.readBytes(nameBytes);
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
            byteBuf.readBytes(data);
            messageRet.message = Parse(name, data);
        } else if (dataLen == 0) {
            messageRet.message = Parse(name, null);
        } else {
            messageRet.parseLen = ParseError_InvalidDataLen;
            return null;
        }

        if (messageRet.message == null) {
            messageRet.parseLen = ParseError_FailedParseData;
        }

        return messageRet;
    }


}
