package main

import (
	"crypto/aes"
	"crypto/cipher"
	"crypto/rand"
	"encoding/hex"
	"fmt"
	"io"
)

func aesEncrypt(key []byte, plaintext []byte) ([]byte, []byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err.Error())
	}

	aesgcm, err := cipher.NewGCM(block)
	if err != nil {
		panic(err.Error())
	}

	// Never use more than 2^32 random nonces with a given key because of the risk of a repeat.
	nonce := make([]byte, 12)
	if _, err := io.ReadFull(rand.Reader, nonce); err != nil {
		panic(err.Error())
	}

	ciphertext := aesgcm.Seal(nil, nonce, plaintext, nil)

	return ciphertext, nonce, nil
}

func aesDecrypt(key []byte, ciphertext, nonce []byte) ([]byte, error) {
	block, err := aes.NewCipher(key)
	if err != nil {
		panic(err.Error())
	}

	aesgcm, err := cipher.NewGCM(block)
	if err != nil {
		panic(err.Error())
	}

	plaintext, err := aesgcm.Open(nil, nonce, ciphertext, nil)
	if err != nil {
		panic(err.Error())
	}

	return plaintext, nil
}

func runAES(plaintext string) {
	fmt.Println("-------- AES GCM --------")

	key := []byte("++++abcd++++abcd++++abcd++++abcd") // AES-256 key

	ciphertext, nonce, err := aesEncrypt(key, []byte(plaintext))
	if err != nil {
		panic(err)
	}
	fmt.Printf("plaintext: %v\n", plaintext)
	fmt.Printf("nonce: %v\n", hex.EncodeToString(nonce))
	fmt.Printf("ciphertext: %v\n", hex.EncodeToString(ciphertext))

	retPlaintext, err := aesDecrypt(key, ciphertext, nonce)
	if err != nil {
		panic(err)
	}
	fmt.Printf("decrypt return plaintext: %v\n", string(retPlaintext))
}
