package com.muggle.common.utils;

import org.springframework.stereotype.Component;

@Component
public class ErrorIdUtils {
    public final int ok = 0;

    /***************** common error id *****************/
    public final int errIdNotFound = 1;
    public final int errIdInvalidParam = 2;

    /***************** user service error id *****************/
    public final int errIdUserServBegin = 1000;
    public final int errIdInvalidUser = errIdUserServBegin + 1;
    public final int errIdPasswordLen = errIdUserServBegin + 2;
    public final int errIdRepeatedUserName = errIdUserServBegin + 3;
    public final int errIdPassword = errIdUserServBegin + 4;
    public final int errIdDuplicateName = errIdUserServBegin + 5;
    public final int errIdUserServEnd = errIdUserServBegin + 999;

    public final int endErrId = 10000;

    public final String UnknownErrMessage = "unknown error";

    private final String[] errMessages = new String[endErrId];

    public ErrorIdUtils() {
        errMessages[ok] = "";

        errMessages[errIdNotFound] = "not found";
        errMessages[errIdInvalidParam] = "invalid parameter";

        errMessages[errIdInvalidUser] = "invalid user name";
        errMessages[errIdPasswordLen] = "invalid password length";
        errMessages[errIdRepeatedUserName] = "user name already exists";
        errMessages[errIdPassword] = "user password error";
        errMessages[errIdDuplicateName] = "duplicated user name";
    }

    public String getErrorMessage(int errId) {
        if (errId < 0 || errId >= endErrId) {
            return UnknownErrMessage;
        }

        if (errMessages[errId] == null) {
            return UnknownErrMessage;
        }

        return errMessages[errId];
    }
}
