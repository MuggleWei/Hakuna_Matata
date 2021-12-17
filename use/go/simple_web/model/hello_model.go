package model

type ReqGreetModel struct {
	Name string `json:"name"`
}

type RspGreetModel struct {
	Greet string `json:"greet"`
}
