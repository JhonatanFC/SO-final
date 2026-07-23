# Informe Técnico del Proyecto SIMGESRC

## Sistema de Gestión de Recursos Computacionales para la Plataforma Bancaria Digital de Caja Huancayo

---

## Información General

| Campo             | Descripción                                                                                         |
| ----------------- | --------------------------------------------------------------------------------------------------- |
| Proyecto          | SIMGESRC                                                                                            |
| Nombre completo   | Sistema de Gestión de Recursos Computacionales para la Plataforma Bancaria Digital de Caja Huancayo |
| Curso             | Sistemas Operativos                                                                                 |
| Universidad       | Universidad Peruana Los Andes                                                                       |
| Sistema Operativo | Ubuntu Desktop 24.04 LTS                                                                            |
| Lenguaje          | C11                                                                                                 |
| Compilador        | GCC                                                                                                 |
| Biblioteca        | POSIX Threads (pthread)                                                                             |

---

# Descripción

SIMGESRC es un simulador académico desarrollado para representar el funcionamiento de los principales mecanismos de administración de recursos de un sistema operativo moderno utilizando como escenario una plataforma bancaria digital inspirada en Caja Huancayo.

El proyecto permite observar cómo el sistema operativo administra procesos concurrentes, memoria, recursos compartidos y planificación del procesador mediante la implementación de algoritmos clásicos estudiados en el curso de Sistemas Operativos.

El simulador no representa una plataforma bancaria real, sino un entorno de experimentación diseñado con fines académicos para analizar el comportamiento de diferentes estrategias de administración de recursos computacionales.

---

# Objetivo General

Desarrollar un simulador académico que permita representar la administración de procesos, memoria y concurrencia dentro de una plataforma bancaria digital mediante la implementación de algoritmos clásicos de sistemas operativos.

---

# Objetivos Específicos

- Simular procesos representativos de una plataforma bancaria.
- Implementar algoritmos clásicos de planificación de CPU.
- Simular administración dinámica de memoria.
- Implementar concurrencia mediante POSIX Threads.
- Utilizar mecanismos de sincronización mediante mutex.
- Calcular métricas de rendimiento.
- Exportar automáticamente los resultados obtenidos.

---

# Tecnologías Utilizadas

## Sistema Operativo

- Ubuntu Desktop 24.04 LTS

## Lenguaje

- C11

## Compilador

- GCC

## Bibliotecas

- pthread
- stdio
- stdlib
- string
- time
- unistd

## Herramientas

- Visual Studio Code
- Make
- Git
- GitHub
- Terminal Linux

---

# Arquitectura General

La arquitectura del proyecto fue diseñada siguiendo un enfoque modular.

Los módulos principales son:

- Programa principal
- Gestor de procesos
- Planificador de CPU
- Administrador de memoria
- Módulo de sincronización
- Gestor de métricas
- Exportador de resultados

La descripción completa de la arquitectura puede consultarse en:

```text
docs/Arquitectura.md
```

---

# Escenario Simulado

El simulador representa una plataforma bancaria digital donde múltiples usuarios ejecutan operaciones simultáneamente.

Entre ellas:

- Inicio de sesión
- Consulta de saldo
- Transferencias
- Pagos
- Actualización de información
- Liquidaciones nocturnas

Cada proceso posee diferentes características de ejecución permitiendo comparar el comportamiento de los algoritmos implementados.

---

# Algoritmos de Planificación

## FCFS (First Come First Served)

Ejecuta los procesos respetando el orden de llegada.

Características:

- No apropiativo
- Fácil implementación
- Baja sobrecarga
- Puede producir efecto convoy

---

## SJF (Shortest Job First)

Selecciona primero el proceso con menor ráfaga de CPU.

Ventajas:

- Reduce el tiempo promedio de espera.

Desventajas:

- Puede generar inanición.

---

## Round Robin

Distribuye el procesador utilizando un quantum fijo.

Configuración utilizada:

```text
Quantum = 4
```

Ventajas:

- Tiempo de respuesta reducido.
- Equidad entre procesos.
- Adecuado para sistemas interactivos.

---

## Prioridades con Aging

Los procesos se ejecutan considerando su prioridad.

La técnica Aging incrementa gradualmente la prioridad de los procesos que permanecen mucho tiempo esperando, evitando la inanición.

---

# Administración de Memoria

El simulador implementa un administrador de memoria utilizando particiones variables.

Los algoritmos desarrollados son:

## First-Fit

Asigna el primer bloque libre disponible.

Ventajas:

- Rápido.
- Simple.

---

## Best-Fit

Selecciona el bloque libre más pequeño que pueda contener el proceso.

Ventajas:

- Mejor aprovechamiento del espacio.

Desventajas:

- Mayor tiempo de búsqueda.

---

# Concurrencia

El simulador incorpora programación concurrente mediante POSIX Threads.

Cada proceso puede ejecutarse utilizando hilos administrados por Linux.

