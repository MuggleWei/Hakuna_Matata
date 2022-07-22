package dc_message_captcha

type MessageCaptchaVerify struct {
	CaptchaSessionID string `json:"k"`
	CaptchaValue     string `json:"v"`
}
