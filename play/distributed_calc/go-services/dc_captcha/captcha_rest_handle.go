package main

import (
	"bytes"
	"encoding/json"
	"errors"
	"fmt"
	"log"
	"net/http"
	"strconv"
	"time"

	"github.com/MuggleWei/Hakuna_Matata/play/distributed_calc/go-services/dc_common"
	msg_captcha "github.com/MuggleWei/Hakuna_Matata/play/distributed_calc/go-services/dc_message_captcha"
	"github.com/dchest/captcha"
	uuid "github.com/satori/go.uuid"
)

type CaptchaRestHandle struct {
	Components *CaptchaComponents
	Config     *CaptchaConfig
}

func (this *CaptchaRestHandle) Load(w http.ResponseWriter, r *http.Request) {
	log.Println("/captcha/load")

	sessionID := r.URL.Query().Get("captcha_session")
	if sessionID == "" {
		sessionID = this.GenUuid()
		cookie := http.Cookie{Name: "captcha_session", Value: sessionID, MaxAge: this.Config.ExpireSecond}
		http.SetCookie(w, &cookie)
	}

	content, val, err := this.GenCaptcha()
	if err != nil {
		log.Println("[err] failed generate captcha")
		panic(err)
	}

	err = this.SaveCaptchaVal(val, sessionID)
	if err != nil {
		log.Println("[err] failed save captcha")
		panic(err)
	}

	w.Header().Set("Content-Type", "image/jpeg")
	http.ServeContent(w, r, "", time.Time{}, bytes.NewReader(content.Bytes()))
}

func (this *CaptchaRestHandle) Verify(w http.ResponseWriter, r *http.Request) {
	log.Println("/captcha/verify")

	var msg msg_captcha.MessageCaptchaVerify
	this.Components.RestUtils.ReadPostBody(r, &msg)

	statusCmd := this.Components.RedisClient.Get(this.Components.RedisCtx, msg.CaptchaSessionID)
	rsp := dc_common.MessageResponseHeader{}
	if statusCmd.Err() == nil && statusCmd.Val() == msg.CaptchaValue {
		rsp.ErrCode = 0
		rsp.ErrMessage = ""
	} else {
		rsp.ErrCode = -1
		rsp.ErrMessage = "failed verify captcha"
	}

	this.Components.RedisClient.Del(this.Components.RedisCtx, msg.CaptchaSessionID)

	b, err := json.Marshal(rsp)
	if err != nil {
		panic(err)
	}

	fmt.Fprint(w, string(b))
}

func (this *CaptchaRestHandle) GenUuid() string {
	uuid := uuid.NewV4()
	return uuid.String()
}

func (this *CaptchaRestHandle) GenCaptcha() (*bytes.Buffer, []byte, error) {
	d := captcha.RandomDigits(captcha.DefaultLen)

	var content bytes.Buffer
	img := captcha.NewImage("", d, captcha.StdWidth, captcha.StdHeight)
	if img == nil {
		return nil, nil, errors.New("failed to generate captcha")
	}
	_, err := img.WriteTo(&content)
	if err != nil {
		return nil, nil, err
	}

	return &content, d, nil
}

func (this *CaptchaRestHandle) SaveCaptchaVal(val []byte, uuid string) error {
	s := ""
	for _, v := range val {
		s = s + strconv.Itoa(int(v))
	}
	statusCmd := this.Components.RedisClient.Set(this.Components.RedisCtx, uuid, s, time.Second*time.Duration(this.Config.ExpireSecond))
	return statusCmd.Err()
}
