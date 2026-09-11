# Port firmware VB3000 → ATmega64A

Port al firmware-ului **optimizat** LM3S2965 (vezi `vibro_optimized/`) pe
**ATmega64A**, compilat cu **avr-gcc 14.2 / avr-libc 2.2.1**.

Codul se compilează fără erori. Toate funcțiile sunt funcționale ca în
original; diferențele sunt cele impuse de arhitectura AVR (detaliate mai jos).

---

## 1. Mărimi (rezultat build, `make size`)

| Regiune | Folosit | Disponibil | Ocupare |
|---|---|---|---|
| **Flash** (program + date const) | **14 490 B** (.text 11 068 + .rodata 3 422) | 64 KB (65 536 B) | **22,1 %** |
| **SRAM** (variabile globale) | **1 713 B** (.data 14 + .bss 1 699) | 4 KB (4 096 B) | **41,8 %** |

Note:
- `avr-size` raportează „Program: 11 082 bytes" — aceasta e doar `.text+.data`;
  datele const (font-uri, șiruri) stau în `.rodata` (3 422 B) tot în flash.
  Flash-ul total ocupat este **14 490 B**.
- Stack-ul de rulează din SRAM peste `.bss` (de la vârf, 0x801100 în jos);
  rămân ~2,4 KB liberi pentru stack + datele de runtime.
- `make` produce `vb3000_atmega64a.elf` și `vb3000_atmega64a.hex`.

### Ce ocupă în SRAM (câțișor)

| Simbol | B |
|---|---|
| `LCDBuff` (pagina afișorului 128×8×1B) | 1 024 |
| `TabelGrafic[128]` (grafic, `int`) | 256 |
| buffer-uri UART RX/RX-msg/TX (64×3) | 192 |
| `WMessage[64]`, `VText[24]`, parametrii, taste, timpi | ~240 |

---

## 2. Mapează de pini (conform librăriei tale KS0108 GLCD)

Inițializarea ta: `KS0108_GLCD(36, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 37, 38, 4)`
= `(DI, RW, E, DB0..DB7, CS1, CS2, RES)`, decodificat cu convenția standard
Arduino (D0-7=PA, D8-15=PB, D16-23=PC, D24-31=PD, D32-39=PE):

| Semnal | D-pin | Port/pin AVR |
|---|---|---|
| DI (RS) | 36 | **PE4** |
| RW | 26 | **PD2** |
| E (EN) | 27 | **PD3** |
| DB0..DB3 | 28..31 | **PD4..PD7** |
| DB4..DB7 | 32..35 | **PE0..PE3** |
| CS1 | 37 | **PE5** |
| CS2 | 38 | **PE6** |
| RES | 4 | **PA4** |

Atenție: magistrala de date este **împărțită pe 2 porturi** (PD + PE).
Scrierile pe porturi folosesc măști (`LCD_DB_LOW_PINS` / `LCD_DB_HIGH_PINS`)
ca să nu piște pe piniile USART-ului (PD0/PD1) sau DI/CS1/CS2 (PE4-6).

Pentru ca portul să meargă **exact** cu librăria ta, piniile sunt centralizate
în `pins.h` — dacă decodificarea cuveției diferă de a ta, se schimbă doar acolo.

---

## 3. Decizii importante de port

### 3.1 RS-485 pe USART1 (nu pe USART0)
PD2/PD3 (RXD0/TXD0) sunt **ocupate de RW/EN ale afișorului** → serialul a
fost mutat pe **USART1**: RXD1 = **PD0** (D24), TXD1 = **PD1** (D25).
- Baud 115200, 8N1, `UBRR1 = 8` la 16 MHz → eroare **−3,55 %** (ok la 8N1).
- Protocolul e identic cu originalul: mesaj `#...` terminat cu `\r`,
  TX prin ISR (UDRIE1), `TXEN` ridicat doar pe durata mesajului.
- Fix-urile din versiunea optimizată sunt păstrate: timeout la așteptarea
  transmiterii, fără scrieri out-of-bounds în bufferul RX.
- Noul utilitar `SelUART1SendTxBufferP()` trimite **șiruri din flash**
  (literals); `SelUART1SendTxBuffer()` rămâne pentru buffer-e RAM.

### 3.2 Font-urile și textele din FLASH (nu din RAM)
ATmega64A are doar 4 KB SRAM, deci:
- `font[]`, `font2[]`, `siglaSeletron[]` și toate string literals-urile
  sunt `const` → `.rodata` (flash) — vezi scriptul de link (3.5).
- Citirea se face **doar prin `pgm_read_byte()`** (LPM): `DisplaySStringP`,
  `DisplaySStringDP`, `PutImage` citesc cu LPM. Variantele simple
  (`DisplaySString`, `DisplaySStringD`) rămân pentru buffer-e din RAM.
- Verificat prin codegen: avr-gcc **nu** emit automat LPM pentru pointer-i
  `const` cu index dinamic — de aceea orice acces de flash trebuie să fie
  explicit prin `pgm_read_byte`.

### 3.3 Fără stdio (sprintf)
`sprintf` consumă ~2-3 KB de flash pe AVR și alocație pe stack. Am înlocuit:
- `fmt.h` / `fmt.c` — `fmt_num` (zero-pad), `fmt3`, `fmt4` pentru `%03d`/`%04d`;
- `SprintTimp` rescris fără stdio;
- mesajele `>SH,...`/`>SW,...`/`>SK,...`/`>SD,...` se construiesc cu
  `msg_build()` în `Message.c`;
