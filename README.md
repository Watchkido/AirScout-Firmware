# 🌍 AirScout Environmental Monitoring System
**Mobile hyperlocal pollution tracking with Arduino & Python**  
*(Arduino Mega 2560 + 12+ sensors → CSV → Kaggle-ready analytics)*  

---

## 📌 Projektübersicht
Ein **IoT-basiertes Umweltmonitoring-System**, das per Fahrrad/Auto durch Städte fährt und Echtzeitdaten sammelt:
- **Gemessene Parameter:**  
  🌀 Luftqualität (9x MQ-Sensoren: CO/CO2/NOx/LPG/etc.)  
  📍 Präzises GPS-Tracking (200Hz)  
  ☢️ Radioaktivität (Geigerzähler)  
  🔊 Lärmbelastung (Dual-Mikrofon)  
  🌡️ Mikroklima (Temperatur/Luftfeuchte/Licht)  

---

## 🛠️ Hardware (Arduino)
### 📦 Komponenten
```python
Arduino Mega 2560
├─ Gas Sensors: MQ2, MQ3, MQ4, MQ5, MQ6, MQ7, MQ8, MQ9, MQ135 (A0-A8)
├─ GPS: UBlox Neo-6M (Serial1, 200Hz)
├─ Strahlung: RadiationD v1.1 Geigerzähler (Pin 29)
├─ Audio: 2x MEMS-Mikrofone (A9, A10)
├─ Klima: DHT11 (Pin 22) + DS18B20 (Pin 8)
└─ Logging: SD-Karte (SPI) + RTC (I2C)
```

### ⚙️ Firmware-Features
- **Multisensor-Sampling:**  
  ```cpp
  void loop() {
    readGasSensors();  // 0.5Hz (MQ-Serie)
    readRadiation();   // 200Hz (Interrupt)
    logGPS();         // 200Hz (Serial1)
  }
  ```
- **Speichereffizienz:** SD-Karten-Logging mit RTC-Zeitstempel  
- **Plug & Play:** Kalibrierungsskripte in `/calibration`  

---

## 🐍 Python-Datenpipeline
### 📂 Ordnerstruktur
```
AirScout-Data-Fusion/
├── data_cleaning/       # CSV Preprocessing
├── analysis/           # Jupyter Notebooks
├── visualization/      # Plotly/Karten
└── export/             # Kaggle/GeoJSON
```

### 🔥 Key Skripte
```python
1. csv_analyser.py       # Erzeugt Heatmaps und Grafiken für alle Sensoren
```

### 🚀 Schnellstart
```bash
pip install -r requirements.txt  # pandas, plotly, scipy
python pipeline.py --input data/raw --output kaggle/
```

---

## 📊 Beispiel-Daten (Auszug)
| timestamp_ms | mq135_raw | gps_lat  | radiation_cps | 
|--------------|-----------|----------|---------------|
| 21517-433    | 256       | 49.3526  | 0             |
| 21519-493    | 293       | 49.3526  | 0             |

---

## 🌟 Nächste Schritte
- [ ] Kalibrierungskurven für MQ-Sensoren dokumentieren  
- [ ] Kaggle-Datensatz unter [CC-BY 4.0](https://creativecommons.org/licenses/by/4.0/) veröffentlichen  
- [ ] Tutorial für Bürgerwissenschaftler erstellen  

---

## 📜 Lizenz
MIT License - Freie Nutzung für Forschung & Citizen Science.  
*Bei Verwendung bitte Quellenangabe:* **"AirScout Project (2025)"**  

---

**📫 Kontakt**  
Frank Albrecht | airscout@watchkido.de  
