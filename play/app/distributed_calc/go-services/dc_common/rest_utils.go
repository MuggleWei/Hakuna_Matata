package dc_common

import (
	"bytes"
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
)

type RestUtils struct {
}

func (this *RestUtils) ReadPostBody(r *http.Request, obj interface{}) error {
	body, err := ioutil.ReadAll(r.Body)
	if err != nil {
		return err
	}

	err = json.Unmarshal(body, obj)
	if err != nil {
		return err
	}

	return nil
}

func (this *RestUtils) Return(code int, message string, data interface{}, w http.ResponseWriter) {
	rsp := MessageResponseHeader{
		ErrCode:    code,
		ErrMessage: message,
		Data:       data,
	}

	b, err := json.Marshal(rsp)
	if err != nil {
		panic(err)
	}

	fmt.Fprint(w, string(b))
}

func (this *RestUtils) TransportGet(url string, transport http.RoundTripper, w http.ResponseWriter) ([]byte, error) {
	client := &http.Client{Transport: transport}
	rsp, err := client.Get(url)
	if err != nil {
		panic(err)
	}
	defer rsp.Body.Close()

	w.Header().Set("Content-Type", rsp.Header.Get("Content-Type"))
	for _, cookie := range rsp.Cookies() {
		http.SetCookie(w, cookie)
	}

	return ioutil.ReadAll(rsp.Body)
}

func (this *RestUtils) TransportPost(url string, transport http.RoundTripper, obj interface{}, w http.ResponseWriter) ([]byte, error) {
	body, err := json.Marshal(obj)
	if err != nil {
		return nil, err
	}

	return this.TransportPostBytes(url, transport, body, w)
}

func (this *RestUtils) TransportPostBytes(url string, transport http.RoundTripper, body []byte, w http.ResponseWriter) ([]byte, error) {
	client := &http.Client{Transport: transport}
	rsp, err := client.Post(url, "application/json", bytes.NewBuffer(body))
	if err != nil {
		panic(err)
	}
	defer rsp.Body.Close()

	w.Header().Set("Content-Type", rsp.Header.Get("Content-Type"))
	for _, cookie := range rsp.Cookies() {
		http.SetCookie(w, cookie)
	}

	return ioutil.ReadAll(rsp.Body)
}
