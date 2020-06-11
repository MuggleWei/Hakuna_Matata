package com.muggle.use.jbcrypt;

import org.mindrot.jbcrypt.BCrypt;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

/**
 * bcrypt(SHA512(password), salt, cost)
 */
public class BCryptSha512 {
    public static void main(String[] args) throws NoSuchAlgorithmException {
        String password = "123456";

        // SHA512(password)
        MessageDigest md = MessageDigest.getInstance("SHA-512");
        byte[] sha512PasswordBytes = md.digest(password.getBytes());
        String sha512Password = bytesToHexString(sha512PasswordBytes);

        for (int i = 0; i < 16; i++) {
            // bcrypt
            String hashed = BCrypt.hashpw(sha512Password, BCrypt.gensalt(10));

            if (BCrypt.checkpw(sha512Password, hashed)) {
                System.out.println("SHA512(\"" + password + "\")" + " match " + hashed);
            } else {
                System.out.println("SHA512(\"" + password + "\")" + " not match " + hashed);
            }
        }
    }

    public static String bytesToHexString(byte[] bArray) {
        StringBuilder sb = new StringBuilder(bArray.length);
        String sTemp;
        for (byte b : bArray) {
            sTemp = Integer.toHexString(0xFF & b);
            if (sTemp.length() < 2) {
                sb.append(0);
            }
            sb.append(sTemp);
        }
        return sb.toString();
    }
}
