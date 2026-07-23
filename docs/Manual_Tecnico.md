# Manual Técnico

# SIMGESRC

## Sistema de Gestión de Recursos Computacionales para la Plataforma Bancaria Digital de Caja Huancayo

---

# Información General

| Campo             | Descripción              |
| ----------------- | ------------------------ |
| Proyecto          | SIMGESRC                 |
| Curso             | Sistemas Operativos      |
| Lenguaje          | C11                      |
| Sistema Operativo | Ubuntu Desktop 24.04 LTS |
| Compilador        | GCC                      |
| Biblioteca        | POSIX Threads (pthread)  |
| Tipo              | Simulador Académico      |

---

# 1. Introducción

El presente Manual Técnico describe la instalación, compilación, estructura, funcionamiento y mantenimiento del proyecto **SIMGESRC (Sistema de Gestión de Recursos Computacionales para la Plataforma Bancaria Digital de Caja Huancayo)**.

Su objetivo es servir como guía para desarrolladores, docentes y estudiantes que deseen comprender la arquitectura interna del sistema, compilar el proyecto, modificar el código fuente o incorporar nuevas funcionalidades.

Este documento complementa el informe técnico y la documentación general del proyecto.

---

# 2. Objetivo del Manual

Este manual tiene como finalidad:

- Describir la estructura técnica del proyecto.
- Explicar los requisitos de instalación.
- Documentar el proceso de compilación.
- Mostrar la organización del código fuente.
- Facilitar el mantenimiento.
- Permitir futuras ampliaciones.

---

# 3. Requisitos del Sistema

## Hardware

| Componente | Requisito                   |
| ---------- | --------------------------- |
| Procesador | Intel Core i5 o equivalente |
| RAM        | 8 GB                        |
| Disco      | 20 GB libres                |

---

## Software

- Ubuntu Desktop 24.04 LTS
- GCC
- GNU Make
- Git
- Visual Studio Code (opcional)

---

# 4. Tecnologías Utilizadas

| Tecnología    | Uso                      |
| ------------- | ------------------------ |
| C11           | Desarrollo del simulador |
| GCC           | Compilación              |
| POSIX Threads | Concurrencia             |
| Make          | Automatización           |
| Git           | Control de versiones     |
| GitHub        | Repositorio remoto       |

---

# 5. Estructura del Proyecto

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
│
└── .gitignore
```

---

# 6. Descripción de los Archivos

## simgesrc.c

Archivo principal del proyecto.

Contiene:

- Programa principal.
- Gestión de procesos.
- Planificación.
- Memoria.
- Concurrencia.
- Métricas.
- Exportación.

---

## Makefile

Automatiza el proceso de compilación.

Permite ejecutar:

```bash
make
```

para generar el ejecutable.

---

## README.md

Documento principal del repositorio.

Contiene información general del proyecto.

---

## docs/

Directorio que almacena toda la documentación técnica.

---

## resultados/

Contiene los archivos generados durante las simulaciones.

Ejemplos:

- resultados.txt
- resultados.csv

---

# 7. Arquitectura del Sistema

El proyecto sigue una arquitectura modular.

Componentes principales:

- Programa principal
- Gestor de procesos
- Planificador
- Administrador de memoria
- Sincronización
- Métricas
- Exportación

La descripción detallada se encuentra en:

```text
docs/Arquitectura.md
```

---

# 8. Compilación

Desde la carpeta del proyecto ejecutar:

```bash
make
```

o

```bash
gcc -std=c11 -Wall -Wextra -pthread simgesrc.c -o simgesrc
```

Si la compilación fue exitosa se generará el archivo:

```text
simgesrc
```

---

# 9. Ejecución

Ejecutar:

```bash
./simgesrc
```

El programa iniciará la simulación y mostrará el menú principal.

---

# 10. Algoritmos Implementados

## Planificación de CPU

Se implementan cuatro algoritmos clásicos:

### FCFS

- No apropiativo.
- Orden de llegada.

### SJF

- Menor ráfaga primero.

### Round Robin

Quantum utilizado:

```text
4
```

### Prioridades con Aging

Evita la inanición incrementando progresivamente la prioridad de los procesos.

---

# 11. Administración de Memoria

Se implementan dos algoritmos:

## First-Fit

Selecciona el primer bloque libre disponible.

## Best-Fit

Selecciona el bloque libre más pequeño que satisfaga la solicitud.

---

# 12. Concurrencia

La concurrencia fue desarrollada mediante:

```text
POSIX Threads
```

Se utilizan mutex para proteger recursos compartidos.

Objetivos:

- Evitar condiciones de carrera.
- Garantizar exclusión mutua.
- Mantener consistencia.

---

# 13. Métricas

El sistema calcula automáticamente:

- Waiting Time
- Turnaround Time
- Response Time
- Throughput
- Utilización de CPU
- Makespan

Estas métricas permiten comparar el desempeño de cada algoritmo.

---

# 14. Flujo de Ejecución

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
Planificación
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
Exportación
   │
   ▼
Fin
```

