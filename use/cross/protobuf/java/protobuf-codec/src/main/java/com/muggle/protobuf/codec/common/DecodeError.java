package com.muggle.protobuf.codec.common;

public class DecodeError {
    public static final String LenBeyondLimit = "byte total length beyond limit";
    public static final String LenTooSmall = "total length is too small";
    public static final String NameLenBeyondLimit = "name length beyond limit";
    public static final String NameLenTooSmall = "name length less than or equal to zero";
    public static final String UnsupportedVersion = "unsupported version";
    public static final String UnsupportedNameEncoding = "unsupported encoding in name";
    public static final String FailedParseName = "invalid protobuf name";
    public static final String FailedParseData = "invalid protobuf data";
}
