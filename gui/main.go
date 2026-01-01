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
		Enabled      bool    `json:"enabled"`
		Key          string  `json:"key"`
		Fov          float64 `json:"fov"`
		Autoshoot    bool    `json:"autoshoot"`
		MaxSimTime   float64 `json:"max_sim_time"`
		ViewModelAim bool    `json:"viewmodelaim"`
	} `json:"aimbot"`

	Misc struct {
		Thirdperson bool    `json:"thirdperson"`
		CustomFOV   float64 `json:"customfov"`
	} `json:"misc"`
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
			CreateToggle("Autoshoot", &settings.Aimbot.Autoshoot),
			CreateEntry("Key", &settings.Aimbot.Key),
			CreateSlider("Fov", &settings.Aimbot.Fov, 0, 180),
			CreateSlider("Max Simulation Time", &settings.Aimbot.MaxSimTime, 0, 5),
			CreateToggle("ViewModel Aim", &settings.Aimbot.ViewModelAim),
		),

		GroupV("ESP",
			CreateToggle("Enabled", &settings.ESP.Enabled),
			CreateToggle("Ignore Cloaked Spies", &settings.ESP.IgnoreCloaked),
		),

		GroupV("Misc",
			CreateToggle("ThirdPerson", &settings.Misc.Thirdperson),
			CreateSlider("Custom FOV", &settings.Misc.CustomFOV, 70, 140),
		),
	))
	w.ShowAndRun()
}
