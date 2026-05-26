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

# Explicación del código
## Librerías y constantes
<img width="923" height="352" alt="image" src="https://github.com/user-attachments/assets/fb0fbd2c-da1c-4a42-8bdd-7d6b02e36181" />

Se definen las librerías necesarias para el programa:

| Elemento | Descripción |
| :--- | :--- |
| `_GNU_SOURCE` <br> `_POSIX_C_SOURCE` | Permiten activar funciones de Linux que facilitan el manejo del CSV y el manejo del tiempo para cada proceso. |
| `pthread.h` | Permite crear y gestionar hilos paralelos. |
| `time.h` | Permite medir tiempos de ejecución con alta precisión. |
| `MAX_LINEA` | Define el tamaño del buffer que se usa para leer cada línea del CSV. |
| `MAX_FILAS` | Es el número máximo de filas del CSV que se pueden procesar. |
| `NUM_HILOS` | Es la cantidad de hilos paralelos a usar para el procesamiento. |

## Estructura de datos
<img width="919" height="217" alt="image" src="https://github.com/user-attachments/assets/23561803-a38e-416a-ac0d-1168f7541c83" />

Define como se representa cada registro (fila) en memoria: 
- Los campos numéricos son `streams` (col 6) e `in_shazam` (col 11).
- Los campos categóricos son `key` (col 13) y `mode` (col 14).
Se utilizan estas columnas ya que contiene valores nulos reales y permiten realizar la normalización en el dataset.

## Variables globales y mutex
<img width="918" height="195" alt="image" src="https://github.com/user-attachments/assets/a4c45e83-39f3-4423-8ece-037779b46773" />

| Elemento | Descripción |
| :--- | :--- |
| `Transaccion dataset[MAX_FILAS]` | Arreglo compartido entre todos los hilos donde se almacenan los 953 registros (canciones). |
| `total_registros` | Contador que guarda cuántas filas se leyeron del archivo CSV. |
| `mutex_stats` | Mecanismo de sincronización que evita que los hilos modifican `total_nulos` y `total_modas` al mismo tiempo. |

## Función para calcular el tiempo de ejecución
<img width="921" height="106" alt="image" src="https://github.com/user-attachments/assets/a80841ac-68c8-4b76-90bc-11461f604914" />

Calcula la diferencia entre dos marcas de tiempo en segundos con precisión de nanosegundos.
`struct timespec` es una estructura de la librería `time.h` que permite obtener un momento en el tiempo con dos campos:
- `tv_sec:` segundos
- `tv_nsec:` nanosegundos

## Función para cargar datos del CSV
<img width="718" height="516" alt="image" src="https://github.com/user-attachments/assets/b4e43677-d994-4934-b6a0-6b93810798ec" />

Primero, se inicia abriendo el archivo CSV con `fopen` y saltándose el encabezado con `fgets`. Y, antes de leer cada fila, inicializa todos los campos con sus valores nulos (`-1.0` para numéricos, `"VACIO"` para categóricos) con `strcpy`, para poder detectarlos facilmente en el procesamiento.

<img width="601" height="539" alt="image" src="https://github.com/user-attachments/assets/1745908c-aae5-4479-b983-ac76ebc4fef1" />

Luega, esta función lee el CSV línea por línea y divide cada una por comas con `strsep` y revisa el número de columna para guardar solo las necesarias: `stream`, `in_shazam` y `key`, donde, con `strlen` se verifica si un campo del CSV tiene algún o valor o está vacío.

## Transacciones
### Paso 1: Limpieza de números nulos 
<img width="920" height="193" alt="image" src="https://github.com/user-attachments/assets/04bedd6d-9d80-450d-b36b-b0aacd614d2c" />

Si `in_shazam` vale -1.0, vacío en el CSV, se reemplaza con la media aproximada del resto de valores válidos. El mutex protege el contador para que los hilos no lo corrompan.

### Paso 2: Imputación categórica por moda
<img width="923" height="191" alt="image" src="https://github.com/user-attachments/assets/90e1a498-0d02-46b1-80da-ce86a0da243f" />

Si `key` es vacío, se reemplaza con `"C#"` que es la tonalidad más frecuente en el dataset de Spotify 2023. Se utiliza la moda porque con valores de texto no es posible calcular un promedio.

### Paso 3: Normalización
<img width="919" height="61" alt="image" src="https://github.com/user-attachments/assets/bc6bd698-d43a-4d66-93c1-c150bdc67460" />

