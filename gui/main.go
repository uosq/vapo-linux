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

	Aimbot struct {
		Enabled bool    `json:"enabled"`
		Key     string  `json:"key"`
		Fov     float64 `json:"fov"`
	} `json:"aimbot"`
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
		GroupV("Aimbot",
			CreateToggle("Enabled", &settings.Aimbot.Enabled),
			CreateEntry("Key", &settings.Aimbot.Key),
			CreateSlider("Fov", &settings.Aimbot.Fov, 0, 180),
		),

		GroupV("ESP",
			CreateToggle("Enabled", &settings.ESP.Enabled),
			CreateToggle("Ignore Cloaked Spies", &settings.ESP.IgnoreCloaked),
		),
	))
	w.ShowAndRun()
}
