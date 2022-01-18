# Języki Assemblerowe
## Steganografia w pliku BMP

Projekt na zajęcia z przedmiotu Języki Assemblerowe. 

### Sposób budowania aplikacji:

Sposób zautomatyzowany (wspierane dystrybucje: Ubuntu, Arch):

 - `$ git clone https://github.com/nixenos/ja_stegano.git`
 - `$ cd ja_stegano/projekt`
 - `$ ./install_and_prepare_build_env.sh`
 - `$ ./buildAll.sh`

Sposób manualny:
 - sklonować repozytorium: `$ git clone https://github.com/nixenos/ja_stegano.git`
 - zainstalować potrzebne pakiety:
   - Qt5 lub Qt6
   - nasm
   - make
   - cmake
   - gcc
 - przejść do katalogu `ja_stegano/projekt`
 - wygenerować konfigurację Cmake: `cmake .`
 - skompilować bibliotekę w ASM: `cd asm_src; make`
 - skompilować bibliotekę w C: `cd c_src; make`
 - skompilować projekt: `make`

### Uruchamianie aplikacji:

```
./projekt
```