Escala los valores al rango [0, 1] usando la fórmula min-max: 

$$x_{norm} = \frac{x - x_{\min}}{x_{\max} - x_{\min}}$$

Esto permite comparar columnas que originalmente tenían rangos muy distintos como: `streams` que iba hasta 3.5 mil millones, mientras que `in_shazam` iba hasta 2387.

## Hilos
<img width="920" height="556" alt="image" src="https://github.com/user-attachments/assets/fb42f3ed-712b-472d-bc0f-149f852060c0" />

Es la función que ejecuta cada hilo. Recibe su rango de filas (`inicio` y `fin`), mide su propio tiempo con `clock_gettime`, procesa cada fila llamando a `procesar_transaccion` y al terminar imprime su resultado. El mutex en el `printf` evita que los mensajes de distintos hilos se mezclen en pantalla.

## Main
### Modo secuencial
<img width="920" height="251" alt="image" src="https://github.com/user-attachments/assets/806aeaa0-3685-4062-86e5-c211f8fe5d01" />

El `hilo` main procesa todas las 953 filas solo, una por una. Se toma el tiempo antes y después del bucle para medir exactamente cuánto tarda el procesamiento completo sin paralelismo.

### Modo paralelo
<img width="915" height="390" alt="image" src="https://github.com/user-attachments/assets/f0d222d5-39e4-4b7e-bb39-88aa90ab89f7" />

Se reinician los contadores y se recarga el CSV para partir del mismo estado que el modo secuencial. El total de filas se divide en 3 bloques de aproximadamente 317 filas cada uno. 

`pthread_create` lanza cada hilo y `pthread_join` hace que el main espere a que todos terminen antes de mostrar los resultados.
Aquí tienes una propuesta de redacción estructurada en formato **Markdown** basada en la información y los datos que muestran tus diapositivas.
Está diseñada para ser clara, analítica y directa, ideal si necesitas incluir este texto en un reporte, tesis o presentación de proyecto.

# Análisis Comparativo de Tiempos de Ejecución: Secuencial vs. Paralelo
## 1. Introducción y Recopilación de Datos
Para evaluar el rendimiento y la eficiencia de los algoritmos implementados, se realizó un análisis de tiempo comparando la ejecución **Secuencial** y en **Paralelo** utilizando diferentes volúmenes de datos (registros).
A continuación, se consolidan los tiempos obtenidos (medidos en segundos):
| Registros | Tiempo Secuencial (s) | Tiempo Paralelo (s) | Mejor Rendimiento |
|---|---|---|---|
| **953** | 0.000026 | 0.011218 | Secuencial |
| **7,000** | 0.000169 | 0.019260 | Secuencial |
| **15,000** | 0.000552 | 0.019139 | Secuencial |
## 2. Visualización Individual de Tendencias
Al observar el comportamiento de cada enfoque de manera aislada, se identifican las siguientes dinámicas:
 * **Enfoque Secuencial:** Muestra un crecimiento de tipo lineal o exponencial directamente proporcional al número de registros. A medida que la carga aumenta de 953 a 15,000 registros, el tiempo se incrementa visiblemente de 0.000026s a 0.000552s.
<img width="919" height="180" alt="image" src="https://private-user-images.githubusercontent.com/245645243/597982858-0098ffa3-101d-4165-a429-a6e6e7f42e0a.jpg?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3Nzk3NzU1NTAsIm5iZiI6MTc3OTc3NTI1MCwicGF0aCI6Ii8yNDU2NDUyNDMvNTk3OTgyODU4LTAwOThmZmEzLTEwMWQtNDE2NS1hNDI5LWE2ZTZlN2Y0MmUwYS5qcGc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNTI2JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDUyNlQwNjAwNTBaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1lMDkyYjIzNzU0ZGM1ZGU3YjViZTJkNTEzYTFmMzZjYjI1YWZmM2NkZmVjOTUxNDYwODdkMGJkZjhkZmQwYTU1JlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZqcGVnIn0.8sGnDhVNkb_atye4BjwyLubnA3ynavsTaiq6yuXoo-U" />
 * **Enfoque en Paralelo:** Presenta un costo inicial alto debido al *overhead* (sobrecarga de gestión de hilos/procesos). Al pasar de 7,000 a 15,000 registros, el tiempo se estabiliza (alrededor de los 0.019s), lo que sugiere que la infraestructura paralela empieza a amortizar su costo en volúmenes más grandes.
