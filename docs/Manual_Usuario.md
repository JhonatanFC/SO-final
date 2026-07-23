# Manual de Usuario

# SIMGESRC

## Sistema de Gestión de Recursos Computacionales para la Plataforma Bancaria Digital de Caja Huancayo

---

# Información General

| Campo            | Descripción              |
| ---------------- | ------------------------ |
| Proyecto         | SIMGESRC                 |
| Tipo             | Simulador Académico      |
| Curso            | Sistemas Operativos      |
| Plataforma       | Ubuntu Desktop 24.04 LTS |
| Lenguaje         | C11                      |
| Público objetivo | Estudiantes y docentes   |

---

# 1. Introducción

El presente Manual de Usuario tiene como finalidad orientar al usuario en la instalación, compilación y utilización del simulador **SIMGESRC (Sistema de Gestión de Recursos Computacionales para la Plataforma Bancaria Digital de Caja Huancayo)**.

El simulador fue desarrollado con fines académicos para representar el funcionamiento de diversos mecanismos de administración de recursos implementados por un sistema operativo moderno, utilizando como escenario una plataforma bancaria digital.

Este manual describe los pasos necesarios para ejecutar el programa, comprender su funcionamiento e interpretar los resultados generados.

---

# 2. Objetivo

Permitir que cualquier usuario pueda instalar, ejecutar y utilizar correctamente el simulador sin necesidad de conocer el funcionamiento interno del código fuente.

---

# 3. Requisitos del Sistema

## Hardware

- Procesador Intel Core i5 o equivalente
- 8 GB de memoria RAM
- 20 GB de espacio libre en disco

## Software

- Ubuntu Desktop 24.04 LTS
- GCC
- GNU Make
- Terminal de Linux

---

# 4. Instalación

## Paso 1

Abrir una terminal de Ubuntu.

---

## Paso 2

Ingresar al directorio del proyecto.

Ejemplo:

```bash
cd SO-final
```

---

## Paso 3

Verificar que existan los siguientes archivos:

```text
simgesrc.c
Makefile
README.md
docs/
```

---

# 5. Compilación

Para compilar el proyecto ejecutar:

```bash
make
```

Si no se utiliza Makefile:

```bash
gcc -std=c11 -Wall -Wextra -pthread simgesrc.c -o simgesrc
```

Cuando la compilación finalice correctamente se generará el archivo ejecutable:

```text
simgesrc
```

---

# 6. Ejecución

Desde la terminal ejecutar:

```bash
./simgesrc
```

El programa iniciará automáticamente.

---

# 7. Funcionamiento General

Al iniciar el simulador se cargan automáticamente los procesos definidos para la simulación.

Posteriormente el usuario podrá seleccionar el algoritmo de planificación que desea evaluar.

Durante la ejecución el sistema administrará:

- Procesos
- CPU
- Memoria
- Recursos compartidos
- Concurrencia

Finalmente se mostrarán las métricas obtenidas y se generarán los archivos de resultados.

---

# 8. Operaciones Simuladas

El escenario representa operaciones comunes de una plataforma bancaria.

Entre ellas:

- Inicio de sesión
- Consulta de saldo
- Transferencias
- Pago de servicios
- Actualización de registros
- Liquidación nocturna

Estas operaciones son simuladas únicamente con fines académicos.

---

# 9. Algoritmos Disponibles

El usuario puede ejecutar las simulaciones utilizando los siguientes algoritmos.

## FCFS

Ejecuta los procesos según el orden de llegada.

---

## SJF

Selecciona primero el proceso con menor tiempo de ejecución.

---

## Round Robin

Distribuye el procesador utilizando un quantum fijo.

Configuración utilizada:

```text
Quantum = 4
```

---

## Prioridades con Aging

Selecciona los procesos considerando su prioridad y aplica la técnica Aging para evitar la inanición.

---

# 10. Administración de Memoria

El simulador permite evaluar dos estrategias de asignación de memoria.

## First-Fit

Asigna el primer bloque libre disponible.

## Best-Fit

Selecciona el bloque libre más pequeño que pueda satisfacer la solicitud.

---

# 11. Resultados Generados

Al finalizar la simulación el sistema muestra información relacionada con el desempeño de los algoritmos.

Entre las métricas calculadas se encuentran:

- Tiempo de espera (Waiting Time)
- Tiempo de retorno (Turnaround Time)
- Tiempo de respuesta (Response Time)
- Throughput
- Utilización de CPU
- Makespan

---

# 12. Archivos Generados

Después de ejecutar la simulación el sistema puede generar archivos de resultados dentro del directorio correspondiente.

Ejemplo:

```text
resultados/
├── resultados.txt
└── resultados.csv
```

Estos archivos contienen la información obtenida durante la ejecución.

---

# 13. Flujo de Uso

```text
Abrir Terminal
       │
       ▼
Ingresar al Proyecto
       │
       ▼
Compilar
       │
       ▼
Ejecutar
       │
       ▼
Seleccionar Algoritmo
       │
       ▼
Ejecutar Simulación
       │
       ▼
Visualizar Resultados
       │
       ▼
Revisar Archivos Generados
```

---

# 14. Recomendaciones

Se recomienda:

- Ejecutar el simulador desde Ubuntu.
- Compilar nuevamente después de realizar cambios.
- Mantener una copia de los archivos de resultados.
- Utilizar la versión más reciente del repositorio.

---

# 15. Solución de Problemas

## El programa no compila

Verificar que GCC esté instalado.

```bash
gcc --version
```

---

## El ejecutable no existe

Ejecutar nuevamente:

```bash
make
```

---

## Error de permisos

Otorgar permisos al ejecutable.

```bash
chmod +x simgesrc
```

---

## Error durante la ejecución

Verificar que todos los archivos del proyecto se encuentren en la misma carpeta.

---

# 16. Preguntas Frecuentes

## ¿El simulador realiza operaciones bancarias reales?

No.

El proyecto representa únicamente una simulación académica.

---

## ¿Necesita conexión a Internet?

No.

Todo el procesamiento se realiza de forma local.

---

## ¿Puede ejecutarse varias veces?

Sí.

Cada simulación puede ejecutarse tantas veces como sea necesario.

---

## ¿Se pueden modificar los algoritmos?

Sí.

El código fuente está disponible para realizar modificaciones y nuevas implementaciones.

---

# 17. Buenas Prácticas

- No modificar archivos sin realizar una copia de seguridad.
- Compilar nuevamente después de cada cambio.
- Mantener organizada la estructura del proyecto.
- Actualizar la documentación cuando se agreguen nuevas funcionalidades.

---

# 18. Conclusiones

El Manual de Usuario proporciona las instrucciones necesarias para instalar, compilar y utilizar correctamente el simulador SIMGESRC.

Gracias a la organización del proyecto y a la simplicidad de su ejecución, el usuario puede evaluar diferentes algoritmos de planificación, administración de memoria y concurrencia, comprendiendo de forma práctica los principales conceptos de la asignatura de Sistemas Operativos.

---

# Soporte

Para consultas relacionadas con el proyecto se recomienda revisar la documentación incluida en el directorio:

```text
docs/
```

Documentos disponibles:

- Arquitectura.md
- Informe_SIMGESRC.md
- Manual_Tecnico.md
- Presentacion.md
- CHECKLIST_RUBRICA.md

---

# Integrantes

- José Carrillo Cárdenas
- Jhonatan Fernández Coronel
- Kieffer Gonzales Chávez
- Juan Diego Salazar Muñoz
