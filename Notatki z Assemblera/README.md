Markdown jest dostosowany pod [Obsidian](https://obsidian.md/), więc w podglądzie Githuba część rzeczy może się psuć

---

Notatki by Maciej Kaszkowiak, v2023-04-02

## Organizacyjne
Mikrokontroler ADuC 842, assembler ponoć identyczny do MCS-51 (Intel 8051?). Workflow: w DOSie wywołujemy ASM51 w celu skompilowania .asm do .lst. Pudełko resetujemy trzymając lewy niebieski guzik i naciskając prawy niebieski guzik. W programie F3 do wgrania binarki oraz F5 do uruchomienia binarki. Można też lecieć step by step. W view > SFRs można podejrzeć rejestry.

### Wyświetlanie cyfry 
Wyświetlanie cyfry realizowane jest przez zapisanie jej w rejestrze P2. 

Liczba w ASM zapisana jest z prefiksem # aby oznaczyć ją jako dane. Sufiks "b" oznacza, że liczba jest zapisana binarnie. Analogicznie można użyć "h" do zapisu heksadecymalnego oraz pominąć sufiks w celu zapisu dziesiętnego.

Rejestr jest ośmiobitowy, zaś liczba jest czterobitowa. W celu nazewnictwa przyjmuję, że liczymy bity od zera, od prawej strony (jest to spójne z adresowaniem bitów w ASM).

- **bit 7.:** włącza odczytywanie przycisku znajdującego się pod cyfrą - jeśli ustawimy go na 1, to wartość zmieni się na 0 w momencie styku ww. przycisku oraz z powrotem na 1 po puszczeniu przycisku.
- **bit 6.:** wyświetla kropkę 
- **bity 5., 4.**: pozycja wyświetlanej cyfry - 00b to cyfra najbardziej po prawej, 11b to cyfra najbardziej po lewej.
```arm-asm
$MOD 842
MOV P2, #00000101b
END					
```

### Wyświetlanie dwóch cyfr jednocześnie
Wyświetlanie dwóch cyfr jednocześnie jest realizowane przez zapalanie w pętli na przemian pierwszego oraz drugiego segmentu.

Pomiędzy jednym a drugim wprowadzony jest delay 200 instrukcji odejmowania - bez opóźnienia 7-segmentowy wyświetlacz średnio to wyświetla.

Tutaj tworzymy podprogram za pomocą etykiety (z) oraz instrukcji CMP. Analogicznie wykorzystujemy skok bezwarunkowy do etykiety x.
```arm-asm
$MOD842  
x:  
MOV P2, #00000110B  
CALL z  
MOV P2, #00100101B  
CALL z  
JMP x  
  
z:  
MOV R7, #200  
DJNZ R7, $  
RET  
  
END
```


### Licznik w dół po naciśnięciu przycisku
Opis w komentarzach w kodzie.

```armasm
$MOD842  
START:  
MOV R7, #9  		; wydzielam rejestr do przechowywania liczby
  
BLINK:  
MOV P2, R7  		; wyświetlam
CALL GUZIK  		; czekam za guzikiem
DJNZ R7, BLINK  	; don't jump until not zero - idzie dalej jeśli R7 == 0
  
MOV P2, R7			; wyświetlam 0
CALL GUZIK  
JMP START
  
GUZIK:  			; podprogram GUZIK czeka za naciśnięciem guzika 
CALL DELAY  		; najpierw czekam trochę ms
					; aby wykrywało jedynie co któreś kliknięcie
SETB P2.7			; ustawiam 7. bit na 1 (odpowiedzialny za zapis)
					; naciśnięcie guzika ustawi 7. bit na 0
JB P2.7, $			; zapętlam się zatem dopóki 7. bit nie będzie równy 0
RET  				; wychodzę z podprogramu

; poniższy kod jest skopiowany z wbudowanego blink.asm
; delaye można zmieniać poprzez zmianę wartości w DLY0 / DLY1

DELAY:
MOV R1,A ; Acc holds delay variable (1 clock)  
DLY0: MOV R2,#0007 ; Set up delay loop0 (2 clocks)  
DLY1: MOV R3,#0AFh ; Set up delay loop1 (2 clocks)  
DJNZ R3,$ ; Dec R3 & Jump here until R3 is 0 (3 clocks)  
DJNZ R2,DLY1 ; Dec R2 & Jump DLY1 until R2 is 0 (3 clocks)  
DJNZ R1,DLY0 ; Dec R1 & Jump DLY0 until R1 is 0 (3 clocks)  
RET ; Return from subroutine  
  
END
```

### Licznik w górę 00-59 
Licznik przechowuje pierwszą i drugą cyfrę w odpowiednio R6 oraz R7. 
Rejestr R6 zawiera prefix 0001 zamiast 0000, przez co liczba wyświetlana jest na drugim wyświetlaczu od prawej.

Istota tego kodu opiera się na tym, że nie możemy wykorzystać wcześniejszego podprogramu DELAY. Podprogram obsługujący czekanie musi jedncześnie wyświetlać nasze cyfry w rejestrze P2 - inaczej zostałaby jedna wyświetlana cyfra.

```arm-asm
$MOD842  
START:  
MOV R6, #00010000b  			; startowa wartość = 0	

STARTLOOP:  
MOV R7, #0  					; startowa wartość = 0
  
BLINK:  
CALL DELAY  
INC R7  
CJNE R7, #9, BLINK  
  
CALL DELAY  
INC R6  
  
CJNE R6, #00010110b, STARTLOOP 	; idzie dalej gdy R6 == 6  
								; w przeciwnym wypadku skacze do STARTLOOP

jmp START						; dobiliśmy do 60, reset wszystkiego  
  

DELAY:							; odczytajcie to jako zagnieżdżoną pętlę  
MOV A, #255  
DELAYLOOP:  
MOV P2, R6  
MOV R1, #255  
DJNZ R1, $  
MOV P2, R7  
MOV R1, #255  
DJNZ R1, $  
DEC A  
JNZ DELAYLOOP  
RET  
  
END
```





### Wyświetlanie innej cyfry przez okres przytrzymania przycisku
Komentarze w kodzie

```arm-asm
$MOD842  
BLINK:  				; główna pętla
MOV P2, #9  			; wyświetlamy 9 
SETB P2.7				; włączam odczyt guzika  
JB P2.7, $				; czekam aż guzik nie zostanie wciśnięty  
CALL GUZIK  			; wywołujemy guzik
JMP BLINK 				; zapętlamy się
  
GUZIK:					; wyświetlamy 1 na okres przytrzymania przycisku  
MOV R6, #1  			; rejestr R6 zliczy nam ile okresów przytrzymaliśmy
MOV A, #1  				; rejestr A steruje długością czekania w podprogramie DELAY
 
GUZIKLOOP: 				; pętla zliczająca okres przytrzymania  
JB P2.7, GUZIKOUT		; jeśli puszczony guzik to wychodzimy z pętli
INC R6  				; zwiększamy R6

; ----------------------------------------------------------------------
; Poniższe 3 instrukcje to obsługa przepełnienia. Nie przetestowałem tego na płytce, ale powinno zadziałać.
; O co chodzi? Jeśli przytrzymamy przycisk na dłużej niż 2^8 okresów czasu, to rejestr A przeskoczy z 255 do 0 i czas już przestanie się zgadzać
; Zrobiłem taki myk, że jak to wykryje to przestanie czekać, wyświetli cyfrę 0 na wyświetlaczu i wyjdzie z pętli
; Jeśli ma bugi, to możecie je wyrzucić razem z podprogramem PRZEPELNIENIE
MOV A, R6
JZ PRZEPELNIENIE ; opcjonalnie  
MOV A, #1 ; opcjonalnie  
; ----------------------------------------------------------------------

CALL DELAY				; czekamy 1 okres czasu (A = 1)
JMP GUZIKLOOP			; zliczamy dalej okres

GUZIKOUT:  
MOV A, R6				; ustawiamy okres czekania na R6   
MOV P2, #2  			; wyświetlamy 2
CALL DELAY  			; czekamy ile trzeba
RET
  
PRZEPELNIENIE:  
MOV P2, #0  
MOV A, #50  
CALL DELAY  
RET  
  
DELAY: ; Delays by ms 10* A  
MOV R1,A ; Acc holds delay variable (1 clock)  
DLY0: MOV R2,#0FFh ; Set up delay loop0 (2 clocks)  
DLY1: MOV R3,#0FFh ; Set up delay loop1 (2 clocks)  
DJNZ R3,$ ; Dec R3 & Jump here until R3 is 0 (3 clocks)  
DJNZ R2,DLY1 ; Dec R2 & Jump DLY1 until R2 is 0 (3 clocks)  
DJNZ R1,DLY0 ; Dec R1 & Jump DLY0 until R1 is 0 (3 clocks)  
RET ; Return from subroutine  
  
END
```

# Przerwania

### Togglowanie kropki na podstawie przerwania

Przerwanie domyślnie wywoływane jest w momencie wystąpienia stanu niskiego.
Na naszej płytce EXT0 podczepione jest pod przycisk, czyli przerwanie wykona się gdy przycisk jest naciśnięty.
Oznacza to, że jeśli **trzymamy** przycisk, to procesor w kółko będzie wchodził do przerwania.
Z tego powodu gdy dla poniższego kodu trzymamy przycisk, można zauważyć że kropka na wyświetlaczu siedmiosegmentowym zmniejsza jasność.
To przez to, że na przemian jest wyłączana i włączana (PWM 50%).


```arm-asm
$MOD842
JMP START
ORG 03H		; adres przerwania external 0 "interrupt vector addresses"
CPL P2.6	; complement = negacja bitu kropki

RETI		; powrót z przerwania

START:
; rejestr IE jest odpowiedzialny za przerwania
; pierwszy bit - wlaczenie wszystkich przerwan
; na ostatnim bicie jest wlaczenie przerwania ext0
; 10000001 = 81h
MOV IE, #81h
;SETB TCON.0 	; ustawienie na togglowanie falling edge
SETB P2.6
JMP $
END
```

### Problem drgań styków przycisku

Aby kod sensownie reagował na pojedyncze przerwanie, należy dodać delay w trakcie przerwania. 

Dla przykładu, przyjmijmy że wciskając przycisk trzymamy go przez 100ms. Jeśli stworzymy dodatkowe opóźnienie 500ms wewnątrz przerwania, to po wyjściu z obsługi przerwań nie będziemy już trzymali przycisku. Co za tym idzie - kolejne przerwanie nie będzie wywołane, ale za to przycisk będziemy mogli naciskać wyłącznie raz na 500ms. Co więcej, gdy będziemy trzymali przycisk, to przerwania będą wywoływane w kółko, jakbyśmy wielokrotnie naciskali przycisk.

Trzeba dobrze dobrać wartości opóźnienia, aby wciskanie guzika było jakkolwiek działające. Moje wartości opóźnienia zostały wybrane eksperymentalnie (7, FFh, AFh).

Alternatywne potencjalne rozwiązanie - reagowanie na falling edge zamiast reagowania na stan niski - da się za to ustawić pomocą rejestru TCON. Przetestowałem to na szybko, ale nie sprawdza się u mnie w pełni - ze względu na ograniczony czas na labach nie kombinowałem dalej i zostawiłem aktywowanie stanem niskim. Dla kontekstu - wyglądało to u mnie tak, że każde naciśnięcie oraz każde puszczenie przycisku wywoływało dwa interrupty, jeden po drugim. 

```arm-asm
$MOD842
JMP START
ORG 03H		; adres przerwania external 0 "interrupt vector addresses"
CPL P2.6	; complement = negacja bitu kropki
MOV A, #7
CALL DELAY
RETI		; powrót z przerwania

START:
; rejestr IE jest odpowiedzialny za przerwania
; pierwszy bit - wlaczenie wszystkich przerwan
; na ostatnim bicie jest wlaczenie przerwania ext0
; 10000001 = 81h
MOV IE, #81h
; SETB TCON.0 	; ustawienie na togglowanie falling edge
SETB P2.6
JMP $


DELAY: ; Delays by ms 10* A
MOV R1,A ; Acc holds delay variable (1 clock)
DLY0: MOV R2,#0FFh ; Set up delay loop0 (2 clocks)
DLY1: MOV R3,#0AFh ; Set up delay loop1 (2 clocks)
DJNZ R3,$ ; Dec R3 & Jump here until R3 is 0 (3 clocks)
DJNZ R2,DLY1 ; Dec R2 & Jump DLY1 until R2 is 0 (3 clocks)
DJNZ R1,DLY0 ; Dec R1 & Jump DLY0 until R1 is 0 (3 clocks)
RET ; Return from subroutine

END
```

### Zliczanie trzech przerwań 
Kod liczy 2 -> 1 -> 0 oraz wyświetla kropkę przy zerze. Naciśnięcie przycisku wywołuje zmianę.

Komentarz prowadzącego - "w P3.2 widać czy jest wywołane te przerwanie" - nie sprawdzałem, zapisuję.

```arm-asm
$MOD842
JMP START
ORG 03H			; poczatek przerwania		
DJNZ R7, NIEZERO	; zlicza od 3 do 0

ZERO:		; sidenote - ta etykieta jest dla czytelności, mogłoby jej nie być
MOV P2, R7
CPL P2.6	
MOV A, #7
CALL DELAY
MOV R7, #3
RETI

NIEZERO:
MOV P2, R7
CALL DELAY
RETI		; powrót z przerwania

START:
MOV IE, #81h
MOV R7, #3
SETB P2.6
JMP $


DELAY: ; Delays by ms 10* A
MOV R1,A ; Acc holds delay variable (1 clock)
DLY0: MOV R2,#0FFh ; Set up delay loop0 (2 clocks)
DLY1: MOV R3,#0AFh ; Set up delay loop1 (2 clocks)
DJNZ R3,$ ; Dec R3 & Jump here until R3 is 0 (3 clocks)
DJNZ R2,DLY1 ; Dec R2 & Jump DLY1 until R2 is 0 (3 clocks)
DJNZ R1,DLY0 ; Dec R1 & Jump DLY0 until R1 is 0 (3 clocks)
RET ; Return from subroutine

END
```

## Timery

Timer na pokładzie naszej płytki co takt zegara inkrementuje nam rejestr 16-bitowy, stworzony z dwóch rejestrów TL/TH. Po przepełnieniu rejestru następuje zwykłe przerwanie, które wykorzystywaliśmy powyżej.

Na płytce mamy dwa timery - Timer0 oraz Timer1, z rejestrami TL0, TH0, TL1, TH1. Obsługa przerwań znajduje się odpowiednio pod adresami 0Bh oraz 1Bh.

Timer może również pełnić funkcję licznika, ale to jest nieistotne.

Timer ma swój tryb. Tryb timera określa między innymi jak dużą liczbę odlicza timer (TL / TH wcale nie musi być 16-bitowe). Przykładowo w trybie pierwszym rejestr jest 16 bitowy, a w trybie zerowym rejestr jest 13 bitowy.

### Jak obliczyć okres?
Domyślnie mamy taktowanie zegara 2.097 MHz. 
W tym procesorze jeden cykl zegarowy odpowiada jednemu cyklowi maszynowemu. 

Ustawmy najpierw tryb timera na pierwszy, aby wykorzystać wszystkie 16 bitów.

Wyznaczmy ile przerwań potrzebujemy w trybie pierwszym aby uzyskać jedną sekundę.

1 / (2.09700 megahertz) = 476.871722 nanoseconds
(1 s) / (476.871722 nanoseconds) = 2 097 000

2097000 nie mieści się w 16 bitach, zatem będziemy potrzebowali dodatkowy rejestr zliczający liczbę przepełnień timera.

2097000 / 2^16 = 31.9976806641

W przybliżeniu: musimy 32 razy przepełnić timer 16 bitowy, aby uzyskać okres \~1s.

### Zliczanie liczby od 0 do 9
Ustawiam tutaj też piąty bit P2, aby zmienić pozycję cyfry zgodnie z wymaganiami prowadzącego.

```arm-asm
$MOD842
JMP START
ORG 0BH		; adres przerwania Timer0		
DJNZ R7, NOTZERO

ZERO:
MOV R7, #32
CALL ZWIEKSZLICZBE
RETI

NOTZERO:
RETI

; ------ koniec obslugi przerwan ------

; ------ procedura ZWIEKSZLICZBE -------
ZWIEKSZLICZBE:
INC R1

CJNE R1, #10, LICZBAWZAKRESIE
POZAZAKRESEM:
MOV R1, #0
MOV P2, R1
SETB P2.5
RET

LICZBAWZAKRESIE:
MOV P2, R1
SETB P2.5
RET

; ------ główny kod --------------
START:
MOV TMOD, #00000001B		; ostatnie dwa bity 01 = włączenie trybu 1
MOV TCON, #00010000B		; bit 4 - włączenie timera 
MOV IE, #10000010B	; włączenie przerwań i przerwań timer0

MOV R7, #32		; zliczamy 32 razy przepelnienie - odlicza
MOV R1, #0		; odpowiada za trzymanie liczby do wyswietlenia
MOV P2, #00100000b
JMP $

END
```


### Wyświetlanie dwóch cyfr jednocześnie - pierwsza stale na 0, druga liczy od 0 do 9

Istotna zmiana - zapisałem pozycję cyfr już w R1 i R2 zamiast wykorzystywać SETB - paradoksalnie uprościło to znacznie kod.

Wrzuciłem to jako program przejściowy pomiędzy poprzednim a następnym, gdyby ktoś się pogubił przy analizie kodu. 

```arm-asm
$MOD842
JMP START
ORG 0BH		; adres przerwania Timer0		
DJNZ R7, NOTZERO

ZERO:
MOV R7, #32
CALL ZWIEKSZLICZBE
RETI

NOTZERO:
RETI

ZWIEKSZLICZBE:
INC R1

CJNE R1, #00101010B, LICZBAWZAKRESIE	; 10 binarnie
POZAZAKRESEM:
MOV R1, #00100000b
RET

LICZBAWZAKRESIE:
RET

START:
MOV TMOD, #00000001B		
MOV TCON, #00010000B		
MOV IE, #10000010B	

MOV R7, #32		
MOV R1, #00100000b		; odpowiada za trzymanie prawej cyfry
MOV R2, #00110000b 		; odpowiada za trzymanie lewej cyfry
MOV P2, #00000000b

; wcześniej mieliśmy tutaj JMP $
; teraz jest taki problem, że musimy wyświetlać obie cyfry na raz
; wywołujemy zatem procedurę do wyświetlania cyfr na pętli

PETLA:
CALL WYSWIETL
JMP PETLA


WYSWIETL:
MOV P2, R1
MOV R6, #20
DJNZ R6, $

MOV P2, R2
MOV R6, #20
DJNZ R6, $
RET

END
```

### Timer od 00-> 29 - mniej czytelna wersja
Tego może nie analizujcie. Kod działa, ale jest brzydki, przeskoczcie do wersji poniżej. Wrzucam aby było.
```arm-asm
$MOD842
JMP START
ORG 0BH		; adres przerwania Timer0		
DJNZ R7, NOTZERO

ZERO:
MOV R7, #32
CALL ZWIEKSZLICZBE
RETI

NOTZERO:
RETI

ZWIEKSZLICZBE:
INC R1

CJNE R1, #00101010B, CYFRAWZAKRESIE2	; 10 binarnie
POZAZAKRESEM2:
MOV R1, #00100000b
INC R2
CJNE R2, #00110011b, CYFRAWZAKRESIE1	; 3 binarnie

POZAZAKRESEM1:
MOV R2, #00110000b
RET

CYFRAWZAKRESIE1:
RET

CYFRAWZAKRESIE2:
RET

START:
MOV TMOD, #00000001B		
MOV TCON, #00010000B		 
MOV IE, #10000010B	

MOV R7, #32		
MOV R1, #00100000b		
MOV R2, #00110000b 		
MOV P2, #00000000b

PETLA:
CALL WYSWIETL
JMP PETLA

WYSWIETL:
MOV P2, R1
MOV R6, #20
DJNZ R6, $

MOV P2, R2
MOV R6, #20
DJNZ R6, $

RET

END
```

### Timer 00->29 z wykorzystaniem podprogramów
Powyżej procedury zwiększania liczb są zagnieżdżone w sobie.

Z wykorzystaniem podprogramów czytelniej i łatwiej rozszerzyć do 4 cyfr jednocześnie.
Zmieniłem też numerację rejestrów aby było jeszcze czytelniej - R1 najbardziej po lewej, R2 drugi od lewej, etc.

```arm-asm

$MOD842
JMP START
ORG 0BH		; adres przerwania Timer0		
DJNZ R7, NOTZERO

ZERO:
MOV R7, #32
CALL ZWIEKSZCYFRE2
RETI

NOTZERO:
RETI

ZWIEKSZCYFRE1:
INC R1
CJNE R1, #00110011b, CYFRAWZAKRESIE1	; 3 binarnie
MOV R1, #00110000b
RET

CYFRAWZAKRESIE1:
RET

ZWIEKSZCYFRE2:
INC R2

CJNE R2, #00101010B, CYFRAWZAKRESIE2	; 10 binarnie
MOV R2, #00100000b
CALL ZWIEKSZCYFRE1
RET

CYFRAWZAKRESIE2:
RET

START:
MOV TMOD, #00000001B	
MOV TCON, #00010000B		 
MOV IE, #10000010B	

MOV R7, #32	
MOV R2, #00100000b		; odpowiada za trzymanie prawej cyfry
MOV R1, #00110000b 		; odpowiada za trzymanie lewej cyfry

PETLA:
CALL WYSWIETL
JMP PETLA

WYSWIETL:
MOV P2, R2
MOV R6, #20
DJNZ R6, $

MOV P2, R1
MOV R6, #20
DJNZ R6, $

RET

END
```

### Dwa oddzielne liczniki, w oparciu o oddzielne przerwania Timer0 i Timer1

Oba liczniki (XX.YY) liczą 00->29, po prostu z wykorzystaniem dwóch oddzielnych timerów.

Liczniki są delikatnie przesunięte w fazie.
Zależy to zapewne od:
- początkowych wartości rejestrów TL0/TL1/TH0/TH1
- względnych priorytetów przerwań T0 i T1

Próbowałem wyzerować ww. rejestry, ale nie wpłynęło to na przesunięcie liczników w fazie.
Wg. prowadzącego takie zachowanie jest OK.

Tutaj powiem kilka słów odnośnie pisania ASM - warto pamiętać na etapie pisania o tym, który rejestr odpowiada za co.
W normalnym języku programowania sobie elegancko nazwiemy zmienne, tutaj nie ma takiej możliwości.
Dla przykładu wypiszę:
- R0: ---
- R1: pierwsza cyfra
- R2: druga cyfra
- R3: trzecia cyfra
- R4: czwarta cyfra 
- R5: zliczanie 32->0 w Timer1 aby uzyskać okres 1s 
- R6: odliczanie przerwy w procedurze WYSWIETL
- R7: zliczanie 32->0 w Timer0 aby uzyskać okres 1s 

```arm-asm
$MOD842
JMP START
ORG 0BH		; adres przerwania Timer0		
DJNZ R7, NOTZERO0

ZERO0:
MOV R7, #32
CALL ZWIEKSZCYFRE2
RETI

NOTZERO0:
RETI


ORG 1BH		; adres przerwania Timer1	
DJNZ R5, NOTZERO1

ZERO1:
MOV R5, #32
CALL ZWIEKSZCYFRE4
RETI

NOTZERO1:
RETI


ZWIEKSZCYFRE1:
INC R1
CJNE R1, #00110011b, CYFRAWZAKRESIE1	; 3 binarnie
MOV R1, #00110000b
RET

CYFRAWZAKRESIE1:
RET

ZWIEKSZCYFRE2:
INC R2

CJNE R2, #00101010B, CYFRAWZAKRESIE2	; 10 binarnie
MOV R2, #00100000b
CALL ZWIEKSZCYFRE1
RET

CYFRAWZAKRESIE2:
RET

ZWIEKSZCYFRE3:
INC R3
CJNE R3, #00010011b, CYFRAWZAKRESIE3	; 3 binarnie
MOV R3, #00010000b
RET

CYFRAWZAKRESIE3:
RET

ZWIEKSZCYFRE4:
INC R4

CJNE R4, #00001010B, CYFRAWZAKRESIE4	; 10 binarnie
MOV R4, #00000000b
CALL ZWIEKSZCYFRE3
RET

CYFRAWZAKRESIE4:
RET



START:

MOV TMOD, #00010001B	; włączam tryb 1 na T0 i T1	
MOV TCON, #01010000B	; włączam T0 oraz T1	
MOV IE, #10001010B	; włączam przerwania T0 oraz T1

MOV R7, #32

MOV R1, #00110000b 		
MOV R2, #00100000b		
MOV R3, #00010000b
MOV R4, #00000000b
 
PETLA:
CALL WYSWIETL
JMP PETLA


WYSWIETL:
MOV P2, R1
MOV R6, #10
DJNZ R6, $

MOV P2, R2
MOV R6, #10
DJNZ R6, $

MOV P2, R3
MOV R6, #10
DJNZ R6, $

MOV P2, R4
MOV R6, #10
DJNZ R6, $

RET

END
```

### Zegar MM:SS 
Tu do tego wystarczy jeden timer. Licznik jak na PTC (\[\*\]). 
Przyspieszyłem timer 8-krotnie za pomocą modyfikacji rejestru R7 - nie ma okresu 1s, tylko 1/8s.

```arm-asm

$MOD842
JMP START
ORG 0BH		; adres przerwania Timer0		
DJNZ R7, NOTZERO0

ZERO0:
MOV R7, #2	; zmniejszyłem delay z 32 do 2 w celu przyspieszenia układu
CALL ZWIEKSZCYFRE4
RETI

NOTZERO0:
RETI


ZWIEKSZCYFRE1:
INC R1
CJNE R1, #00110110b, CYFRAWZAKRESIE1	; 6 binarnie
MOV R1, #00110000b
RET

CYFRAWZAKRESIE1:
RET

ZWIEKSZCYFRE2:
INC R2

CJNE R2, #01101010B, CYFRAWZAKRESIE2	; 10 binarnie
MOV R2, #01100000b
CALL ZWIEKSZCYFRE1
RET

CYFRAWZAKRESIE2:
RET

ZWIEKSZCYFRE3:
INC R3
CJNE R3, #00010110b, CYFRAWZAKRESIE3	; 6 binarnie
MOV R3, #00010000b
CALL ZWIEKSZCYFRE2
RET

CYFRAWZAKRESIE3:
RET

ZWIEKSZCYFRE4:
INC R4

CJNE R4, #00001010B, CYFRAWZAKRESIE4	; 10 binarnie
MOV R4, #00000000b
CALL ZWIEKSZCYFRE3
RET

CYFRAWZAKRESIE4:
RET



START:

MOV TMOD, #00000001B	; włączamy Timer 0
MOV TCON, #00010000B		 
MOV IE, #10000010B

MOV R7, #2		; zmniejszyłem delay z 32 do 2 w celu przyspieszenia układu

MOV R1, #00110000b 		
MOV R2, #01100000b	; aktywny bit kropki		
MOV R3, #00010000b
MOV R4, #00000000b

PETLA:
CALL WYSWIETL
JMP PETLA


WYSWIETL:
MOV P2, R1
MOV R6, #10
DJNZ R6, $

MOV P2, R2
MOV R6, #10
DJNZ R6, $

MOV P2, R3
MOV R6, #10
DJNZ R6, $

MOV P2, R4
MOV R6, #10
DJNZ R6, $

RET


END
```

### Zegar MM:SS z pauzowaniem przy naciśnięciu guzika
Hybryda poprzednich kodów. Toggluję bit rejestru IE odpowiedzialnego za przerwania zegara zamiast togglować kropkę.

Gdy naciśniemy przycisk to aktywujemy DELAY oraz będzie wyświetlana tylko jedna cyfra - wg. prowadzącego ta drobnostka jest OK. Można to naprawić za pomocą modyfikacji procedury DELAY, ale nie chciało mi się na labach.

Rejestry się skończyły, więc zmodyfikowałem procedurę DELAY aby wykorzystywała wolne rejestry - R0, R5, R6, przy czym R6 jest już wykorzystywany przez procedurę WYSWIETL. Wykorzystuję go dwa razy, bo jak się nad tym zastanowicie to zastosowania nie kolidują.

Ponadto, wspomnę że kolejność obsługi przerwań wydaje się być istotna. Gdy zamieniłem miejscami procedury obsługi przerwań dla ext0 i timer0, to kod wyrzucał błędy przy kompilacji. Prawdopodobnie instrukcja na adresie 3 musiała być już zapisana. 

```arm-asm 
$MOD842
JMP START


ORG 03H		; adres przerwania external 0 (guzik)
CPL IE.1	; toggluje obsluge przerwan zegara	
MOV A, #7
CALL DELAY
RETI	


ORG 0BH		; adres przerwania Timer0		
DJNZ R7, NOTZERO0

ZERO0:
MOV R7, #2	; zmniejszyłem delay z 32 do 2 w celu przyspieszenia układu
CALL ZWIEKSZCYFRE4
RETI

NOTZERO0:
RETI


ZWIEKSZCYFRE1:
INC R1
CJNE R1, #00110110b, CYFRAWZAKRESIE1	; 6 binarnie
MOV R1, #00110000b
RET

CYFRAWZAKRESIE1:
RET

ZWIEKSZCYFRE2:
INC R2

CJNE R2, #01101010B, CYFRAWZAKRESIE2	; 10 binarnie
MOV R2, #01100000b
CALL ZWIEKSZCYFRE1
RET

CYFRAWZAKRESIE2:
RET

ZWIEKSZCYFRE3:
INC R3
CJNE R3, #00010110b, CYFRAWZAKRESIE3	; 6 binarnie
MOV R3, #00010000b
CALL ZWIEKSZCYFRE2
RET

CYFRAWZAKRESIE3:
RET

ZWIEKSZCYFRE4:
INC R4

CJNE R4, #00001010B, CYFRAWZAKRESIE4	; 10 binarnie
MOV R4, #00000000b
CALL ZWIEKSZCYFRE3
RET

CYFRAWZAKRESIE4:
RET



START:

MOV TMOD, #00000001B	; włączamy Timer 0 oraz ext0 (guzik)
MOV TCON, #00010000B		 
MOV IE, #10000011B

MOV R7, #2		; zmniejszyłem delay z 32 do 2 w celu przyspieszenia układu

MOV R1, #00110000b 		
MOV R2, #01100000b	; aktywny bit kropki		
MOV R3, #00010000b
MOV R4, #00000000b

PETLA:
CALL WYSWIETL
JMP PETLA


WYSWIETL:
MOV P2, R1
MOV R6, #10
DJNZ R6, $

MOV P2, R2
MOV R6, #10
DJNZ R6, $

MOV P2, R3
MOV R6, #10
DJNZ R6, $

MOV P2, R4
MOV R6, #10
DJNZ R6, $

RET


DELAY: ; Delays by ms 10* A
MOV R0,A ; Acc holds delay variable (1 clock)
DLY0: MOV R5,#0FFh ; Set up delay loop0 (2 clocks)
DLY1: MOV R6,#0AFh ; Set up delay loop1 (2 clocks)
DJNZ R6,$ ; Dec R3 & Jump here until R3 is 0 (3 clocks)
DJNZ R5,DLY1 ; Dec R2 & Jump DLY1 until R2 is 0 (3 clocks)
DJNZ R0,DLY0 ; Dec R1 & Jump DLY0 until R1 is 0 (3 clocks)
RET ; Return from subroutine

END
```


Na następne zajęcia: Timery, zegar TIC oraz ADC 
Update: ADC nie robiliśmy, Watchdoga też nie.

## Lab3

Nie ma assemblera:
1. Wrzuciłem notatki na ekursy
2. pliczek się wrzucił, usunąłem wszystko z pc za pomocą shift+delete
3. odpalam laptopa, z uśmiechem na twarzy chciałem wysłać kompletne notatki z ASK
4. pliczka nie ma. 
Nie nacisnąłem Save Changes. F dla notatek \[\*\]

Wszystko jest ładnie opisane w datasheet pod hasłem TIMECON.
![[Pasted image 20230402105953.png]]
![[Pasted image 20230402105944.png]]


### Zegar :SS

Setup - aktywujemy przerwania:
- INTVAL równe 1 (aby wywoływało każde przerwanie - przy wartości 10 np. co 10), 
- `00010011b` w TIMECON (aktywujemy przerwania co sekundy, włączamy time interval / time clock), 
- `#0ffH` w rejestrze IEIP2 (chyba przerwania aktywujemy dodatkowo? lub zmieniamy priorytet? nie rozczytałem się, ale podobno wymagane), 
- #80 w rejestrze IE (włączam globalnie przerwania). 

Można jeszcze wyzerować SEC, jeśli chcecie. Ja nie korzystałem z SEC, wyłącznie z mechanizmu przerwań co sekundę.

Nie bawiłem się w wyświetlanie SEC na LCD zgodnie z sugestią prowadzącego - R1/R0 zadysponowałem na SS, a w oddzielnej procedurze wyświetlałem odpowiednio R1/R0 z odstępami.
Czyścimy rejestry. R0/R1 bazowo nie miały wgrane 0000 0000b, tylko np. 0001 0000b i analogicznie było sprawdzone przepełnienie cyfr.
Przerwanie timera wywoływało procedurę zwiększającą po kolei rejestry.

Rejestr R4 zadysponowałem do czekania pomiędzy wyświetlaniem poszczególnych liczb, możecie jaki chcecie, też np. A.

Fajny, czysty kod. Zginął przez UI ekursów.

### Zegar MM:SS
R3/R2 zadysponowałem na MM. Analogiczny kod co we wszystkich powyższych przykładach. Wystarczy rozszerzyć o dodatkowe rejestry.

### Zegar MM:SS z wyświetlaniem godziny przez jakiś czas po przepełnieniu

Rejestry R5/R6 zadysponowałem do przechowywania HH:.
Procedura zwiększająca po kolei rejestry nie uległa zmianie w strukturze - dorzuciłem analogicznie zwiększanie R5 po przepełnieniu R3, zwiększałem R6 po przepełnieniu R4.

Kluczowa różnica: stworzyłem dodatkową procedurę - dotychczasową wyświetlającą standardowo MM:SS (z R0-R3) nazwałem WYSWIETLCZAS, nową wyświetlającą :HH (z R5-R6) nazwałem WYSWIETLGODZINE.

Wrzuciłem na początku programu w rejestr R7 wartość 1. Wartość 1 symbolizowała wyświetlanie czasu, zaś wartość 0 symbolizowała wyświetlanie godziny.

W momencie przepełnienia R3 i zwiększenia R5 (przepełnienie MM:SS, zwiększenie godziny), ustawiałem wartość R7 na 0 - aby pokazywać czas.

Poprzednia pętla:
1. W kółko wywoływała WYSWIETLCZAS

Nowa pętla:
1. Sprawdzała czy R7 == 1, jeśli tak to WYSWIETLCZAS
2. Jeśli R7 == 0, sprawdzała czy R1 == 2, jeśli tak to ustawiało R7 = 1, jeśli nie to WYSWIETLDATA

O co chodzi z tym checkiem? Sprawdzałem czy MM:SS były równe xx:2x, przez co wyświetlanie godziny trwałoby w przybliżeniu 20 sekund - w momencie dobicia do 00:20, kod zmieniłby wartość rejestru R7 z powrotem na 1 i wyświetlałby czas. 


### Ww. zegar, pauzujący przy wciśniętym guziku
Wystarczy jedna dodatkowa linijka kodu - bodajże  `P3.2` jest równy 0 gdy czerwony guzik jest wciśnięty na płytce. Na początku obsługi przerwania inkrementującego nasze liczniki, sprawdzamy czy P3.2 jest równe 0 - jeśli tak, to wywołujemy `RETI` i wychodzimy z przerwania.

### WW. zegar, z guzikiem działającym jako start/stop
Rejestry mi się skończyły - musiałem posłużyć się jakimś nieużywanym bitem i trochę poprzesuwać, mniejsza. (chyba P3.5)

Tutaj zmiana - nie mogłem wykorzystać P3.2, musiałem posłużyć się przerwaniami. Aktywowałem przerwanie INT0 (81h na IE), dodałem obsługę przerwania.
Aktywowałem również aktywowanie przerwania w tryb falling edge (ctrl+f po notatkach na "falling edge"), przez co przerwanie w teorii powinno wywołać się dokładnie raz wraz z naciśnięciem guzika.

Przerwanie togglowało bit P3.5, który z kolei zastąpił bit P3.2 w obsłudze przerwań zegara. 

### Skrzyżowanie

Na początku zróbcie pętle, wejdźcie w debugger i sprawdźcie w view > SFRs które bity aktywują co. Protip - guzik aktywuje stan niski, więc aby go wykryć dajcie wszędzie stan wysoki.

Dwie podprocedury:
- ZIELONEAUTA
- ZIELONEPIESI

Pętla czeka w kółko aż bit guzika się nie wyzeruje, następnie wywołuje ZIELONEPIESI, DELAY, ZIELONEAUTA.

Podprocedury odpowiednio korzystają z SETB i CLR na poszczególnych bitach, no i z Delay aby to dało się zauważyć okiem człowieka.

Na początku ustawcie bity na jadące auta.

Efekt - naciśniecie guzik, przejście dla pieszych się włączy.


