# BenGin

**BenGin** to autorski silnik do gier 2.D stworzony w celu nauki architektury silników gier oraz implementacji niskopoziomowych rozwiązań graficznych. Projekt skupia się na wydajności i modularności.
Został stworzony na potrzeby gry [Laboratorun](https://krasnalshortworks.itch.io/laboratorun), przez okres 03-06.2025.

## O projekcie

Celem projektu BenGin jest stworzenie funkcjonalnego środowiska do tworzenia gier, które pozwala na głębsze zrozumienie tego, jak działają komercyjne silniki "pod maską". Silnik obsługuje renderowanie grafiki, system wejścia oraz podstawową fizykę.
Zaimplentowaliśmy osobno silnik gry oraz level editor, służący do tworzenia prefabów pokojów, które są instancjonowane w samej grze.

## Funkcjonalności

* **Obsługa point light'ów**
* **Animacje szkieletowe i blending**
* **Prosta fizyka**
* **Instancjonowanie GPU i graf sceny**
* **Serializacja prefabów**

## Technologie

Projekt został napisany przy użyciu:

* **Język:** C++ (Standard C++17/20)
* **API Graficzne:** OpenGL
* **Zarządzanie oknem:** GLFW
* **Inne biblioteki:** SPDLog, ImGui, STB Image

## Status projektu

Aktualnie projekt jest uznany za ukończony.

## Autorzy

**Kamil Tasarz**
**Bartłomiej Ejzert**
**Marcel Piotrowicz**
**Marek Zarychta**
---
*BenGin © 2025. Wszelkie prawa zastrzeżone.*
