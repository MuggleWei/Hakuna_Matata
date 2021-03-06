package com.muggle.protobuf.codec.netty;

import com.google.protobuf.Message;
import com.muggle.protobuf.codec.common.DecodeError;
import io.netty.buffer.ByteBuf;
import io.netty.channel.ChannelHandlerContext;
import io.netty.handler.codec.ByteToMessageDecoder;

import java.io.UnsupportedEncodingException;
import java.util.List;

public class ProtobufDecoder extends ByteToMessageDecoder {

    private com.muggle.protobuf.codec.common.Decoder decoder = new com.muggle.protobuf.codec.common.Decoder();

    @Override
    protected void decode(ChannelHandlerContext ctx, ByteBuf in, List<Object> out) throws Exception {
        if (in.readableBytes() < 4) {
            return;
        }

        // total len
        int totalLen = in.getInt(in.readerIndex());
        if (totalLen > decoder.getTotalLenLimit()) {
            throw new IllegalArgumentException(DecodeError.LenBeyondLimit);
        }
        if (totalLen > in.readableBytes()) {
            return;
        }
        if (totalLen < 4 + 4) {
            throw new IllegalArgumentException(DecodeError.LenTooSmall);
        }
        totalLen = in.readInt();

        // name len
        int nameLen = in.readInt();
        if (nameLen > decoder.getNameLenLimit()) {
            throw new IllegalArgumentException(DecodeError.NameLenBeyondLimit);
        }
        if (nameLen <= 0) {
            throw new IllegalArgumentException(DecodeError.NameLenTooSmall);
        }
        if (totalLen < 4 + 4 + nameLen) {
            throw new IllegalArgumentException(DecodeError.LenTooSmall);
        }

        // name
        byte[] nameBytes = new byte[nameLen];
        if (nameLen > in.readableBytes()) {
            throw new IllegalArgumentException(DecodeError.RestByteLTNameLen);
        }
        in.readBytes(nameBytes);
        String name = null;
        try {
            name = new String(nameBytes, "utf-8");
        } catch (UnsupportedEncodingException e) {
            throw new IllegalArgumentException(DecodeError.UnsupportedNameEncoding);
        }

        // data
        Message message = null;
        int dataLen = totalLen - (4 + 4 + nameLen);
        if (dataLen > 0) {
            if (dataLen > in.readableBytes()) {
                throw new IllegalArgumentException(DecodeError.RestByteLTDataLen);
            }
            byte[] data = new byte[dataLen];
            in.readBytes(data);
            message = decoder.Parse(name, data);
        } else if (dataLen == 0) {
            message = decoder.Parse(name, null);
        }

        if (message == null){
            throw new IllegalArgumentException(DecodeError.FailedParseData);
        }

        out.add(message);
    }

    public int getTotalLenLimit() {
        return decoder.getTotalLenLimit();
    }

    public void setTotalLenLimit(int totalLenLimit) {
        decoder.setTotalLenLimit(totalLenLimit);
    }

    public int getNameLenLimit() {
        return decoder.getNameLenLimit();
    }

    public void setNameLenLimit(int nameLenLimit) {
        decoder.setNameLenLimit(nameLenLimit);
    }

    public void AddDesc(String fileName) {
        decoder.AddDesc(fileName);
    }

    public void AddProtoType(Message message) {
        decoder.AddProtoType(message);
    }

}
