package com.muggle.dcuser.componenet;

import org.mindrot.jbcrypt.BCrypt;
import org.springframework.stereotype.Component;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

@Component
public class PasswordUtils {
    public String bcryptHash(String plaintext) throws NoSuchAlgorithmException {
        // SHA512(password)
        MessageDigest md = MessageDigest.getInstance("SHA-512");
        md.update(plaintext.getBytes());
        byte[] sha512PasswordBytes = md.digest();
        String sha512Password = bytesToHexString(sha512PasswordBytes);

        // Bcrypt(SHA512(password))
        return BCrypt.hashpw(sha512Password, BCrypt.gensalt(10));
    }

    public boolean bcryptCheck(String plaintext, String hashed) throws NoSuchAlgorithmException {
        // SHA512(password)
        MessageDigest md = MessageDigest.getInstance("SHA-512");
        md.update(plaintext.getBytes());
        byte[] sha512PasswordBytes = md.digest();
        String sha512Password = bytesToHexString(sha512PasswordBytes);

        return BCrypt.checkpw(sha512Password, hashed);
    }

    public String md5CryptPassword(String salt, String plainText) throws NoSuchAlgorithmException {
        return md5(md5(salt), md5(plainText));
    }

    private String md5(String plainText) throws NoSuchAlgorithmException {
        return md5(null, plainText);
    }

    private String md5(String salt, String plainText)
            throws NoSuchAlgorithmException {
        MessageDigest md = MessageDigest.getInstance("MD5");

        if (salt != null) {
            md.update(salt.getBytes());
        }
        md.update(plainText.getBytes());

        byte[] byteData = md.digest();

        StringBuilder sb = new StringBuilder();
        for (byte byteDatum : byteData) {
            sb.append(Integer.toString((byteDatum & 0xff) + 0x100, 16)
                    .substring(1));
        }
        return sb.toString();
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
