# Optimizare firmware VB3000 (vibro_telecomanda)

Proiect: telecomanda de vibratii, MCU **LM3S2965** (Cortex-M3, 50 MHz, **fara FPU**), Keil uVision.
Data: 2026-09-10. Codul optimizat e in aceasta folder; `diff_optimizari.txt` contine toate diferentele fata de original.

Toate fișierele trec verificarea de compilare (gcc -Wall, 0 erori) cu header-ele TI originale.
Nu am atins: `driverlib/`, `inc/` (biblioteca TI), `VS_LCD.uvproj` (aceleasi fisiere, aceleasi nume — se compileaza direct in Keil).

---

## 1. OPTIMIZARI DE PERFORMANTA

### 1.1 Afisajul OLED — cea mai mare castig (≈90% din CPU-ul consumat de afisaj)

**Problema din original:**
- `RefreshPage()` era apelata la **fiecare 10 ms** (dintr-o bucla fixa), si trimitea **toata pagina (1024 byte)** pe interfata paralela bit-banged, chiar daca ecranul nu se schimbase.
- La fiecare refresh se mai facea si **`DisplayReset()`** (secventa completa de initiere, 6 comenzi) — de fiecare data, degeaba.
- Fiecare byte costa: 3x `SysCtlDelay(50)` (≈9 µs) + 8 scrieri de registru → un refresh complet dura **≈12 ms**, adica **≈100% din CPU** doar pentru afisaj.
- Pe ecranele din regim normal, singurul lucru care se schimba la 10 ms era **ceasul de pe randul 7**.

**Ce am facut (OLED.c, Draws.c, OLED.h):**
- **Urmarire randuri modificate** (`LCD_DirtyMask`, 8 biti): orice primitiva de desene (`ClearPage`, `PutImage`, `PutPixel`, `DisplaySString`, `DisplaySStringD`, `FillRect`) marcheaza randul pe care scrie.
- `RefreshPage()` trimite **doar randurile modificate** (inloc de 1024 byte → de obicei 64–128 byte). Adresele X/Y se seteaza explicit inainte de fiecare rand, deci saltul de randuri e corect.
- `DisplayReset()` s-a mutat **doar la pornire** (se cheama o singura data din `main()`), nu la fiecare refresh.
- `RefreshPage()` din `DrawScreen()` e apelata **doar daca `LCD_IsDirty()`** — daca nimic nu s-a schimbat, nu se trimite nimic (0 byte).
- RW/RS se seteaza o singura data pe rand in loc de 2 scrieri/byte (starea pinilor la marginea ENA ramane identica cu originalul).

**Castig estimat:**
- Regim static (ecran de setare, fara tastare): CPU de afisaj **≈0%** (in loc de ≈100%).
- Regim cu ceas vizibil (toti ecranii de setare): ≈1.5 ms/10 ms **≈15% CPU** (in loc de 100%).
- Ecranul de RUN cu grafic (toate randurile se schimba la fiecare mesaj de la vibsist): ≈90% din timpul original — singura situatie in care se trimit multa pagina.

### 1.2 Float → intregi (fără FPU pe LM3S2965)

- Graficul din `MSM_RUN` si `MSM_STOP_SWEEP`: 124 inmultiri/trunchieri **float soft** pe frame → aritmetica pe intregi `v1 = 55-(v2*47)/350` (rezultat identic).
- `PutLine()`: algoritmul cu float (si cu o logica proasta de rotunjire, care lasa goluri pe unele pante) → **Bresenham clasic pe intregi** — mai rapid si linii mai continue.

### 1.3 KBD.c — 10x copy-paste → table-driven

- 10 bucati de cod identice (≈250 linii) → un singur ciclu pe o tabela `Keys[10]`.
- **Bonus — bug fixat:** blocul `KeySlide` era **duplicat** (probabil copy-paste gresit), ceea ce facea timpii tastei Slide de 2x mai rapidi decat ai celorlalte 9 taste. Acum comportamentul e uniform (verifica in hardware ca Slide se comporta ca in intentie).

### 1.4 UART (UART_GEN.c)

