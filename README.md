# Reversi
a simple game written in C with GTK+



1.  **Okno główne aplikacji.**
  
  ![alt tag](https://github.com/dingorth/Reversi/blob/master/reversi_obrazek.jpg)

2. **Funkcjonalności aplikacji:**

- Po kliknięciu przycisku "Nowa gra" przeciwnik dostaje zapytanie czy chce rozpocząć nową grę. Może zaakceptować albo odrzucić propozycję. W tym czasie gracz który wnioskował o nową grę czeka na odpowiedź przeciwnika. W przypadku odpowiedzi pozytywnej gra rozpoczyna się.
- Po kliknięciu przycisku "Cofnij" przeciwnik dostaje zapytanie czy pozwala swojemu przeciwnikowi na cofnięcie jednego ruchu. W tym czasie gracz który wnioskował o cofnięcie ruchu czeka na odpowiedź przeciwnika. W przypadku odpowiedzi pozytywnej ruch jest cofany. Można wnioskować tylko o cofnięcie jednego swojego ruchu.
- Gracz nie może nacisnąć żądnego pola dla którego jego ruch byłby niepoprawny.
- Pola oznaczone małym szarym kółkiem to pola na które gracz może nacisnąć i wykonać poprawny ruch.
- Jeśli poprawny ruch na gracza nie istnieje to automatycznie przeciwnik ma 2 ruch.
- Jeśli gracz wyjdzie z gry to jego przeciwnik otrzymują taką informację i może wyjść z gry.
- Wszystkie niedozwolone akcje są poblokowane.
- Gracze podczas rozgrywki mogą ze sobą konwersować.
- Po zakończonej rozgrywce obu graczom wyświetla się informacja który z nich wygrał. Mają możliwość albo od razu wnioskować o następną nową grę albo zrezygnować z tej przyjemności i wejść do okna z zakończoną grą i znowu ewentualnie zacząć nową grę lub wyjść.

3. **Moduły aplikacji:**

- Moduł główny - tworzy GUI i podpina zdarzenia do sygnałów.
- Moduł gry - zarządza całym przebiegiem gry i wszystkimi akcjami z nią związanymi, oraz czatem.
- Moduł  komunikacyjny - odpowiada za komunikację międzyprocesową.

4. **Uruchamianie / kompilacja: **
  Na linuxie kompilować z plikiem pipes-lin.c a na windowsie z plikiem pipes-win.c
  Wymagana jest biblioteka GTK