Para proteger recursos compartidos se implementa:

- Mutex
- Exclusión mutua
- Secciones críticas

Esto evita condiciones de carrera y mantiene la consistencia de los datos.

---

# Recursos Administrados

Durante la simulación se administran los siguientes recursos:

- Procesador
- Memoria
- Procesos
- Hilos
- Recursos compartidos
- Archivos de salida

---

# Métricas Calculadas

Al finalizar cada simulación el sistema calcula automáticamente:

- Waiting Time (WT)
- Turnaround Time (TAT)
- Response Time (RT)
- Throughput
- Utilización de CPU
- Makespan

Estas métricas permiten comparar el rendimiento de cada algoritmo implementado.

---

# Archivos Generados

El simulador genera automáticamente archivos de resultados.

Entre ellos:

- Resultados TXT
- Resultados CSV

Estos archivos contienen la información obtenida durante la simulación y permiten realizar análisis posteriores.

---

# Flujo General

```text
Inicio
   │
   ▼
Inicialización
   │
   ▼
Creación de procesos
   │
   ▼
Selección del algoritmo
   │
   ▼
Planificación de CPU
   │
   ▼
Asignación de memoria
   │
   ▼
Sincronización
   │
   ▼
Ejecución
   │
   ▼
Cálculo de métricas
   │
   ▼
Exportación de resultados
   │
   ▼
Fin
```

---

# Estructura del Proyecto

```text
SO-final/
│
├── docs/
│   ├── Arquitectura.md
│   ├── Informe_SIMGESRC.md
│   ├── Manual_Tecnico.md
│   ├── Manual_Usuario.md
│   ├── Presentacion.md
│   └── CHECKLIST_RUBRICA.md
│
├── simgesrc.c
├── Makefile
├── README.md
├── resultados/
└── .gitignore
```

---

# Compilación

Compilar manualmente:

```bash
gcc -std=c11 -Wall -Wextra -pthread simgesrc.c -o simgesrc
```

O utilizando Makefile:

```bash
make
```

---

# Ejecución

```bash
./simgesrc
```

---

# Requisitos

## Hardware

- Intel Core i5 o equivalente
- 8 GB RAM
- 20 GB libres

## Software

- Ubuntu Desktop 24.04 LTS
- GCC
- Make
- Git

---

# Resultados Esperados

Durante la ejecución del simulador se espera:

- Comparar algoritmos de planificación.
- Analizar tiempos de respuesta.
- Evaluar utilización del procesador.
- Observar el comportamiento de la memoria.
- Analizar la concurrencia.
- Generar resultados exportables.

---

# Ventajas del Proyecto

- Arquitectura modular.
- Código organizado.
- Fácil mantenimiento.
- Fácil ampliación.
- Implementación académica completa.
- Compatible con Linux.
- Basado en estándares POSIX.
- Resultados reproducibles.

---

# Limitaciones

- No utiliza bases de datos reales.
- No procesa transacciones bancarias reales.
- No implementa comunicación con servidores externos.
- La memoria es simulada.
- Los tiempos corresponden a un escenario académico.

---

# Trabajos Futuros

El proyecto puede ampliarse incorporando:

- Multilevel Feedback Queue (MLFQ).
- Worst-Fit.
- Paginación.
- Segmentación.
- Algoritmo Banker.
- Semáforos POSIX.
- Interfaz gráfica.
- Base de datos.
- Dashboard de métricas.
- Exportación de gráficos.

---

# Conclusiones

SIMGESRC demuestra de forma práctica la aplicación de los conceptos fundamentales de los sistemas operativos mediante un simulador modular desarrollado en lenguaje C.

La implementación de algoritmos clásicos de planificación, administración de memoria y concurrencia permite analizar el comportamiento de un sistema operativo frente a diferentes cargas de trabajo.

La utilización de Ubuntu Desktop 24.04 LTS, GCC y POSIX Threads proporciona un entorno estable y adecuado para el desarrollo del proyecto, permitiendo representar de manera académica los mecanismos internos utilizados por los sistemas operativos modernos.

La arquitectura modular facilita el mantenimiento, la reutilización del código y la incorporación de nuevas funcionalidades, convirtiendo al proyecto en una base sólida para futuras ampliaciones y prácticas relacionadas con Sistemas Operativos.

---

# Referencias

- Abraham Silberschatz, Peter B. Galvin y Greg Gagne. _Fundamentos de Sistemas Operativos_.
- Andrew S. Tanenbaum y Herbert Bos. _Sistemas Operativos Modernos_.
- GNU Project. _GNU Compiler Collection Documentation_.
- Ubuntu Documentation. _Ubuntu 24.04 LTS_.
- The Open Group. _POSIX Threads Programming_.

---

# Integrantes

- José Carrillo Cárdenas
- Jhonatan Fernández Coronel
- Kieffer Gonzales Chávez
- Juan Diego Salazar Muñoz
