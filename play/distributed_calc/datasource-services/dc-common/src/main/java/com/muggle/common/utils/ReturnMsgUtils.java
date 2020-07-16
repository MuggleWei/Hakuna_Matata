package com.muggle.common.utils;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.util.HashMap;
import java.util.Map;

@Component
public class ReturnMsgUtils {
    @Autowired
    private ErrorIdUtils errorIdUtils;

    public Map<String, Object> success(Object obj) {
        Map<String, Object> mapObj = new HashMap<>();
        mapObj.put("error_id", 0);
        mapObj.put("error_msg", "");
        mapObj.put("data", obj);
        return mapObj;
    }

    public Map<String, Object> failedMap(Object data, int errId) {
        Map<String, Object> mapObj = new HashMap<>();
        mapObj.put("error_msg", errorIdUtils.getErrorMessage(errId));
        mapObj.put("error_id", errId);
        if (data != null) {
            mapObj.put("data", data);
        }
        return mapObj;
    }
}
