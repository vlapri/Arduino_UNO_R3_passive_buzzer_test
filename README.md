# Arduino_UNO_R3_passive_buzzer_test

Jednoduchý program pro Arduino UNO R3 s pasivním buzzerem a dvěma tlačítky – 12 ukázkových melodií/signálů, mezi kterými lze cyklicky listovat.

A simple Arduino UNO R3 program driving a passive buzzer with two buttons – 12 demo melodies/signals you can cycle through.

---

## Čeština

### Co program dělá

Po nahrání a spuštění se nic nepřehrává – program čeká na stisk jednoho ze dvou tlačítek:

- **Tlačítko "Další"** (pin D2) – přejde na další ukázku (cyklicky, po poslední se vrátí na první).
- **Tlačítko "Předchozí"** (pin D3) – přejde na předchozí ukázku (cyklicky, před první se vrátí na poslední).

Po stisku tlačítka se zvolená ukázka celá přehraje a program o tom průběžně informuje přes sériovou linku (Serial monitor, 9600 Bd). Po dohrání se buzzer ztiší a program opět čeká na další stisk tlačítka. Tlačítka zmáčknutá během přehrávání se ignorují – reakce je vždy až na stisk po dohrání ukázky.

### 12 ukázek

1. Hasiči (siréna)
2. Policie (siréna)
3. Záchranka (siréna)
4. Požární poplach
5. Ohrožení (civilní siréna)
6. Red Alert (Star Trek: TNG)
7. Imperial March (Star Wars)
8. Super Mario Bros
9. Tetris (Korobeiniki)
10. Mise: Nemožná
11. Óda na radost (Beethoven)
12. SOS (Morseova abeceda)

Sirény a poplachové signály (1–6, 12) jsou generovány procedurálně (změny frekvence za běhu), skutečné melodie (7–11) jsou uložené jako noty ve flash paměti (PROGMEM).

### Jazyk výpisů do konzole

Na začátku souboru `src/main.cpp` je konstanta:

```cpp
const uint8_t LANGUAGE = LANG_CS; // <-- zde přepnout na LANG_EN pro angličtinu
```

Změnou hodnoty na `LANG_EN` se přepnou všechny výpisy do Serial monitoru do angličtiny.

### Schéma zapojení

```
                     Arduino UNO R3
                  ┌───────────────────┐
   Buzzer (+) ────┤ D8                │
   Buzzer (-) ──┐ │                   │
                │ │                   │
  Tl. Další ────┼─┤ D2                │
  (druhý pin) ──┤ │                   │
                │ │                   │
  Tl. Předchozí ┼─┤ D3                │
  (druhý pin) ──┤ │                   │
                │ │                   │
                └─┤ GND               │
                  └───────────────────┘
```

| Součástka | Pin/vývod | Arduino UNO R3 |
|---|---|---|
| Pasivní buzzer | `+` | D8 |
| Pasivní buzzer | `-` | GND |
| Tlačítko "Další" | vývod 1 | D2 |
| Tlačítko "Další" | vývod 2 | GND |
| Tlačítko "Předchozí" | vývod 1 | D3 |
| Tlačítko "Předchozí" | vývod 2 | GND |

Obě tlačítka používají vnitřní pull-up rezistory (`INPUT_PULLUP`), takže **není potřeba žádný externí rezistor** – stačí propojit jeden vývod tlačítka na příslušný digitální pin a druhý vývod na GND. Tlačítka jsou připojená na piny D2 a D3, které na Arduinu UNO podporují hardwarová přerušení (INT0/INT1), takže reagují okamžitě i během delšího zpracování v `loop()`.

### Technické poznámky (úspora paměti)

- Všechny textové řetězce vypisované do konzole jsou obalené makrem `F()`, takže zůstávají ve flash paměti a nezabírají cenné SRAM (na ATmega328P jen 2 kB).
- Noty melodií (frekvence + délka) jsou uložené v poli struktur v `PROGMEM` a čtou se pomocí `pgm_read_word()`.
- Tabulka ukázek je pole ukazatelů na funkce uložené v `PROGMEM` (`pgm_read_ptr()`), takže se nezabírá zbytečné SRAM ani pro výběr melodie.
- Sirény/poplachy (hasiči, policie, záchranka, požár, ohrožení, Red Alert, SOS) negenerují žádná datová pole – vzniknou přímo výpočtem frekvence za běhu (`tone()` ve smyčce), takže nezabírají skoro žádnou paměť.

### Struktura projektu

```
platformio.ini       # konfigurace PlatformIO (deska, framework)
src/
  main.cpp            # hlavní program
include/
  pitches.h           # definice frekvencí hudebních not
```

### Vývoj a nahrání přes VS Code + PlatformIO

