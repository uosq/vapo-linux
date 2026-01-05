package main

import (
	"encoding/json"
	"log"

	"fyne.io/fyne/v2"
	"fyne.io/fyne/v2/app"
	"fyne.io/fyne/v2/container"

	"net/http"
)

var settings struct {
	ESP struct {
		Enabled       bool `json:"enabled"`
		IgnoreCloaked bool `json:"ignorecloaked"`
		Buildings     bool `json:"buildings"`
		Name          bool `json:"name"`
		Box           bool `json:"box"`
		Chams         bool `json:"chams"`
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
		Thirdperson       bool    `json:"thirdperson"`
		CustomFOV_Enabled bool    `json:"customfov_enabled"`
		CustomFOV         float64 `json:"customfov"`
		SpectatorList     bool    `json:"spectatorlist"`
		BackpackExpander  bool    `json:"backpack_expander"`
		SVPureBypass      bool    `json:"sv_pure_bypass"`
		StreamerMode      bool    `json:"streamer_mode"`
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
	a.Settings().SetTheme(&myTheme{})
	w := a.NewWindow("Vapo Linux")
	w.Resize(fyne.NewSize(800, 400))

	w.SetContent(container.NewVScroll(container.NewBorder(
		nil, nil, nil, nil,
		container.NewVBox(
			container.NewGridWithColumns(2,
				GroupV("Aimbot",
					CreateToggle("Enabled", &settings.Aimbot.Enabled),
					CreateToggle("Autoshoot", &settings.Aimbot.Autoshoot),
					CreateEntry("Key", &settings.Aimbot.Key),
					CreateSlider("Fov", &settings.Aimbot.Fov, 0, 180),
					CreateSlider("Max Sim Time", &settings.Aimbot.MaxSimTime, 0, 5),
					CreateToggle("Viewmodel Aim", &settings.Aimbot.ViewModelAim),
				),
				GroupV("ESP",
					CreateToggle("Enabled", &settings.ESP.Enabled),
					CreateToggle("Ignore Cloaked Spies", &settings.ESP.IgnoreCloaked),
					CreateToggle("Buildings", &settings.ESP.Buildings),
					CreateToggle("Name", &settings.ESP.Name),
					CreateToggle("Box", &settings.ESP.Box),
					CreateToggle("Chams", &settings.ESP.Chams),
				),
			),

			GroupH("Misc",
				container.NewGridWithColumns(4,
					CreateToggle("Third person", &settings.Misc.Thirdperson),
					CreateToggle("Backpack Expander", &settings.Misc.BackpackExpander),
					CreateToggle("Spectator List", &settings.Misc.SpectatorList),
					CreateToggle("sv_pure bypass", &settings.Misc.SVPureBypass),
					CreateToggle("Streamer Mode", &settings.Misc.StreamerMode),
				),
			),

			GroupV("Custom Fov",
				CreateToggle("Enabled", &settings.Misc.CustomFOV_Enabled),
				CreateSlider("Fov", &settings.Misc.CustomFOV, 70, 140),
			),
		),
	)))

	w.ShowAndRun()
}
