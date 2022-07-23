package model

type Response struct {
	Code    int         `json:"code"`
	Message string      `json:"msg,omitempty"`
	Data    interface{} `json:"data,omitempty"`
}