1. Ve VS Code nainstalujte rozšíření **PlatformIO IDE** (repozitář obsahuje `.vscode/extensions.json`, takže VS Code jej sám nabídne k instalaci).
2. Otevřete kořenovou složku repozitáře ve VS Code – PlatformIO automaticky rozpozná `platformio.ini` (deska `uno`, framework `arduino`).
3. Sestavení: PlatformIO panel → *Build*, nebo v terminálu `pio run`.
4. Nahrání do desky: PlatformIO panel → *Upload*, nebo `pio run --target upload`.
5. Sériový monitor (9600 Bd): PlatformIO panel → *Monitor*, nebo `pio device monitor`.

Projekt lze samozřejmě nahrát i klasicky přes Arduino IDE – stačí otevřít `src/main.cpp` a `include/pitches.h` zkopírovat do stejné složky jako `.ino` (nebo přidat cestu `include/` do "Additional Board/Library" vyhledávání).

---

## English

### What the program does

After uploading and starting, nothing plays – the program waits for one of two buttons to be pressed:

- **"Next" button** (pin D2) – moves to the next demo (cyclically; wraps from the last demo back to the first).
- **"Previous" button** (pin D3) – moves to the previous demo (cyclically; wraps from the first demo back to the last).

When a button is pressed, the selected demo plays in full while the program reports what is happening over the serial port (Serial monitor, 9600 baud). Once playback finishes, the buzzer is silenced and the program waits for the next button press again. Button presses that happen while a demo is playing are ignored – the next reaction always happens on a press received after playback has finished.

### The 12 demos

1. Firefighters (siren)
2. Police (siren)
3. Ambulance (siren)
4. Fire alarm
5. Air-raid siren (civil defense)
6. Red Alert (Star Trek: TNG)
7. Imperial March (Star Wars)
8. Super Mario Bros
9. Tetris (Korobeiniki)
10. Mission: Impossible
11. Ode to Joy (Beethoven)
12. SOS (Morse code)

Sirens and alarm signals (1–6, 12) are generated procedurally (frequency changed on the fly at runtime); the actual melodies (7–11) are stored as note data in flash memory (PROGMEM).

### Console output language

At the top of `src/main.cpp` there is a constant:

```cpp
const uint8_t LANGUAGE = LANG_CS; // <-- change to LANG_EN for English output
```

Changing the value to `LANG_EN` switches all Serial monitor messages to English.

### Wiring diagram

```
                     Arduino UNO R3
                  ┌───────────────────┐
   Buzzer (+) ────┤ D8                │
   Buzzer (-) ──┐ │                   │
                │ │                   │
  "Next" btn ───┼─┤ D2                │
  (other leg) ──┤ │                   │
                │ │                   │
  "Prev" btn ───┼─┤ D3                │
  (other leg) ──┤ │                   │
                │ │                   │
                └─┤ GND               │
                  └───────────────────┘
```

| Part | Pin/leg | Arduino UNO R3 |
|---|---|---|
| Passive buzzer | `+` | D8 |
| Passive buzzer | `-` | GND |
| "Next" button | leg 1 | D2 |
| "Next" button | leg 2 | GND |
| "Previous" button | leg 1 | D3 |
| "Previous" button | leg 2 | GND |

Both buttons use the internal pull-up resistors (`INPUT_PULLUP`), so **no external resistor is needed** – just wire one leg of each button to its digital pin and the other leg to GND. The buttons are wired to pins D2 and D3, which support hardware interrupts on the Arduino UNO (INT0/INT1), so they react instantly even during longer processing inside `loop()`.

### Technical notes (memory efficiency)

- Every console text string is wrapped in the `F()` macro, so it stays in flash memory instead of consuming the scarce SRAM (only 2 KB on the ATmega328P).
- Melody notes (frequency + duration) are stored as an array of structs in `PROGMEM` and read back with `pgm_read_word()`.
- The demo table is an array of function pointers stored in `PROGMEM` (`pgm_read_ptr()`), so selecting a demo costs no extra SRAM either.
- Sirens/alarms (firefighters, police, ambulance, fire alarm, air-raid, Red Alert, SOS) use no data arrays at all – they are generated purely by computing the frequency at runtime (`tone()` in a loop), so they use almost no memory.

### Project structure

```
platformio.ini        # PlatformIO configuration (board, framework)
src/
  main.cpp             # main program
include/
  pitches.h            # musical note frequency definitions
```

### Building and uploading with VS Code + PlatformIO

1. Install the **PlatformIO IDE** extension in VS Code (the repo ships a `.vscode/extensions.json`, so VS Code will offer to install it automatically).
2. Open the repository's root folder in VS Code – PlatformIO auto-detects `platformio.ini` (board `uno`, framework `arduino`).
3. Build: PlatformIO panel → *Build*, or `pio run` in the terminal.
4. Upload to the board: PlatformIO panel → *Upload*, or `pio run --target upload`.
5. Serial monitor (9600 baud): PlatformIO panel → *Monitor*, or `pio device monitor`.

The project can still be built with the classic Arduino IDE too – open `src/main.cpp` and copy `include/pitches.h` next to it (or add the `include/` path to the IDE's library/include search path).
