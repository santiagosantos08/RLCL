# RLCL: **RedLine Charge Limits**  
*Vantage-free charge limits for ThinkPads*

---

## 🚀 Features

- **Set charge limits**: Easily toggle charge limits on or off.
- **Custom charge limit**: Set a custom charge percentage for your battery.
---

## 📦 Installation

### 🛠️ Prerequisites
- **SINGLE BATTERY THINKPAD**: The Lenovo CLI only works for single battery models, this being a GUI wrapper, has the same shortcomings.
- **Lenovo Power Manager driver**: Make sure you have the official Lenovo Power Manager driver installed.
- **PowerShell**: Ensure PowerShell is working on your system.

### 🔽 Download & Install

1. **Download the latest version** from the [Releases Page](link-pending).
2. Run the installer and follow the prompts.

   **OR**

3. **Build from source** (instructions below).

---

### ⚙️ How It Works

RLCL is a simple **GUI** for an official Lenovo CLI tool.

- Lenovo's ChargeThreshold CLI:  
  [ChargeThreshold.exe](https://download.lenovo.com/pccbbs//thinkvantage_en/metroapps/Vantage/ChargeThreshold/ChargeThreshold.exe)

- The tool will be automatically downloaded during installation, or you'll need to provide your own if you build it from source.

- The app is built using **Python** and packaged into an `.exe` using **PyInstaller**.

- The installer is created using **Inno Setup**.

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
### 3. Build the executable
```
pyinstaller --noconfirm --onefile --windowed rlcl.py
```
- This will create an `.exe` in the `dist` directory: `dist\rlcl.exe`.
- The executable will look for **ChargeThreshold.exe** (the official Lenovo CLI) in the same directory, provide your own, linked above.

### 4. Extras

By default, the binary installer will install to:  
`C:\Program Files\RedLine\ChargeLimits`
But you can do as you like, as long as the CLI is in the same dir.
If you build it from source, you can figure out how to make it launch at boot :D.

---

### 💡 Built with ❤️ in Tandil, Argentina  
**Stand for the right to repair and software freedom.**
