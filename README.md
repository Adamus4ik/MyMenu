# Geometry Dash Mod Menu (Open Source)

⚠️ **PROJECT STATUS: EARLY ALPHA / WORK IN PROGRESS** ⚠️

This project is an open-source, completely free alternative to paid mod menus like MegaHack. The goal is to provide the Geometry Dash community with a safe, transparent, and highly customizable tool that includes advanced AI-powered features.

> **Note:** The current code in this repository is an early, non-functional prototype. I am actively fixing the core logic and memory hooks. I am using Claude to help debug the code and finish the development.

## 🚧 Current Status (What is broken / not working yet)

Please note that the mod menu is currently **not ready for use**.
- [ ] **Memory Hooks (C++):** The core hooks for game memory are currently broken and need to be rewritten to match the latest GD version.
- [ ] **UI/ImGui:** The menu interface works partially but causes crashes when injecting into the game.
- [ ] **MegaHack-style Features:** Speed hack, layout tools, and other classic mods are currently non-functional (requires fixing the memory addresses).
- [ ] **AI Level Maker:** The algorithm is in development and not yet implemented in the actual game code.
- [ ] **AI Layout Maker:** Not working.
- [ ] **Stability:** The mod may crash the game upon injection.

## 🎯 Planned Features (Roadmap)

Once the core is fixed, the project will include:
1. **Classic Mod Features:** Better optimized, free versions of classic MegaHack features (speed hacks, level customization, practice tools, etc.).
2. **AI Level Maker:** Generate levels using AI.
3. **AI Layout Maker:** Create layouts automatically.
4. **Open Source:** Fully transparent code so anyone can audit it for safety and contribute.

## 🛠️ How to Build / Test (For Developers)

*Coming soon.* Once the base code is fixed, build instructions for Windows will be added here.

## 🤝 Contributing

Contributions, bug reports, and feature requests are welcome! Since the project is in early alpha, please wait until the core structure is stable before submitting Pull Requests.

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.
