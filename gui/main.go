package main

import (
	"encoding/json"
	"log"

	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"

	"net/http"
)

var settings struct {
	ESP struct {
		Enabled       bool `json:"enabled"`
		IgnoreCloaked bool `json:"ignorecloaked"`
	} `json:"esp"`
}

func main() {
	go func() {
		sendSettingsHandler := func(w http.ResponseWriter, req *http.Request) {
			response, err := json.Marshal(settings)
			if err != nil {
				w.Write([]byte("oh no"))
				return
			}

			if response == nil {
				w.Write([]byte("oh no 2"))
				return
			}

			w.Write(response)
		}

		http.HandleFunc("/", sendSettingsHandler)
		log.Fatal(http.ListenAndServe(":6969", nil))
	}()

	a := app.New()
	w := a.NewWindow("Vapo Linux")

	w.SetContent(container.NewVBox(
		GroupV("ESP",
			CreateToggle("Enabled", &settings.ESP.Enabled),
			CreateToggle("Ignore Cloaked Spies", &settings.ESP.IgnoreCloaked),
		),
	))
	w.ShowAndRun()
}
