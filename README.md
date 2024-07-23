## MPI (Message Passing Interface)
### Pokretanje programa
Nakon bildovanja(build solution) programa, otvaranje cmd u Debug folderu i unos komande mpiexec -n 5 pok1.exe, gde 5 predstavlja broj procesa a pok1 ime aplikacije.

### Opis aplikacija
1. **Množenje matrice sa vektorom:**
   - U ovom delu se koristi 16 procesora za množenje matrice sa vektorom.
   - Matrica `a` i vektor `b` su inicijalizovani i podeljeni na podmatrice i podvektore, koji se šalju različitim procesima.
   - Svaki proces vrši lokalno množenje i vraća rezultat glavnom procesu (MASTER).
   Za ovo mnozenje korišćeno je 16 procesora
![Result](Mnozenje%20matrice%20sa%20vektorom/rezultat.png)

2. ### Mnozenje matrice sa matricom
    - Korišćeno je 4 procesa jer su matrice manjih dimenzija zbog lakše provere. Naravno program radi i za više procesa.
    - U suštini slično je odradjen zadatak kao i gore, korišćenjem grupnih komunikatora.

![Result](Mnozenje%20matrice%20sa%20matricom/rezultat1.png)

3. ### Množenje matrice sa vektorom s tim što procesi dobijaju 2^rank vrsta
    - Tekst zadatka i rešenje prikazani u folderu
