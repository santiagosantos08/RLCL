import subprocess
import threading
import re
import os
import sys
import json
from pystray import Icon, MenuItem, Menu
from PIL import Image
from functools import partial

startupinfo = subprocess.STARTUPINFO()
startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW  # Hide the console

CLI_PATH = "./ChargeThreshold.exe"
ICON_ON = "cl_on.png"
ICON_OFF = "cl_off.png"
APP_DATA_DIR = os.path.join(os.getenv('APPDATA'), 'RLCLApp')
PRESET_FILE = os.path.join(APP_DATA_DIR, "presets.json")

DEFAULT_START = 75
DEFAULT_STOP = 80
DEFAULT_PRESETS = []

def run_cli(*args):
    try:
        result = subprocess.run([CLI_PATH] + list(args), startupinfo=startupinfo, capture_output=True, text=True)
        print(result.stdout)
        return result.stdout.strip()
    except Exception as e:
        print(f"Error running CLI: {e}")
        return ""

def resource_path(filename):
    if hasattr(sys, '_MEIPASS'):
        # If running as a bundled executable, _MEIPASS is the temp folder
        return os.path.join(sys._MEIPASS, filename)
        # Otherwise, assume it's in the current script's directory
    return os.path.join(os.path.abspath("."), filename)

def load_image(filename):
    return Image.open(resource_path(filename))

def parse_status(output):
    match = re.search(r"Start at (\d+)%.*, Stop at (\d+)%", output)
    if match:
        return {"on": True, "start": int(match.group(1)), "stop": int(match.group(2))}
    elif "OFF" in output:
        return {"on": False}
    return None

class RLCLApp:
    def __init__(self):
        self.icon = None
        self.status = None
        self.start = DEFAULT_START
        self.stop = DEFAULT_STOP

        self.presets = self.load_presets()
        self.load_icons()
        self.check_status()
        self.setup_menu()

    def load_icons(self):
        self.icon_on = load_image(ICON_ON)
        self.icon_off = load_image(ICON_OFF)

    def check_status(self):
        output = run_cli("status")
        parsed = parse_status(output)
        if parsed:
            self.status = parsed["on"]
            if self.status:
                self.start = parsed["start"]
                self.stop = parsed["stop"]
        else:
            self.status = False

    def load_presets(self):
        os.makedirs(APP_DATA_DIR, exist_ok=True)
        if not os.path.exists(PRESET_FILE):
            with open(PRESET_FILE, "w") as f:
                json.dump(DEFAULT_PRESETS, f)
            return DEFAULT_PRESETS
        else:
            try:
                with open(PRESET_FILE, "r") as f:
                    loaded_raw = json.load(f)
                    # convert all inner lists (from JSON) to tuples
                    # this ensures consistency with how presets are created in add_current_preset and how they are compared/removed.
                    # otherwise it would behave differently for the ones parsed from the file and the ones created from the menu
                    return [tuple(p) for p in loaded_raw]
            except Exception as e:
                print("Error loading presets:", e)
                return DEFAULT_PRESETS

    def save_presets(self):
        try:
            with open(PRESET_FILE, "w") as f:
                json.dump(self.presets, f)
        except Exception as e:
            print("Error saving presets:", e)

    def toggle_limit(self, icon=None, item=None):
        if self.status:
            run_cli("off")
        else:
            run_cli("on", str(self.stop), str(self.start))
        self.check_status()
        self.update_icon()
        self.update_menu()

    def update_icon(self):
        self.icon.icon = self.icon_on if self.status else self.icon_off

    def update_menu(self):
        toggleLabel = f"{'[ ON ✔️ ] - OFF' if self.status else 'ON - [OFF ✔️]'}"
        can_inc_start = self.start + 5 <= self.stop - 5
        can_dec_stop = self.stop - 5 >= self.start + 5
        preset_items = [
            MenuItem(
                f"{s}% - {e}%",
                Menu(
                    MenuItem("✔️ Activate", partial(self.apply_preset, s, e)),
                    MenuItem("❌ Delete", partial(self.delete_preset, s, e))
                )
            )
            for s, e in self.presets
        ]

        self.menu = Menu(
            MenuItem(toggleLabel, self.toggle_limit),
            Menu.SEPARATOR,
            MenuItem(f"Current start: {self.start}%", lambda: None),
            MenuItem("➕ Increase 5%", lambda: self.set_values("inc", "start"), enabled=can_inc_start),
            MenuItem("➖ Decrease 5%", lambda: self.set_values("dec", "start")),
            Menu.SEPARATOR,
            MenuItem(f"Current stop: {self.stop}%", lambda: None),
            MenuItem("➕ Increase 5%", lambda: self.set_values("inc", "stop")),
            MenuItem("➖ Decrease 5%", lambda: self.set_values("dec", "stop"), enabled=can_dec_stop),
            Menu.SEPARATOR,
            MenuItem("💾 Save current as preset", self.add_current_preset),
            Menu.SEPARATOR,
            MenuItem("Presets", Menu(*preset_items)) if preset_items else MenuItem("Presets", Menu(MenuItem("No presets", lambda: None, enabled=False))),
            Menu.SEPARATOR,
            MenuItem("Quit", self.exit_app)
        )
        self.icon.menu = self.menu

    def set_values(self, direction, target):
        if target == "start":
            # Ensure start stays within 0-100 and doesn't exceed stop - 5
            self.start = max(0, min(100, self.start + (5 if direction == "inc" else -5)))
            self.start = min(self.start, self.stop - 5)
        elif target == "stop":
            # Ensure stop stays within 0-100 and doesn't go below start + 5
            self.stop = max(0, min(100, self.stop + (5 if direction == "inc" else -5)))
            self.stop = max(self.stop, self.start + 5)

        if self.status:
            run_cli("on", str(self.stop), str(self.start))
        self.check_status()
        self.update_icon()
        self.update_menu()

    def add_current_preset(self, icon=None, item=None):
        new_preset = (self.start, self.stop)
        if new_preset not in self.presets:
            self.presets.append(new_preset)
            self.save_presets()
            self.update_menu()

    def apply_preset(self, s, e, icon=None, item=None):
        self.start = s
        self.stop = e
        if self.status:
            run_cli("on", str(self.stop), str(self.start))
        self.check_status()
        self.update_icon()
        self.update_menu()

    def delete_preset(self, s, e, icon=None, item=None):
        try:
            self.presets.remove((s, e))
            self.save_presets()
            self.update_menu()
        except ValueError:
            pass

    def exit_app(self, icon=None, item=None):
        self.icon.stop()

    def setup_menu(self):
        self.icon = Icon("rlcl", self.icon_on if self.status else self.icon_off)
        self.update_menu()

    def run(self):
        threading.Thread(target=self.icon.run).start()

if __name__ == "__main__":
    app = RLCLApp()
    app.run()