- `SelUART1SendTxBuffer()` avea `while(UART1FLAG_TxEmpty == 0);` — **busy-wait fara timeout**: daca linia RS-485 ramanea blocata, intregul sistem se oprea definitiv (fara watchdog activ). Acum: asteptare maxima ≈50 ms + repornirea starii de TX.
- RX: la ultimul index al bufferului (255) se scria **1 byte din afara** bufferului (`UART1RxBuffer[256]`). Corectat cu verificare de limite.

### 1.5 Flash/RAM

- Sters cod mort: `MSM_SET4` (≈150 linii comentate), functiile vechi comentate din OLED.c (`DisplaySendData/DisplaySendCommand/RefreshPageOLD`), `ReadKeys()` (nesemnificativa, citiri duplicate).
- KBD.c: ≈250 linii → ≈60 linii.
- `int KeyNone` a fost definit corect in KBD.c (vezi bug 2.1).

---

## 2. BUG-URI GASITE

### 2.1 `extern int KeyNone=-1;` (KBD.c) — **rezolvat**
`extern` cu initializer e invalid in C (rezultat nedefinit; lucreaza intamplator pe ARMCC/GCC). Acum e o definitie normala in KBD.c.

### 2.2 `NotInArea()` (Message.c) — **rezolvat**
```c
// original:
if(v<vmin) return -1;
if(v<vmax) return 1;   // <-- inversat: returna "eroare" pentru valori VALIDI
return 0;              //         si 0 pentru valori PESTE maxim
```
`CheckValues()`/`NotInArea()` **nu sunt apelate nicai** in proiect (cod mort cu bug) — am corectat logica ca sa fie corecta daca vor fi folosite: `-1` sub minim, `1` peste maxim, `0` in interval.

### 2.3 `invert_lcd()` (OLED.c) — **rezolvat**
```c
for (i = 0; i < 8; i++)
  for (j = 0; i < 128; i++)   // <-- incrementa i (bucla externa), nu j
```
Inversa doar 8 pixeli (coloana 0), nu toata pagina. Corectat in `j < 128` / `j++`. (Functia e nefolosită in proiect.)

### 2.4 Mesajul `#W` (RUN) — **rezolvat**
`DecodeMsg()` copia 20 byte in `WMessage` fara sa garanteze terminatorul `'\0'`. Un mesaj cu >20 caractere ne-nule lasa `WMessage` fara terminator, iar `DisplaySString(&WMessage[4],...)` citea memorie veche → text garbage pe ecran. Acum: copiere limitata (max 23) + terminator garantat.

### 2.5 Limite de acces in afisaj (OLED.c) — **rezolvat**
`DisplaySString`/`DisplaySStringD` verificau `if(var1 > 128)` — dar indexul 128 e **in afara** zonei valide (0..127): scriere out-of-bounds la rand plin. Corectat in `>= 128` (+ guard si pe ultima scriere `inv`). In codul actual niciun apel nu ajunge la limita, dar era o bomba incapsulata.

### 2.6 `Timer 1B` nu e 1 ms (sel_timer.h / TimerProg.c) — **notat, NEschimbat**
Cu prescaler 255 si load 1000, perioada reala e `50MHz/(256×1000) = 5.12 ms`, nu 1 ms (comentariul era gresit). `Bit1ms` nu e folosit ca flag de 1 ms in cod, deci nu am schimbat timpii hardware. **Pentru 1 ms: prescaler 255, load 195** — daca o sa ai nevoie de un flag de 1 ms real.

### 2.7 `SV3` in modul Slide (Draws.c) — **suspect, de verificat in hardware**
```c
if(SV2) VText[5]='1',FillRect(59,3,11); else FillRect(59,4,11);
if(SV3) VText[6]='1',FillRect(59,4,11);            // <-- fara else, si aceeasi locatie ca SV2
if(SV4) VText[7]='1',FillRect(107,3,11); else FillRect(107,4,11);
```
Parca e copy-paste gresit: SV3 are `FillRect` in aceeasi pozitie ca SV2 (si fara `else`). Am lasat un `TODO` in cod — spune-mi pozitia corecta a cutiei SV3 si o corectez.

### 2.8 Watchdog — **neactivat (așa cum era)**
`watch_dog_start()` e definita in main.c dar nu e apelata (e comentata). L-am lasat asa; activeaza-o (`watch_dog_start()` dupa init) daca vrei protectie impotriva blocarii — cu noile masuri de timeout UART, riscul de blocaj ramas e mic.

---

