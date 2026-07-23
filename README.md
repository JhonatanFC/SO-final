# SIMGESRC – Caja Huancayo

Proyecto final de Sistemas Operativos desarrollado en lenguaje C para Ubuntu Desktop 24.04 LTS.

## Escenario

Plataforma Bancaria Digital de Caja Huancayo.

## Contenidos implementados

### Módulo 2 – Planificación

- FCFS.
- SJF no apropiativo.
- Round Robin con quantum 4.
- Prioridades con Aging.
- Waiting Time, Turnaround Time, Response Time, Throughput, uso de CPU y makespan.
- Diagramas de Gantt en terminal.
- Carga de 10 procesos con llegadas escalonadas.

### Módulo 3 – Sincronización

- Dos secciones críticas: saldo compartido y log de auditoría.
- Condición de carrera sin mutex.
- Solución con `pthread_mutex_t`.
- Prueba con 8 hilos.
- Medición del overhead.
- Deadlock entre dos transferencias.
- Condiciones de Coffman.
- Prevención mediante orden total de adquisición de mutex.

### Módulo 4 – Memoria

- Partición variable.
- First-Fit y Best-Fit.
- 15 operaciones de asignación y liberación.
- Mapa intermedio y final de particiones.
- Fragmentación externa.
- Huecos libres y asignaciones denegadas.

### Módulo 5 – Integración

- Asignación de memoria con Best-Fit.
- Planificación con Round Robin.
- Actualización de saldo con mutex.
- Throughput, tiempo de respuesta, uso de CPU, uso máximo de memoria, fragmentación, conflictos y deadlocks.
- Dos cuellos de botella y plan de optimización cuantificado.

## Compilación

```bash
make
```

## Ejecución

```bash
./simgesrc
```

## Demostración automática

```bash
make demo
```

## Limpiar

```bash
make clean
```

## Resultados generados

- `resultados/planificacion.csv`
- `resultados/sincronizacion.txt`
- `resultados/memoria.csv`
- `resultados/integracion.txt`

## Sistema objetivo

Ubuntu Desktop 24.04 LTS con GCC y POSIX Threads.
# SO-final
