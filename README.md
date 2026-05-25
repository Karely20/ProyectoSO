# PROCESAMIENTO PARALELO DE TRANSACCIONES
**ESCUELA POLITÉCNICA NACIONAL | CIENCIA DE DATOS E INTELIGENCIA ARTIFICIAL | PROYECTO BIMESTRAL**

**Nombre:** Karely Bombón, Jhon Tiupul, Jhoan Sasnalema, Fidel Quilumba.

**Fecha:** 26-05-2026

# PROBLEMA
El objetivo del proyecto es demostrar la diferencia de rendimiento entre el procesamiento secuencial y el procesamiento paralelo usando hilos (`pthreads`) en C, aplicado a un dataset real de Spotify 2023.

## Dataset
Se utilizó el archivo `spotify_2023.csv`, que contienía 953 canciones con 24 columnas originalmente, sin embargo, fue preprocesado de la siguiente manera: 
- Se eliminaron `track_name` y `artist(s)_name` ya que conteneían texto complejo.
- Se corrigieron valores numéricos que contenían comas.

Tras un análisis exhaustivo del dataset a implementar, se identificaron los siguientes campos a analizar por el programa: 

| Tipo | Columnas |
| :--- | :--- |
| **Numéricas** | `artist_count`, `released_year`, `released_month`, `released_day`, `in_spotify_playlists`, `in_spotify_charts`, `streams`, `in_apple_playlists`, `in_apple_charts`, `in_deezer_playlists`, `in_deezer_charts`, `in_shazam_charts`, `bpm`, `danceability_%`, `valence_%`, `energy_%`, `acousticness_%`, `instrumentalness_%`, `liveness_%`, `speechiness_%` |
| **Categóricas** | `key`, `mode` |

Igualmente se identificaron los valores nulos dentro del dataset que serían útiles para llevar a cabo la limpieza de valores nulos numéricos y la imputación de valores categóricos por la moda:

| Columna | Nulos |
| :--- | :--- |
| `in_shazam_charts` | 50 |
| `key` | 95 |

## Objetivo del programa
Cada hilo procesa un bloque de filas y aplica 3 transformaciones:

1. **Limpieza de valores nulos numéricos:**
  Los campos numéricos vacíos se reemplazan con la media calculada sobre todos los valores válidos de esa columna.

       Ejemplo:
         in_shazam_charts = ""  →  in_shazam_charts = 567.32  (media global)

3. **Imputación de valores categóricos con la moda:**
  Los campos de texto vacíos se reemplazan con el valor más frecuente (moda) de esa columna.

       Ejemplo:
        key = ""  →  key = "C#"  (moda global de la columna key)

5. **Normalización (0 a 1):**
  Todos los valores numéricos se escalan al rango [0, 1] con la fórmula:

  $$valor_{norm} = \frac{x - min}{max - min}$$
  
      Ejemplo:
        bpm = 125,  min_bpm = 65,  max_bpm = 206
        norm_bpm = (125 - 65) / (206 - 65) = 0.4255

# PROCEDIMIENTO DE EJECUCIÓN
1. Se creó un repositorio en GitHub donde se creó el archivo `README.md` con la documentación del proyecto y el dataset `spotify_2023.csv` que contiene los datos que serán procesados por el programa.
2. Desde la terminal de Ubuntu se clonó el repositorio con el comando:

       git clone https://github.com/Karely20/ProyectoSO.git
       cd ProyectoSO
   Este proceso descargó automaticamente el dataset y el README al entorno de trabajo.

  4. Dentro de la carpeta del repositorio se creó el archivo `proyecto.c` utilizando el editor de texto `nano`:

         nano proyecto.c
     Se escribió todo el código fuente directamente en la maquina virtual, implementando la lectura del CSV, el procesamiento secuencial, el procesamiento paralelo   con 3 hilos y la medición de tiempos.
4. Una vex terminado el código se compiló con `gcc` indicando la librería de los hilos:

        gcc proyecto.c -o proyecto -lpthread

   Esto generó el ejecutable `proyecto` listo para correr.

5. Se ejecutó el programa con:

        ./proyecto
    
   El programa leyó el archivo `spotify_2023.csv`, procesó las 953 canciones en modo secuencial y luego en modo paralelo con 3 hilos, mostrando los tiempos y resultados en pantalla.

6. Una vez verificado el funcionamiento correcto del programa, se subió el archivo `proyecto.c` al repositorio con los siguientes comandos:

        git add proyecto.c
        git commit -m "Proyecto SO"
        git push origin main

## Explicación del código
### Librerías y constantes
<img width="919" height="369" alt="Captura de pantalla 2026-05-25 171130" src="https://github.com/user-attachments/assets/a038b2b0-dbcf-4058-8e61-a49f116864f0" />

Se definen las librerías necesarias para el programa:
- `pthread.h:` permite crear y gestionar hilos.
- `time.h:` permite medir tiempos con precisión.


