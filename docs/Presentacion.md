# Presentación del Proyecto

# SIMGESRC

## Sistema de Gestión de Recursos Computacionales para la Plataforma Bancaria Digital de Caja Huancayo

---

# Descripción General

SIMGESRC es un simulador académico desarrollado como proyecto final de la asignatura de **Sistemas Operativos** de la Universidad Peruana Los Andes.

El proyecto fue implementado en lenguaje **C11** sobre **Ubuntu Desktop 24.04 LTS**, utilizando **POSIX Threads (pthread)** para representar mecanismos fundamentales de administración de recursos computacionales en un entorno inspirado en una plataforma bancaria digital.

El simulador integra conceptos esenciales de sistemas operativos como planificación de CPU, administración de memoria, concurrencia, sincronización y evaluación de rendimiento mediante diferentes algoritmos clásicos.

---

# Objetivo del Proyecto

Desarrollar un simulador académico que permita representar el funcionamiento de un sistema operativo moderno mediante la gestión de procesos, memoria y recursos compartidos, utilizando como escenario una plataforma bancaria digital.

---

# Objetivos Específicos

- Implementar algoritmos clásicos de planificación de CPU.
- Simular la administración dinámica de memoria.
- Incorporar concurrencia mediante POSIX Threads.
- Implementar sincronización utilizando mutex.
- Calcular métricas de rendimiento.
- Generar resultados para su análisis.

---

# Problema

Las plataformas bancarias digitales procesan miles de operaciones simultáneamente.

Estas operaciones requieren que el sistema operativo administre correctamente:

- Procesos.
- Tiempo de CPU.
- Memoria.
- Recursos compartidos.
- Ejecución concurrente.

Una administración inadecuada puede ocasionar:

- Incremento en los tiempos de respuesta.
- Baja utilización del procesador.
- Conflictos entre procesos.
- Condiciones de carrera.
- Desperdicio de memoria.

SIMGESRC permite representar estos escenarios mediante una simulación académica.

---

# Escenario Simulado

El proyecto utiliza como referencia una plataforma bancaria digital inspirada en Caja Huancayo.

Durante la simulación se representan operaciones como:

- Inicio de sesión.
- Consulta de saldo.
- Transferencias.
- Pago de servicios.
- Actualización de registros.
- Liquidación nocturna.

Cada operación es tratada como un proceso independiente administrado por el sistema operativo.

---

# Tecnologías Utilizadas

## Sistema Operativo

- Ubuntu Desktop 24.04 LTS

## Lenguaje

- C11

## Compilador

- GCC

## Bibliotecas

- POSIX Threads (pthread)

## Herramientas

- Make
- Git
- GitHub
- Visual Studio Code

---

# Componentes Principales

El simulador está organizado en módulos independientes.

Los principales componentes son:

- Programa principal.
- Gestor de procesos.
- Planificador de CPU.
- Administrador de memoria.
- Módulo de sincronización.
- Gestor de métricas.
- Exportador de resultados.

Esta estructura facilita el mantenimiento y la ampliación del proyecto.

---

# Algoritmos Implementados

## Planificación de CPU

El simulador incorpora cuatro algoritmos clásicos.

### FCFS

Planifica los procesos según el orden de llegada.

### SJF

Ejecuta primero el proceso con menor tiempo de CPU.

### Round Robin

Distribuye el procesador mediante un quantum fijo.

```text
Quantum = 4
```

### Prioridades con Aging

Incrementa progresivamente la prioridad de los procesos para evitar la inanición.

---

# Administración de Memoria

Se implementan dos algoritmos de asignación.

## First-Fit

Selecciona el primer bloque libre disponible.

## Best-Fit

Selecciona el bloque libre más pequeño que satisfaga la solicitud.

Estos algoritmos permiten analizar el comportamiento de la memoria durante la ejecución.

---

# Concurrencia y Sincronización

El simulador incorpora programación concurrente utilizando POSIX Threads.

Para garantizar el acceso seguro a los recursos compartidos se implementan mutex mediante la biblioteca pthread.

Esto permite evitar condiciones de carrera y mantener la consistencia de los datos.

---

# Métricas Calculadas

Al finalizar cada simulación el sistema calcula automáticamente:

- Waiting Time (WT).
- Turnaround Time (TAT).
- Response Time (RT).
- Throughput.
- Utilización del procesador.
- Makespan.

Estas métricas permiten comparar objetivamente el rendimiento de los algoritmos implementados.

---

# Flujo General

```text
Inicio
   │
   ▼
Inicialización
   │
   ▼
Carga de procesos
   │
   ▼
Selección del algoritmo
   │
   ▼
Planificación de CPU
   │
   ▼
Administración de memoria
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

# Estructura del Repositorio

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

Compilar utilizando:

```bash
make
```

o

```bash
gcc -std=c11 -Wall -Wextra -pthread simgesrc.c -o simgesrc
```

---

# Ejecución

```bash
./simgesrc
```

El programa iniciará la simulación y permitirá ejecutar los algoritmos implementados.

---

# Resultados Esperados

Con la ejecución del simulador es posible:

- Comparar algoritmos de planificación.
- Analizar tiempos de respuesta.
- Evaluar la utilización del procesador.
- Observar el comportamiento de la memoria.
- Analizar la concurrencia.
- Generar archivos de resultados para posteriores análisis.

---

# Beneficios del Proyecto

- Arquitectura modular.
- Código organizado.
- Fácil mantenimiento.
- Fácil ampliación.
- Compatible con Linux.
- Basado en estándares POSIX.
- Implementación académica completa.
- Resultados reproducibles.

---

# Limitaciones

El proyecto fue desarrollado con fines académicos.

Por ello:

- No utiliza bases de datos reales.
- No procesa operaciones bancarias reales.
- No implementa autenticación bancaria.
- No mantiene comunicación con servidores externos.
- La memoria administrada corresponde a una simulación.

---

# Futuras Mejoras

El proyecto puede ampliarse incorporando:

- MLFQ.
- Algoritmo Banker.
- Paginación.
- Segmentación.
- Semáforos.
- Dashboard de métricas.
- Interfaz gráfica.
- Exportación automática de gráficos.
- Integración con bases de datos.

---

# Conclusiones

SIMGESRC demuestra la aplicación práctica de los conceptos fundamentales de los sistemas operativos mediante un simulador modular desarrollado en C.

La implementación de algoritmos de planificación, administración de memoria y concurrencia permite evaluar el comportamiento de diferentes estrategias de gestión de recursos, proporcionando una herramienta académica útil para el aprendizaje de Sistemas Operativos.

---

# Documentación Disponible

El proyecto cuenta con la siguiente documentación técnica:

- Arquitectura.md
- Informe_SIMGESRC.md
- Manual_Tecnico.md
- Manual_Usuario.md
- CHECKLIST_RUBRICA.md

Toda la documentación se encuentra disponible en el directorio:

```text
docs/
```

---

# Integrantes

- José Carrillo Cárdenas
- Jhonatan Fernández Coronel
- Kieffer Gonzales Chávez
- Juan Diego Salazar Muñoz

---

# Universidad

**Universidad Peruana Los Andes**

**Escuela Profesional de Ingeniería de Sistemas y Computación**

**Curso:** Sistemas Operativos

**Proyecto Final:** SIMGESRC

**Periodo Académico:** 2026-I
