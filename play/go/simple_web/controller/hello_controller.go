package controller

import (
	"encoding/json"
	"fmt"
	"io"
	"net/http"
	"sync"

	"example.com/hello/model"
	log "github.com/sirupsen/logrus"
)

type HelloController struct{}

var (
	singletonHello *HelloController
	onceHello      sync.Once
)

func GetHelloController() *HelloController {
	if singletonHello == nil {
		onceHello.Do(func() {
			singletonHello = &HelloController{}
		})
	}
	return singletonHello
}

func (this *HelloController) Greet(w http.ResponseWriter, r *http.Request) {
	var req model.ReqGreetModel
	err := json.NewDecoder(r.Body).Decode(&req)
	if err != nil {
		log.Warning("failed parse body: %v", err.Error())
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	if len(req.Name) == 0 {
		errMsg := fmt.Sprintf("failed get field 'name'")
		log.Warning(errMsg)
		http.Error(w, errMsg, http.StatusBadRequest)
		return
	}

	res := &model.Response{
		Code: 0,
		Data: model.RspGreetModel{
			Greet: fmt.Sprintf("hello %v", req.Name),
		},
	}
	HttpResponse(w, res)
}

func (this *HelloController) Echo(w http.ResponseWriter, r *http.Request) {
	b, err := io.ReadAll(r.Body)
	// The server will close the request body, handler does not need to
	// defer r.Body.Close()
	if err != nil {
		log.Warning("failed read http body: %v", err.Error())
		http.Error(w, err.Error(), http.StatusBadRequest)
		return
	}

	w.Write(b)
}
