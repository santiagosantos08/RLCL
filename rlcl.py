import subprocess
import threading
import re
import os
import sys
import subprocess
from pystray import Icon, MenuItem, Menu
from PIL import Image

startupinfo = subprocess.STARTUPINFO()
startupinfo.dwFlags |= subprocess.STARTF_USESHOWWINDOW  # Hide the console

CLI_PATH = "./ChargeThreshold.exe"
ICON_ON = "cl_on.png"
ICON_OFF = "cl_off.png"

DEFAULT_START = 75
DEFAULT_STOP = 80

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
        return os.path.join(sys._MEIPASS, filename)  # PyInstaller temp dir
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

        # Check there is a 5% gap, not strictly required, remove at your own risk. I haven't tested stop < start
        can_inc_start = self.start + 5 <= self.stop - 5
        can_dec_stop = self.stop - 5 >= self.start + 5

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
            MenuItem("Quit", self.exit_app)
        )
        self.icon.menu = self.menu

    def set_values(self, direction, target):
        if target == "start":
            self.start = max(0, min(100, self.start + (5 if direction == "inc" else -5)))
        elif target == "stop":
            self.stop = max(0, min(100, self.stop + (5 if direction == "inc" else -5)))
        if self.status:
            run_cli("on", str(self.stop), str(self.start))
        self.check_status()
        self.update_icon()
        self.update_menu()

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