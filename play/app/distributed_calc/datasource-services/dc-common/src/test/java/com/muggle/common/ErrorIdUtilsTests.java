package com.muggle.common;

import com.muggle.common.utils.ErrorIdUtils;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;

@SpringBootTest
public class ErrorIdUtilsTests {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private ErrorIdUtils errorIdUtils;

    private void errMessageExists(int errId) {
        String errorMessage = errorIdUtils.getErrorMessage(errId);
        Assertions.assertNotEquals(errorMessage, errorIdUtils.UnknownErrMessage);
    }

    @Test
    public void testErrMsg() {
        errMessageExists(errorIdUtils.ok);

        errMessageExists(errorIdUtils.errIdInvalidUser);
        errMessageExists(errorIdUtils.errIdPasswordLen);
        errMessageExists(errorIdUtils.errIdRepeatedUserName);
    }
}
