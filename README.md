# BenGin

**BenGin** to autorski silnik do gier 2.5D stworzony w celu nauki architektury silników gier oraz implementacji niskopoziomowych rozwiązań graficznych w ramach przedmiotu Moduł Sumatywny, realizowanego na VI. semestrze Informatyki Stosowanej na Politechnice Łódzkiej (Wydział FTIMS).
Projekt skupia się na modularności i kompaktowości.
Został stworzony na potrzeby gry [Laboratorun](https://krasnalshortworks.itch.io/laboratorun), przez okres 03-06.2025.

## O projekcie

Celem projektu BenGin jest stworzenie funkcjonalnego środowiska do tworzenia gier, które pozwala na głębsze zrozumienie tego, jak działają komercyjne silniki "pod maską". Silnik obsługuje m.in. renderowanie grafiki, system wejścia oraz podstawową fizykę.
Zaimplentowaliśmy osobno silnik gry oraz level editor, służący do tworzenia prefabów pokojów, które są instancjonowane w samej grze.

## Funkcjonalności

* **Oświetlenie - directional, point light, spotlight**
* **Dynamiczne renderowanie cieni**
* **Animacje szkieletowe i blending**
* **Podłączanie komponentów do obiektów**
* **Prosta fizyka**
* **Instancjonowanie GPU i graf sceny**
* **Serializacja prefabów**
* **Obsługa audio (FMod)**

## Technologie

Projekt został napisany przy użyciu:

* **Język:** C++ (Standard C++17/20)
* **API Graficzne:** OpenGL
* **Zarządzanie oknem:** GLFW
* **Inne biblioteki:** SPDLog, ImGui, STB Image, FMod

## Status projektu

Aktualnie projekt jest uznany za ukończony.

## Autorzy

* **Kamil Tasarz**
* **Bartłomiej Ejzert**
* **Marcel Piotrowicz**
* **Marek Zarychta**
---
*BenGin © 2025. Wszelkie prawa zastrzeżone.*