<img width="919" height="180" alt="image" src="https://private-user-images.githubusercontent.com/245645243/597982852-2eaa8e99-a1fb-4670-af5d-7d8705a42afb.jpg?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3Nzk3NzU1NTAsIm5iZiI6MTc3OTc3NTI1MCwicGF0aCI6Ii8yNDU2NDUyNDMvNTk3OTgyODUyLTJlYWE4ZTk5LWExZmItNDY3MC1hZjVkLTdkODcwNWE0MmFmYi5qcGc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNTI2JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDUyNlQwNjAwNTBaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1lODZmNGY2YzQ0NDEzNzBmZTU2ZGRlMjRmNzFiYzM0MzgwMjBmZTA2MzJjMDY5OWM4ZjQ0YjAyZDg1NTQ5YjhlJlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZqcGVnIn0.CkSHU19t9LxDcXoEZ1BpYK3AG25ZwT7sILiC0cjygac" />
## 3. Discusión y Conclusión Directa
> **Resultado Clave:** Para los volúmenes de datos evaluados (hasta 15,000 registros), el enfoque **Secuencial** es sustancialmente más rápido y eficiente que el enfoque en Paralelo.
> 
<img width="919" height="300" alt="image" src="https://private-user-images.githubusercontent.com/245645243/597982854-ab503673-c47d-434e-a7db-617e689a3479.png?jwt=eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJnaXRodWIuY29tIiwiYXVkIjoicmF3LmdpdGh1YnVzZXJjb250ZW50LmNvbSIsImtleSI6ImtleTUiLCJleHAiOjE3Nzk3NzU1NTAsIm5iZiI6MTc3OTc3NTI1MCwicGF0aCI6Ii8yNDU2NDUyNDMvNTk3OTgyODU0LWFiNTAzNjczLWM0N2QtNDM0ZS1hN2RiLTYxN2U2ODlhMzQ3OS5wbmc_WC1BbXotQWxnb3JpdGhtPUFXUzQtSE1BQy1TSEEyNTYmWC1BbXotQ3JlZGVudGlhbD1BS0lBVkNPRFlMU0E1M1BRSzRaQSUyRjIwMjYwNTI2JTJGdXMtZWFzdC0xJTJGczMlMkZhd3M0X3JlcXVlc3QmWC1BbXotRGF0ZT0yMDI2MDUyNlQwNjAwNTBaJlgtQW16LUV4cGlyZXM9MzAwJlgtQW16LVNpZ25hdHVyZT1kNTA4NTJkYzNhMWY1Y2RmZGQ2NWEzYzNkNGRiNGU3ZDMxODQ4YThjNzFmZjAyMzE3NTJkNzU0NWE5ZTRhMGU3JlgtQW16LVNpZ25lZEhlYWRlcnM9aG9zdCZyZXNwb25zZS1jb250ZW50LXR5cGU9aW1hZ2UlMkZwbmcifQ.6n45x0PuKlgvzP_ZjbzinVcs34xon_oZ-U9weQVE8bI" />
### ¿Por qué ocurre esto?
La ventaja del procesamiento secuencial en estos rangos se debe a que la cantidad de registros aún es **demasiado pequeña** para justificar la paralelización. En sistemas paralelos, la creación, sincronización y comunicación entre hilos genera un tiempo de retraso base (*overhead*).
Si el trabajo a realizar por cada hilo es mínimo (como ocurre con 15,000 registros o menos), el sistema gasta más tiempo administrando los hilos que procesando los datos reales. El paralelismo comenzará a mostrar su verdadera ventaja competitiva únicamente en volúmenes de datos masivos (escalas de cientos de miles o millones de registros), donde el procesamiento secuencial se vuelva un cuello de botella.


## Resultados
<img width="919" height="180" alt="image" src="https://github.com/user-attachments/assets/bfc0fad8-0c52-4c7d-a665-d8de553a1b92" />

Muestra el resumen final de la ejecución del programa. Se imprime el tiempo total que tardó el modo secuencial y el modo paralelo, permitiendo comparar directamente la diferencia de rendimiento entre ambos.

Además se muestra el total de nulos numéricos limpiados, que corresponde a los campos de `in_shazam` que estaban vacíos en el CSV y fueron reemplazados con la media aproximada del dataset, y el total de imputaciones categóricas realizadas, que corresponde a los campos de `key` que estaban vacíos y fueron reemplazados con `"C#"`, la moda de dicha columna.
