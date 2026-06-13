# 🎓 Face Attendance System

> Real-time face recognition based attendance tracking system built with **C++** and **OpenCV 4.13**

---

## 📌 What it does

This system uses your webcam to automatically detect and recognize faces, then logs attendance into a CSV file — no manual entry needed.

```
Camera opens → Face detected → Name recognized → Attendance marked ✅
```

Each session creates a new timestamped log file so records never get mixed up.

---

## ✨ Features

- 🔍 **Real-time face detection** using Haar Cascade classifier
- 🧠 **Face recognition** using LBPH (Local Binary Patterns Histogram) algorithm
- 📋 **Auto attendance logging** to CSV with name, date and time
- 🗂️ **Per-session log files** — new file created every time camera opens
- 🚫 **Duplicate prevention** — each person logged only once per session
- ⚡ **Runs at 30fps** — smooth real-time performance

---

## 🛠️ Tech Stack

| Tool | Purpose |
|---|---|
| C++17 | Core programming language |
| OpenCV 4.13 | Computer vision — camera, detection, recognition |
| CMake | Build system |
| Haar Cascade | Face detection algorithm |
| LBPH Algorithm | Face recognition algorithm |

---

## 📁 Project Structure

```
face-attendance/
├── src/
│   ├── main.cpp          # Entry point — train or recognize mode
│   ├── trainer.cpp/h     # Loads photos, trains LBPH model
│   ├── recognizer.cpp/h  # Webcam loop, face detection + recognition
│   └── logger.cpp/h      # CSV attendance writer
├── data/
│   ├── haarcascade_frontalface_default.xml
│   └── known_faces/      # Training photos (one folder per person)
├── models/               # Saved trained model
├── logs/                 # Attendance CSV files
└── CMakeLists.txt
```

---

## 🚀 Setup & Run

### Prerequisites
- macOS with Homebrew
- OpenCV 4.x (`brew install opencv`)
- CMake (`brew install cmake`)
- Xcode Command Line Tools (`xcode-select --install`)

### Step 1 — Clone the repo
```bash
git clone https://github.com/YOUR_USERNAME/face-attendance.git
cd face-attendance
```

### Step 2 — Get the cascade file
```bash
cp /opt/homebrew/share/opencv4/haarcascades/haarcascade_frontalface_default.xml data/
```

### Step 3 — Add training photos
```bash
mkdir data/known_faces/your_name
# Add 20-30 photos of the person into that folder
```

### Step 4 — Build
```bash
cmake -S . -B build
cmake --build build
```

### Step 5 — Train the model
```bash
cd build
./face_attendance train
```

### Step 6 — Start attendance
```bash
./face_attendance recognize
```

---

## 🎮 Controls

| Key | Action |
|---|---|
| `Q` or `ESC` | Quit |
| `S` | Print attendance summary |

---

## 📊 Attendance Log

Every session generates a new CSV file in `logs/`:

```
logs/attendance_2026-06-13_09-32-00.csv
```

Contents:
```
Name,Date,Time
Subham Jalan,2026-06-13,09:32:14
Alice,2026-06-13,09:33:01
```

---

## 🧠 How it works

### Face Detection
Uses OpenCV's **Haar Cascade** classifier — a sliding window algorithm that scans the image for contrast patterns that match a human face (dark eyes, lighter cheeks, etc.)

### Face Recognition
Uses **LBPH (Local Binary Patterns Histogram)**:
1. Each pixel is compared to its 8 neighbours → converted to a binary pattern
2. Image is divided into regions, each region gets a histogram of patterns
3. All histograms combined = face "fingerprint"
4. Recognition = compare fingerprints, lowest difference = identity

### Attendance Logging
- Each recognized person is logged once per session (duplicate prevention via in-memory set)
- New CSV file created per session with timestamp in filename

---

## 📸 Demo

> Camera opens → detects face → draws bounding box → displays name + confidence → logs to CSV

Green box = recognized person ✅  
Red box = unknown person ❌

---

## 🔮 Planned Features

- [ ] Deep learning face recognition (dlib) for higher accuracy
- [ ] Live enrollment — add new person without restarting
- [ ] Web dashboard to view attendance logs
- [ ] Email report at end of session
- [ ] Anti-spoofing detection

---

## 👤 Author

**Subham Jalan**  
GitHub: [@subhamjalan-spec](https://github.com/subhamjalan-spec)

---

## 📄 License

MIT License — feel free to use and modify