## 3. OBSERVATII (neintervenit)

1. **`UART1.c` si `UART1J.c` NU sunt in build** (`.uvproj` foloseste `UART_GEN.c`). Ele definesc aceleasi variabile globale si aceleasi functii ca `UART_GEN.c` — daca cineva le adauga in proiect, apar erori de link (simboluri duplicate) si, mai rau, `SelUART1SendTxBuffer` are semnaturi diferite (cele doua variante: `unsigned long len` vs `unsigned char *text`). Recomand sa le muti intr-o folder `legacy/` sau sa le stergi.
2. **`Startup.s` lipseste din repo** — e referent in `.uvproj` (`.\Startup.s`) dar nu e commitat. Proiectul nu se poate builda din repo fara el. Adauga-l in git (e fisierul de startup Keil cu tabela de interrupte).
3. `sel_uart.h` declara `SelUART1IntHandler` si functii inline `SelUARTPutChar/GetChar` — `SelUART1IntHandler` nu e definita nicai (doar `UART1FIFOIntHandler` e in tabela de interrupte). Declaratii moarte, dar inofensive.
4. Citirile de taste citesc **intregul registru de port**, nu doar bitul tastei (`Key1 = HWREG(PORTE + DATA)` contine 4 biti). Functioneaza atata timp cat alti pinii din port sunt 0. Am pastrat exact comportamentul; pentru robustete ai putea masca: `Key1 = (HWREG(...) & KBD_K1)`.
5. `sel_timer.h` are si `TIMER_0_LOAD_INTERVAL` si prototip-uri `SelTimer0Init` nefolosite.

---

## 4. CE AM SCHIMBAT FISIER CU FISIER

| Fisier | Schimbari |
|---|---|
| `OLED.c` | `LCD_DirtyMask` + marcheaza randuri; `RefreshPage()` doar randuri modificate, fara `DisplayReset()`; `out_lcd_data()`; `PutLine()` Bresenham intregi; fix `invert_lcd`; fix limite `>=128`; sters cod mort comentat |
| `OLED.h` | prototip-uri noi (`LCD_MarkRow/MarkRows/MarkAll/IsDirty`); include guard corect; sterse prototip-uri moarte |
| `Draws.c` | `if(LCD_IsDirty()) RefreshPage()`; grafice fara float; sters `MSM_SET4` comentat; TODO pe SV3 |
| `KBD.c` | table-driven (10 taste → 1 ciclu); fix duplicare KeySlide; fix definitie `KeyNone`; sters `ReadKeys()` |
| `KBD.h` | stersa declararea `ReadKeys` |
| `Message.c` | fix `NotInArea`; mesaj `#W` cu terminator garantat; variabila `c` nefolosită stersa |
| `UART_GEN.c` | timeout pe busy-wait TX + reset stare; fix out-of-bounds RX |
| `TimerProg.c` | fix `TBOTE\|TBOTE` → `TAOTE\|TBOTE` (cosmetic) |
| `sel_timer.h` | comentariu corect despre perioada reala a Timer 1B (5.12 ms) |
| `main.c`, `Globals.c`, `sel_uart.h`, rest | **neschimbat** |

---

## 5. TESTARE IN HARDWARE (recomandat)

1. **Build in Keil** — ar trebui sa compileze fara erori (aceleasi fisiere, aceleasi nume).
2. **Pornire** — sigla Seletron sa apara normal (testul de init afisaj).
3. **Navigare** — toate ecranele de setare (SET0..SET3), Prev/Next, +/- (cu auto-repeat), Select.
4. **Mod Slide** — verifica comportamentul tastei Slide (timpi) si pozitia cutiei SV3 (vezi 2.7).
5. **Mod Sweep + RUN** — graficul de pe ecranul RUN sa se actualizeze ca inainte; masoara ca mesajele `#W` de la vibsist se afiseaza corect.
6. **Comunicatie** — START/params de la vibsist, `#G` (Get), `#F` (Force), `#S` (Set).
7. **Timeout UART** — deconecteaza linia RS-485 pe o transmitere: inainte sistemul se bloca definitiv; acum trece mai departe dupa ≈50 ms.
8. **Consum CPU** — cu un pin liber + osiloscop, poti verifica ca in regim statik afisajul nu mai face nicio activitate pe linia de date (inainte: activitate continua).
