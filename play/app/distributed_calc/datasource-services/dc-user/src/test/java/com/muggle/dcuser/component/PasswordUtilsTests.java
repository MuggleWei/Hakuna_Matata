package com.muggle.dcuser.component;

import com.muggle.dcuser.componenet.PasswordUtils;
import org.junit.jupiter.api.Assertions;
import org.junit.jupiter.api.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.test.context.SpringBootTest;

import java.security.NoSuchAlgorithmException;
import java.util.Random;

@SpringBootTest
public class PasswordUtilsTests {
    private final Logger logger = LoggerFactory.getLogger(this.getClass());

    @Autowired
    private PasswordUtils passwordUtils;

    @Test
    public void testBcryptPassword() throws NoSuchAlgorithmException {
        for (int i = 0; i < 16; i++) {
            Random random = new Random();
            int passwordLen = random.nextInt(24) + 8;

            String plaintext = getRandomStringByLength(passwordLen);
            String hashed = passwordUtils.bcryptHash(plaintext);

            Assertions.assertTrue(passwordUtils.bcryptCheck(plaintext, hashed));
            Assertions.assertTrue(hashed.length() < 64);
        }

    }

    public static String getRandomStringByLength(int length) {
        String base = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        Random random = new Random();
        StringBuffer sb = new StringBuffer();
        for (int i = 0; i < length; i++) {
            int number = random.nextInt(base.length());
            sb.append(base.charAt(number));
        }
        return sb.toString();
    }
}