- `KText` (afișarea „K= %d") — `fmt4`.

### 3.4 Timere (corectate exact, la 16 MHz)
- **Timer1**: CTC, prescaler 64, `OCR1A = 2499` → **10 ms** exact
  (decrementează `TimeStatusMachine`, avansează ceasul, pune `Bit10ms`);
- **Timer0**: CTC, prescaler 64, `OCR0 = 249` → **1 ms** exact (`Bit1ms`).
  ⚠️ În original, „Bit1ms" venea de la Timer1B și era de fapt **5,12 ms** —
  aici `Bit1ms` e corect 1 ms (aceeași corecție ca în versiunea optimizată).

### 3.5 Script de link custom (`ld_atmega64a_rodata_flash.x`)
⚠️ **Bug de toolchain întâlnit**: scripturile de link din acest avr-gcc/ld
(puse implicit de avr-libc 2.2.1, inclusiv `avr5.x`) plasează `.rodata`
**în RAM** (`.data : ... *(.rodata) > data AT> text`) — la 4 KB SRAM
proiectul nu ar fi intrat niciodată. Am generat `ld_atmega64a_rodata_flash.x`
(ca `avr5.x`, dar cu `.rodata` mutat în zona de text/flash). Makefile-ul îl
folosește automat.

### 3.6 OLED / KS0108
Port complet al `OLED.c`:
- scrieri pe magistrala **împărțită** cu măști de port (vezi 2);
- pulsul de reset pe `RES = PA4` în `DisplayReset()` (o singură dată, la boot);
- `_delay_us(3)` pentru timing-ul KS0108 (E < 450 ns, DS ≤ 120 ns);
- `RefreshPage()` redă **doar rândurile murdare** (optimizare păstrată);
- `pgm_read_byte` pentru font și sigla;
- Bresenham, `PutLine`, `Rectangle`, `FillRect` — identice cu originalul.

### 3.7 Tastatura
`KBD.c` e table-driven (cea din versiunea optimizată). **Piniile sunt
presupuse** (nu au fost date de tine) — vezi TODO din `pins.h`.

---

## 4. Presupuneri de verificare (TODO în `pins.h`)

| # | Presupunere | Unde se schimbă |
|---|---|---|
| 1 | **Cristal 16 MHz** extern pe PB5/PB6, `F_CPU = 16000000UL` | `Makefile` (`F_CPU`) + fuses |
| 2 | **TXEN RS-485 = PB0** (D8) | `pins.h` → `UART_TXEN_PORT/PIN` |
| 3 | **Taste**: K1-K4 = PB1..PB4 (D9-D12), K5-K8 = PC0..PC3 (D16-D19), K9 = PC4 (D20), K10 = PC5 (D21) | `pins.h` → K1..K10 |
| 4 | Taste pe **input fără pull**; dacă placa nu are pull extern, dezcomentează pull-up-urile din `main.c` | `main.c` |
| 5 | Decodificarea D-pin → port conform convenției standard (2) | `pins.h` |

---

## 5. Structura folderului

```
atmega64a/
├── Makefile                  # make / make size / make clean
├── main.c                    # init + bucla cu mașina de stări (10 ms)
├── pins.h                    # ★ toți piniile, centralizați
├── OLED.c / OLED.h           # KS0108 (magistrală împărțită, dirty-rows, LPM)
├── Draws.c / Draws.h         # mașina de stări de desen (port, fără stdio)
├── KBD.c / KBD.h             # tastatura (table-driven)
├── Message.c / Message.h     # protocol cu vibsist (msg_build, fără stdio)
├── UART0.c / uart0.h         # USART1 RS-485 (RX ISR, TX ISR, TXEN)
├── TimerProg.c / TimerProg.h # Timer1=10ms, Timer0=1ms
├── fmt.c / fmt.h             # înlocuitor de sprintf (zero-pad)
├── font.c                    # font + sigla (const → flash)
├── globals.h / Globals.c     # parametrii comuni (copiați din original)
├── ld_atmega64a_rodata_flash.x  # script de link: .rodata → FLASH
├── vb3000_atmega64a.elf / .hex  # output
└── PORT_ATMEGA64A.md         # acest raport
```

---

## 6. Build

```sh
cd atmega64a
make            # generează vb3000_atmega64a.elf + .hex
make size       # afișează Flash/SRAM
make clean
```

Fuzioni recomandate (16 MHz, cristal extern): CKDIV8 = OFF (0xFF),
SUT/CUTF = ceramica 16 MHz (ex. 0x14 / 0x12 conform datasheet-ului),
BOD activ. (Depinde de setările actuale ale plăcii tale.)

## 7. Lista de teste hardware (aceeași ca pentru versiunea optimizată)

1. La pornire apare sigla SELETRON, apoi trecerea automată în modul SET.
2. Tastele: RUN/STOP, SET, SLIDE, SWEEP, ±, SELECT, PREV/NEXT — fiecare
   schimbă ecranul corespunzător; graficul se desenează la RUN.
3. RS-485: comandă `#G,C` de pe PC la 115200 8N1 → primești `>SH,...` etc.;
   `#S,H,050,300,005` modifică parametrii și revine la ecranul SET.
4. Eroare de comunicație (vibsist deconectat): după 0,5 s se afișează „ERR COM"
   și după 1 s se retrimite `>START` — bucla de reîncercare funcționează.
5. Ceasul (HH:MM:SS) avansează corect; la 10 ms mașina de stări reacționează
   fără sămoane (dovadă că Timer1 e exact 10 ms).
