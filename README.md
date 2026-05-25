# PROCESAMIENTO PARALELO DE TRANSACCIONES
**ESCUELA POLITÉCNICA NACIONAL | CIENCIA DE DATOS E INTELIGENCIA ARTIFICIAL | PROYECTO BIMESTRAL**

**Nombre:** Karely Bombón, Jhon Tiupul, Jhoan Sasnalema, Fidel Quilumba.

**Fecha:** 26-05-2026

# PROBLEMA
El objetivo del proyecto es demostrar la diferencia de rendimiento entre el procesamiento secuencial y el procesamiento paralelo usando hilos (`pthreads`) en C, aplicado a un dataset real de Spotify 2023.

## Dataset
Se utilizó el archivo `spotify_2023.csv`, que contienía 953 canciones con 22 columnas originalmente, sin embargo, fue preprocesado de la siguiente manera: 
- Se eliminaron `track_name` y `artist(s)_name` ya que conteneían texto complejo.
- Se corrigieron valores numéricos que contenían comas.

Tras un análisis exhaustivo del dataset a implementar, se identificaron los siguientes campos a analizar por el programa: 

| Tipo | Columnas |
| :--- | :--- |
| **Numéricas** | `artist_count`, `released_year`, `released_month`, `released_day`, `in_spotify_playlists`, `in_spotify_charts`, `streams`, `in_apple_playlists`, `in_apple_charts`, `in_deezer_playlists`, `in_deezer_charts`, `in_shazam_charts`, `bpm`, `danceability_%`, `valence_%`, `energy_%`, `acousticness_%`, `instrumentalness_%`, `liveness_%`, `speechiness_%` |
| **Categóricas** | `key`, `mode` |

Igualmente se identificaron los valores nulos dentro del dataset que serían útiles para la limpieza de valores nulos numéricos y la imputación de valores categóricos por la moda:

| Columna | Nulos |
| ;--- | ;--- |
| `in_shazam_charts` | 50 |
| `key` | 95 |
