package dc_common

type MessageResponseHeader struct {
	ErrCode    int         `json:"err_code"`
	ErrMessage string      `json:"err_msg"`
	Data       interface{} `json:"data,omitempty"`
}
