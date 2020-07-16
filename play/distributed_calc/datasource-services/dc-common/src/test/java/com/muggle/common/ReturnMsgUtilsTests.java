package com.muggle.common;

import com.muggle.common.utils.ErrorIdUtils;
import com.muggle.common.utils.ReturnMsgUtils;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;

import java.util.Map;

@SpringBootTest
public class ReturnMsgUtilsTests {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private ReturnMsgUtils returnMsgUtils;

    @Autowired
    private ErrorIdUtils errorIdUtils;

    private void checkReturnMessage(int errId) {
        Map<String, Object> retObj;
        retObj = returnMsgUtils.failedMap(null, errId);

        Assertions.assertEquals(retObj.get("error_id"), errId);
        String errMsg = (String) retObj.get("error_msg");
        Assertions.assertEquals(errMsg, errorIdUtils.getErrorMessage(errId));
    }

    @Test
    public void testReturnMsg() {
        Map<String, Object> retObj;
        retObj = returnMsgUtils.success(null);
        Assertions.assertEquals(retObj.get("error_id"), 0);

        checkReturnMessage(errorIdUtils.errIdInvalidUser);
        checkReturnMessage(errorIdUtils.errIdPasswordLen);
        checkReturnMessage(errorIdUtils.errIdRepeatedUserName);
    }
}