---

# 15. Mantenimiento

Para realizar modificaciones al proyecto se recomienda:

- Mantener la estructura original.
- Documentar nuevas funciones.
- Probar cada modificación.
- Actualizar la documentación.
- Versionar mediante Git.

---

# 16. Agregar un Nuevo Algoritmo

Para incorporar un nuevo algoritmo se recomienda:

1. Implementar la lógica del algoritmo.
2. Integrarlo al menú principal.
3. Adaptar el cálculo de métricas.
4. Validar los resultados.
5. Actualizar la documentación.

---

# 17. Solución de Problemas

## Error

```text
gcc: command not found
```

**Solución**

Instalar GCC.

---

## Error

```text
make: command not found
```

**Solución**

Instalar GNU Make.

---

## Error

```text
Permission denied
```

**Solución**

Dar permisos de ejecución:

```bash
chmod +x simgesrc
```

---

## Error

```text
pthread.h: No such file
```

**Solución**

Verificar la instalación del compilador y utilizar la opción:

```bash
-pthread
```

durante la compilación.

---

# 18. Buenas Prácticas

- Mantener funciones pequeñas.
- Utilizar nombres descriptivos.
- Documentar cambios.
- Evitar duplicación de código.
- Realizar pruebas antes de cada commit.
- Mantener actualizado el repositorio.

---

# 19. Futuras Mejoras

El sistema puede ampliarse incorporando:

- MLFQ.
- Semáforos.
- Algoritmo Banker.
- Paginación.
- Segmentación.
- Dashboard gráfico.
- Interfaz gráfica.
- Exportación automática de gráficos.
- Integración con base de datos.

---

# 20. Conclusiones

El Manual Técnico documenta los aspectos fundamentales necesarios para comprender el funcionamiento interno de SIMGESRC.

La organización modular del proyecto facilita el mantenimiento, la ampliación y la reutilización del código, permitiendo que nuevos desarrolladores comprendan rápidamente la estructura del sistema.

El uso de Ubuntu Desktop 24.04 LTS, GCC y POSIX Threads proporciona un entorno adecuado para el desarrollo de aplicaciones relacionadas con la administración de procesos, memoria y concurrencia, cumpliendo los objetivos académicos del proyecto.

---

# Referencias

- GNU Compiler Collection (GCC)
- Ubuntu 24.04 LTS Documentation
- POSIX Threads Documentation
- Abraham Silberschatz, _Fundamentos de Sistemas Operativos_
- Andrew S. Tanenbaum, _Sistemas Operativos Modernos_

---

# Integrantes

- José Carrillo Cárdenas
- Jhonatan Fernández Coronel
- Kieffer Gonzales Chávez
- Juan Diego Salazar Muñoz
