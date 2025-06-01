# RLCL: **RedLine Charge Limits**  
*Vantage-free charge limits for ThinkPads*

## 🚀 Features
- **Set charge limits**: Easily toggle charge limits on or off.
- **Custom charge limits**: Set custom charge percentages for your battery.

![alt text]( "screenshot of RLCL")

## 📦 Installation

### 🛠️ Prerequisites / Considerations
- **SINGLE BATTERY THINKPAD**: The Lenovo CLI only works for single battery models, this being a GUI wrapper, has the same shortcomings.
- **Lenovo Power Manager driver**: Make sure you have the official Lenovo Power Manager driver installed.
- **PowerShell**: Ensure PowerShell is working on your system.
- **64bit Windows**: Current release is x64 only, for 32bit you can build it manually or just run as a python script.

### 🔽 Download & Install

1. **Download the latest version** from the [Releases Page](https://github.com/santiagosantos08/RLCL/releases/).
2. Run the installer and follow the prompts.

   **OR**

3. **Build from source** (instructions below).

### ⚙️ How It Works

RLCL is a simple **GUI** for an official Lenovo CLI tool.

- Lenovo's ChargeThreshold CLI:  
  [ChargeThreshold.exe](https://download.lenovo.com/pccbbs//thinkvantage_en/metroapps/Vantage/ChargeThreshold/ChargeThreshold.exe)

- The tool will be automatically downloaded during installation, or you'll need to provide your own if you build it from source.

- The app is built using **Python** and packaged into an `.exe` using **PyInstaller**.

- The installer is created using **Inno Setup** and manages desktop shortcuts, launch at boot, etc.

---

## 🛠️ Build Instructions

### 1. Clone this repository
```
git clone https://github.com/santiagosantos08/RLCL
cd RLCL
```
### 2. Install required Python packages
```
pip install pillow pystray pyinstaller
```
(pyinstaller only required if you want to bundle it to an .exe, running as a script is also fine, if that's the case, you are done here, just run rlcl.py with python, make sure to put Lenovo's "ChargeThreshold.exe" in the same dir, i can't redistribute it, that's why it needs to be downloaded at setup. It is linked above in the 'how it works' section.)
### 3. Build the executable
```
pyinstaller rlcl.spec
```
- This will create an `.exe`.
- The executable will look for **ChargeThreshold.exe** (the official Lenovo CLI) in the same directory, provide your own, linked above.

### 4. Extras
If you also want to build the installer, just get [Inno Setup](https://jrsoftware.org/isinfo.php) and compile `rlcl_installer.iss` with it.

If you don't want to build the installer, take in to consideration:

By default, the binary installer will install to:  
`C:\Program Files\RedLine\ChargeLimits`

But you can do as you like, as long as the CLI is in the same dir.

If you build it from source, you can figure out how to make it launch at boot :D.

---

### 💡 Built with ❤️ in Tandil, Argentina  
**Stand for the right to repair and software freedom.**
