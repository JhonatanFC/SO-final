/*
===============================================================================
SIMGESRC - SISTEMA DE GESTION DE RECURSOS COMPUTACIONALES
Escenario: Plataforma Bancaria Digital de Caja Huancayo
Lenguaje: C11
Sistema objetivo: Ubuntu Desktop 24.04 LTS

Contenidos aplicados de clase:
- FCFS, SJF no apropiativo, Round Robin y Prioridades con Aging.
- Seccion critica, condicion de carrera, mutex y deadlock.
- Particion variable, First-Fit, Best-Fit y fragmentacion externa.
- Integracion de planificacion, sincronizacion y memoria.
===============================================================================
*/

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sched.h>
#include <time.h>

#define TOTAL_PROCESOS 10
#define QUANTUM 4
#define AGING_INTERVALO 5
#define NUM_HILOS 8
#define OPERACIONES_HILO 2000
#define MEMORIA_TOTAL 2048
#define MAX_PARTICIONES 64
#define MAX_GANTT 256

typedef struct {
    int pid;
    char nombre[32];
    char funcion[80];
    int llegada;
    int rafaga;
    int prioridad;
    int memoria;
    int tipo; /* 0 = I/O-bound, 1 = CPU-bound */

    int restante;
    int inicio;
    int fin;
    int espera;
    int retorno;
    int respuesta;
    int completado;
} Proceso;

typedef struct {
    char algoritmo[32];
    double espera_promedio;
    double retorno_promedio;
    double respuesta_promedio;
    double throughput;
    double uso_cpu;
    int makespan;
} Metricas;

typedef struct {
    char nombre[32];
    int inicio;
    int duracion;
} BloqueGantt;

typedef struct {
    int inicio;
    int tamanio;
    int libre;
    char proceso[32];
} Particion;

typedef struct {
    Particion bloques[MAX_PARTICIONES];
    int cantidad;
    int denegadas;
} Memoria;

typedef enum {
    FIRST_FIT,
    BEST_FIT
} Estrategia;

/* ============================== PROTOTIPOS ============================== */

static void limpiar_entrada(void);
static void pausa(void);
static void encabezado(void);
static void crear_directorio_resultados(void);

static void cargar_procesos(Proceso procesos[]);
static void copiar_procesos(Proceso destino[], const Proceso origen[]);
static void mostrar_carga(const Proceso procesos[]);

static Metricas calcular_metricas(const char *nombre, const Proceso procesos[],
                                  int makespan, int ocupado);
static void mostrar_resultados(const char *algoritmo, const Proceso procesos[],
                               const Metricas *metricas);
static void mostrar_gantt(const BloqueGantt gantt[], int cantidad);
static Metricas ejecutar_fcfs(const Proceso base[], int mostrar);
static Metricas ejecutar_sjf(const Proceso base[], int mostrar);
static Metricas ejecutar_round_robin(const Proceso base[], int mostrar);
static Metricas ejecutar_prioridad_aging(const Proceso base[], int mostrar);
static void guardar_planificacion_csv(const Metricas resultados[], int cantidad);
static void modulo_planificacion(const Proceso base[]);

static double reloj_segundos(void);
static void *depositar_sin_mutex(void *arg);
static void *depositar_con_mutex(void *arg);
static void *escribir_log_con_mutex(void *arg);
static void demostrar_deadlock(void);
static void modulo_sincronizacion(void);

static void memoria_inicializar(Memoria *memoria);
static int memoria_asignar(Memoria *memoria, const char *nombre, int tamanio,
                           Estrategia estrategia);
static int memoria_liberar(Memoria *memoria, const char *nombre);
static void memoria_fusionar(Memoria *memoria);
static void memoria_mostrar(const Memoria *memoria, FILE *salida);
static double fragmentacion_externa(const Memoria *memoria);
static int contar_huecos_libres(const Memoria *memoria);
static void ejecutar_prueba_memoria(Estrategia estrategia, int mostrar,
                                    double *fragmentacion_final,
                                    int *huecos_finales, int *denegadas_finales);
static void modulo_memoria(void);

static void modulo_integrado(const Proceso base[]);
static void demostracion_completa(const Proceso base[]);
static void menu_principal(void);

/* ============================== UTILIDADES ============================== */

static void limpiar_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

static void pausa(void) {
    printf("\nPresione ENTER para continuar...");
    limpiar_entrada();
    getchar();
}

static void encabezado(void) {
    printf("\n============================================================\n");
    printf(" SIMGESRC - PLATAFORMA BANCARIA DIGITAL CAJA HUANCAYO\n");
    printf(" Sistema de Gestion de Recursos Computacionales\n");
    printf("============================================================\n");
}

static void crear_directorio_resultados(void) {
#ifdef _WIN32
    system("if not exist resultados mkdir resultados");
#else
    system("mkdir -p resultados");
#endif
}

/* ============================== PROCESOS ============================== */

static void cargar_procesos(Proceso p[]) {
    Proceso carga[TOTAL_PROCESOS] = {
        {1, "Consulta1", "Consultar el saldo de una cuenta", 0, 2, 2, 64, 0,
         2, -1, 0, 0, 0, 0, 0},
        {2, "Consulta2", "Consultar el saldo de una cuenta", 1, 2, 2, 64, 0,
         2, -1, 0, 0, 0, 0, 0},
        {3, "Login1", "Validar credenciales del cliente", 2, 3, 1, 96, 0,
         3, -1, 0, 0, 0, 0, 0},
        {4, "Transferencia1", "Transferir dinero entre dos cuentas", 3, 8, 1, 256, 0,
         8, -1, 0, 0, 0, 0, 0},
        {5, "Pago1", "Procesar el pago de un servicio", 4, 6, 2, 192, 0,
         6, -1, 0, 0, 0, 0, 0},
        {6, "Consulta3", "Consultar el saldo de una cuenta", 5, 2, 2, 64, 0,
         2, -1, 0, 0, 0, 0, 0},
        {7, "ActualizaBD", "Actualizar registros de la base de datos", 6, 4, 3, 128, 0,
         4, -1, 0, 0, 0, 0, 0},
        {8, "Transferencia2", "Transferir dinero entre dos cuentas", 7, 8, 1, 256, 0,
         8, -1, 0, 0, 0, 0, 0},
        {9, "Pago2", "Procesar el pago de un servicio", 8, 6, 2, 192, 0,
         6, -1, 0, 0, 0, 0, 0},
        {10, "Liquidacion", "Ejecutar la liquidacion nocturna por lotes", 10, 12, 5, 512, 1,
         12, -1, 0, 0, 0, 0, 0}
    };

    memcpy(p, carga, sizeof(carga));
}

static void copiar_procesos(Proceso destino[], const Proceso origen[]) {
    memcpy(destino, origen, sizeof(Proceso) * TOTAL_PROCESOS);
}

static void mostrar_carga(const Proceso p[]) {
    printf("\n====================================================================================================\n");
    printf("PID  PROCESO              LLEGADA  RAFAGA  PRIORIDAD  MEMORIA  TIPO        FUNCION\n");
    printf("====================================================================================================\n");

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        printf("%-4d %-20s %-8d %-7d %-10d %-8d %-11s %s\n",
               p[i].pid,
               p[i].nombre,
               p[i].llegada,
               p[i].rafaga,
               p[i].prioridad,
               p[i].memoria,
               p[i].tipo == 0 ? "I/O-bound" : "CPU-bound",
               p[i].funcion);
    }

    printf("====================================================================================================\n");
}

/* ============================== PLANIFICACION ============================== */

static Metricas calcular_metricas(const char *nombre, const Proceso p[],
                                  int makespan, int ocupado) {
    Metricas m = {0};
    snprintf(m.algoritmo, sizeof(m.algoritmo), "%s", nombre);

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        m.espera_promedio += p[i].espera;
        m.retorno_promedio += p[i].retorno;
        m.respuesta_promedio += p[i].respuesta;
    }

    m.espera_promedio /= TOTAL_PROCESOS;
    m.retorno_promedio /= TOTAL_PROCESOS;
    m.respuesta_promedio /= TOTAL_PROCESOS;
    m.makespan = makespan;
    m.throughput = makespan > 0 ? (double)TOTAL_PROCESOS / makespan : 0.0;
    m.uso_cpu = makespan > 0 ? (100.0 * ocupado / makespan) : 0.0;
    return m;
}

static void mostrar_resultados(const char *algoritmo, const Proceso p[],
                               const Metricas *m) {
    printf("\n======================== %s ========================\n", algoritmo);
    printf("PID  PROCESO              INICIO  FIN  WT   TAT  RT\n");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        printf("%-4d %-20s %-7d %-4d %-4d %-4d %-4d\n",
               p[i].pid,
               p[i].nombre,
               p[i].inicio,
               p[i].fin,
               p[i].espera,
               p[i].retorno,
               p[i].respuesta);
    }

    printf("---------------------------------------------------------------\n");
    printf("Waiting Time promedio:       %.2f\n", m->espera_promedio);
    printf("Turnaround Time promedio:    %.2f\n", m->retorno_promedio);
    printf("Response Time promedio:      %.2f\n", m->respuesta_promedio);
    printf("Throughput:                   %.3f procesos/u.t.\n", m->throughput);
    printf("Uso de CPU:                   %.2f%%\n", m->uso_cpu);
    printf("Makespan:                     %d unidades\n", m->makespan);
}

static void mostrar_gantt(const BloqueGantt gantt[], int cantidad) {
    printf("\nDiagrama de Gantt:\n");

    for (int i = 0; i < cantidad; i++) {
        printf("| %s ", gantt[i].nombre);
    }
    printf("|\n");

    if (cantidad > 0) {
        printf("%d", gantt[0].inicio);
        for (int i = 0; i < cantidad; i++) {
            printf(" -> %d", gantt[i].inicio + gantt[i].duracion);
        }
    }
    printf("\n");
}

static Metricas ejecutar_fcfs(const Proceso base[], int mostrar) {
    Proceso p[TOTAL_PROCESOS];
    BloqueGantt gantt[TOTAL_PROCESOS];
    copiar_procesos(p, base);

    for (int i = 0; i < TOTAL_PROCESOS - 1; i++) {
        for (int j = i + 1; j < TOTAL_PROCESOS; j++) {
            if (p[j].llegada < p[i].llegada) {
                Proceso temporal = p[i];
                p[i] = p[j];
                p[j] = temporal;
            }
        }
    }

    int tiempo = 0;
    int ocupado = 0;

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        if (tiempo < p[i].llegada) {
            tiempo = p[i].llegada;
        }

        p[i].inicio = tiempo;
        p[i].respuesta = p[i].inicio - p[i].llegada;
        p[i].espera = p[i].respuesta;
        p[i].fin = tiempo + p[i].rafaga;
        p[i].retorno = p[i].fin - p[i].llegada;

        snprintf(gantt[i].nombre, sizeof(gantt[i].nombre), "%s", p[i].nombre);
        gantt[i].inicio = tiempo;
        gantt[i].duracion = p[i].rafaga;

        tiempo = p[i].fin;
        ocupado += p[i].rafaga;
    }

    Metricas m = calcular_metricas("FCFS", p, tiempo, ocupado);

    if (mostrar) {
        mostrar_resultados("FCFS", p, &m);
        mostrar_gantt(gantt, TOTAL_PROCESOS);
    }

    return m;
}

static Metricas ejecutar_sjf(const Proceso base[], int mostrar) {
    Proceso p[TOTAL_PROCESOS];
    BloqueGantt gantt[TOTAL_PROCESOS];
    copiar_procesos(p, base);

    int tiempo = 0;
    int completados = 0;
    int ocupado = 0;
    int cantidad_gantt = 0;

    while (completados < TOTAL_PROCESOS) {
        int elegido = -1;

        for (int i = 0; i < TOTAL_PROCESOS; i++) {
            if (!p[i].completado && p[i].llegada <= tiempo) {
                if (elegido == -1 ||
                    p[i].rafaga < p[elegido].rafaga ||
                    (p[i].rafaga == p[elegido].rafaga &&
                     p[i].llegada < p[elegido].llegada)) {
                    elegido = i;
                }
            }
        }

        if (elegido == -1) {
            tiempo++;
            continue;
        }

        p[elegido].inicio = tiempo;
        p[elegido].respuesta = tiempo - p[elegido].llegada;
        p[elegido].espera = p[elegido].respuesta;
        p[elegido].fin = tiempo + p[elegido].rafaga;
        p[elegido].retorno = p[elegido].fin - p[elegido].llegada;
        p[elegido].completado = 1;

        snprintf(gantt[cantidad_gantt].nombre,
                 sizeof(gantt[cantidad_gantt].nombre),
                 "%s", p[elegido].nombre);
        gantt[cantidad_gantt].inicio = tiempo;
        gantt[cantidad_gantt].duracion = p[elegido].rafaga;
        cantidad_gantt++;

        tiempo = p[elegido].fin;
        ocupado += p[elegido].rafaga;
        completados++;
    }

    Metricas m = calcular_metricas("SJF", p, tiempo, ocupado);

    if (mostrar) {
        mostrar_resultados("SJF NO APROPIATIVO", p, &m);
        mostrar_gantt(gantt, cantidad_gantt);
    }

    return m;
}

static Metricas ejecutar_round_robin(const Proceso base[], int mostrar) {
    Proceso p[TOTAL_PROCESOS];
    BloqueGantt gantt[MAX_GANTT];
    int cola[MAX_GANTT];
    int en_cola[TOTAL_PROCESOS] = {0};

    copiar_procesos(p, base);

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        p[i].restante = p[i].rafaga;
        p[i].inicio = -1;
    }

    int frente = 0;
    int final = 0;
    int tiempo = 0;
    int completados = 0;
    int ocupado = 0;
    int cantidad_gantt = 0;

    while (completados < TOTAL_PROCESOS) {
        for (int i = 0; i < TOTAL_PROCESOS; i++) {
            if (!en_cola[i] && !p[i].completado && p[i].llegada <= tiempo) {
                cola[final++] = i;
                en_cola[i] = 1;
            }
        }

        if (frente == final) {
            tiempo++;
            continue;
        }

        int indice = cola[frente++];

        if (p[indice].inicio == -1) {
            p[indice].inicio = tiempo;
            p[indice].respuesta = tiempo - p[indice].llegada;
        }

        int ejecucion = p[indice].restante < QUANTUM
                            ? p[indice].restante
                            : QUANTUM;

        snprintf(gantt[cantidad_gantt].nombre,
                 sizeof(gantt[cantidad_gantt].nombre),
                 "%s", p[indice].nombre);
        gantt[cantidad_gantt].inicio = tiempo;
        gantt[cantidad_gantt].duracion = ejecucion;
        cantidad_gantt++;

        tiempo += ejecucion;
        ocupado += ejecucion;
        p[indice].restante -= ejecucion;

        for (int j = 0; j < TOTAL_PROCESOS; j++) {
            if (!en_cola[j] && !p[j].completado && p[j].llegada <= tiempo) {
                cola[final++] = j;
                en_cola[j] = 1;
            }
        }

        if (p[indice].restante > 0) {
            cola[final++] = indice;
        } else {
            p[indice].completado = 1;
            p[indice].fin = tiempo;
            p[indice].retorno = p[indice].fin - p[indice].llegada;
            p[indice].espera = p[indice].retorno - p[indice].rafaga;
            completados++;
        }
    }

    Metricas m = calcular_metricas("Round Robin", p, tiempo, ocupado);

    if (mostrar) {
        mostrar_resultados("ROUND ROBIN (QUANTUM = 4)", p, &m);
        mostrar_gantt(gantt, cantidad_gantt);
    }

    return m;
}

static Metricas ejecutar_prioridad_aging(const Proceso base[], int mostrar) {
    Proceso p[TOTAL_PROCESOS];
    BloqueGantt gantt[TOTAL_PROCESOS];
    copiar_procesos(p, base);

    int tiempo = 0;
    int completados = 0;
    int ocupado = 0;
    int cantidad_gantt = 0;

    while (completados < TOTAL_PROCESOS) {
        int elegido = -1;
        int mejor_prioridad = 999;

        for (int i = 0; i < TOTAL_PROCESOS; i++) {
            if (!p[i].completado && p[i].llegada <= tiempo) {
                int espera_actual = tiempo - p[i].llegada;
                int prioridad_dinamica =
                    p[i].prioridad - (espera_actual / AGING_INTERVALO);

                if (prioridad_dinamica < 1) {
                    prioridad_dinamica = 1;
                }

                if (elegido == -1 ||
                    prioridad_dinamica < mejor_prioridad ||
                    (prioridad_dinamica == mejor_prioridad &&
                     p[i].llegada < p[elegido].llegada)) {
                    elegido = i;
                    mejor_prioridad = prioridad_dinamica;
                }
            }
        }

        if (elegido == -1) {
            tiempo++;
            continue;
        }

        p[elegido].inicio = tiempo;
        p[elegido].respuesta = tiempo - p[elegido].llegada;
        p[elegido].espera = p[elegido].respuesta;
        p[elegido].fin = tiempo + p[elegido].rafaga;
        p[elegido].retorno = p[elegido].fin - p[elegido].llegada;
        p[elegido].completado = 1;

        snprintf(gantt[cantidad_gantt].nombre,
                 sizeof(gantt[cantidad_gantt].nombre),
                 "%s", p[elegido].nombre);
        gantt[cantidad_gantt].inicio = tiempo;
        gantt[cantidad_gantt].duracion = p[elegido].rafaga;
        cantidad_gantt++;

        tiempo = p[elegido].fin;
        ocupado += p[elegido].rafaga;
        completados++;
    }

    Metricas m = calcular_metricas("Prioridad + Aging", p, tiempo, ocupado);

    if (mostrar) {
        mostrar_resultados("PRIORIDADES CON AGING", p, &m);
        mostrar_gantt(gantt, cantidad_gantt);
    }

    return m;
}

static void guardar_planificacion_csv(const Metricas resultados[], int cantidad) {
    FILE *archivo = fopen("resultados/planificacion.csv", "w");

    if (archivo == NULL) {
        perror("No se pudo crear resultados/planificacion.csv");
        return;
    }

    fprintf(archivo,
            "Algoritmo,WT_promedio,TAT_promedio,RT_promedio,Throughput,Uso_CPU,Makespan\n");

    for (int i = 0; i < cantidad; i++) {
        fprintf(archivo, "%s,%.2f,%.2f,%.2f,%.6f,%.2f,%d\n",
                resultados[i].algoritmo,
                resultados[i].espera_promedio,
                resultados[i].retorno_promedio,
                resultados[i].respuesta_promedio,
                resultados[i].throughput,
                resultados[i].uso_cpu,
                resultados[i].makespan);
    }

    fclose(archivo);
}

static void modulo_planificacion(const Proceso base[]) {
    printf("\n================ MODULO 2: PLANIFICACION ================\n");

    Metricas resultados[4];
    resultados[0] = ejecutar_fcfs(base, 1);
    resultados[1] = ejecutar_sjf(base, 1);
    resultados[2] = ejecutar_round_robin(base, 1);
    resultados[3] = ejecutar_prioridad_aging(base, 1);

    printf("\n================================================================================\n");
    printf("ALGORITMO             WT PROM.   TAT PROM.  RT PROM.   THROUGHPUT   CPU\n");
    printf("================================================================================\n");

    for (int i = 0; i < 4; i++) {
        printf("%-21s %-10.2f %-10.2f %-10.2f %-12.3f %.2f%%\n",
               resultados[i].algoritmo,
               resultados[i].espera_promedio,
               resultados[i].retorno_promedio,
               resultados[i].respuesta_promedio,
               resultados[i].throughput,
               resultados[i].uso_cpu);
    }

    printf("================================================================================\n");

    double reduccion_rt =
        resultados[0].respuesta_promedio - resultados[2].respuesta_promedio;
    double mejora_rt = resultados[0].respuesta_promedio > 0.0
                           ? 100.0 * reduccion_rt /
                                 resultados[0].respuesta_promedio
                           : 0.0;

    printf("\nAnalisis:\n");
    printf("- SJF suele reducir WT y TAT, pero necesita estimar la rafaga.\n");
    printf("- Round Robin mejora la equidad y el tiempo de respuesta interactivo.\n");
    printf("- FCFS puede generar efecto convoy con procesos largos.\n");
    printf("- Aging evita que los procesos de baja prioridad esperen indefinidamente.\n");
    printf("- Mejora de RT de Round Robin frente a FCFS: %.2f%%\n", mejora_rt);

    guardar_planificacion_csv(resultados, 4);
    printf("\nArchivo generado: resultados/planificacion.csv\n");
}

/* ============================== SINCRONIZACION ============================== */

static long saldo_global = 100000;
static pthread_mutex_t mutex_saldo = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mutex_log = PTHREAD_MUTEX_INITIALIZER;
static int entradas_log = 0;

static double reloj_segundos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}

static void *depositar_sin_mutex(void *arg) {
    (void)arg;

    for (int i = 0; i < OPERACIONES_HILO; i++) {
        long temporal = saldo_global;
        sched_yield();
        saldo_global = temporal + 1;
    }

    return NULL;
}

static void *depositar_con_mutex(void *arg) {
    (void)arg;

    for (int i = 0; i < OPERACIONES_HILO; i++) {
        pthread_mutex_lock(&mutex_saldo);
        saldo_global++;
        pthread_mutex_unlock(&mutex_saldo);
    }

    return NULL;
}

static void *escribir_log_con_mutex(void *arg) {
    (void)arg;

    for (int i = 0; i < 100; i++) {
        pthread_mutex_lock(&mutex_log);
        entradas_log++;
        pthread_mutex_unlock(&mutex_log);
    }

    return NULL;
}

typedef struct {
    int id;
    long saldo;
    pthread_mutex_t mutex;
} Cuenta;

typedef struct {
    Cuenta *origen;
    Cuenta *destino;
    long monto;
} TransferenciaArgs;

static void *transferencia_segura(void *arg) {
    TransferenciaArgs *t = (TransferenciaArgs *)arg;

    Cuenta *primera =
        t->origen->id < t->destino->id ? t->origen : t->destino;
    Cuenta *segunda =
        t->origen->id < t->destino->id ? t->destino : t->origen;

    pthread_mutex_lock(&primera->mutex);
    pthread_mutex_lock(&segunda->mutex);

    if (t->origen->saldo >= t->monto) {
        t->origen->saldo -= t->monto;
        t->destino->saldo += t->monto;
    }

    pthread_mutex_unlock(&segunda->mutex);
    pthread_mutex_unlock(&primera->mutex);

    return NULL;
}

static void demostrar_deadlock(void) {
    printf("\n================ ANALISIS DE DEADLOCK ================\n");
    printf("Escenario peligroso:\n");
    printf("Hilo 1 bloquea Cuenta A y espera Cuenta B.\n");
    printf("Hilo 2 bloquea Cuenta B y espera Cuenta A.\n\n");

    printf("Condiciones de Coffman presentes:\n");
    printf("1. Exclusion mutua.\n");
    printf("2. Retencion y espera.\n");
    printf("3. No apropiacion.\n");
    printf("4. Espera circular.\n");

    Cuenta cuenta_a = {1, 1000, PTHREAD_MUTEX_INITIALIZER};
    Cuenta cuenta_b = {2, 1000, PTHREAD_MUTEX_INITIALIZER};

    TransferenciaArgs transferencia_1 = {&cuenta_a, &cuenta_b, 100};
    TransferenciaArgs transferencia_2 = {&cuenta_b, &cuenta_a, 50};

    pthread_t hilo_1;
    pthread_t hilo_2;

    pthread_create(&hilo_1, NULL, transferencia_segura, &transferencia_1);
    pthread_create(&hilo_2, NULL, transferencia_segura, &transferencia_2);

    pthread_join(hilo_1, NULL);
    pthread_join(hilo_2, NULL);

    printf("\nPrevencion aplicada:\n");
    printf("Los mutex se adquieren siempre en orden ascendente de ID.\n");
    printf("Esto elimina la espera circular.\n");
    printf("Saldo final Cuenta A: S/ %ld\n", cuenta_a.saldo);
    printf("Saldo final Cuenta B: S/ %ld\n", cuenta_b.saldo);
    printf("Deadlocks ocurridos: 0\n");

    pthread_mutex_destroy(&cuenta_a.mutex);
    pthread_mutex_destroy(&cuenta_b.mutex);
}

static void modulo_sincronizacion(void) {
    printf("\n============= MODULO 3: SINCRONIZACION =============\n");

    pthread_t hilos[NUM_HILOS];
    const long saldo_esperado =
        100000L + (long)NUM_HILOS * OPERACIONES_HILO;

    saldo_global = 100000;
    double inicio = reloj_segundos();

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_create(&hilos[i], NULL, depositar_sin_mutex, NULL);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    double tiempo_sin_mutex = reloj_segundos() - inicio;
    long saldo_sin_mutex = saldo_global;

    saldo_global = 100000;
    inicio = reloj_segundos();

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_create(&hilos[i], NULL, depositar_con_mutex, NULL);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    double tiempo_con_mutex = reloj_segundos() - inicio;
    long saldo_con_mutex = saldo_global;

    entradas_log = 0;

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_create(&hilos[i], NULL, escribir_log_con_mutex, NULL);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    printf("\nSecciones criticas identificadas:\n");
    printf("1. Actualizacion del saldo compartido.\n");
    printf("2. Escritura del log de auditoria.\n");

    printf("\nPrueba con %d hilos y %d depositos por hilo:\n",
           NUM_HILOS, OPERACIONES_HILO);
    printf("Saldo esperado:                    S/ %ld\n", saldo_esperado);
    printf("Saldo sin mutex:                   S/ %ld\n", saldo_sin_mutex);
    printf("Operaciones perdidas sin mutex:    %ld\n",
           saldo_esperado - saldo_sin_mutex);
    printf("Saldo con mutex:                   S/ %ld\n", saldo_con_mutex);
    printf("Operaciones perdidas con mutex:    %ld\n",
           saldo_esperado - saldo_con_mutex);
    printf("Entradas esperadas en el log:      %d\n", NUM_HILOS * 100);
    printf("Entradas registradas con mutex:    %d\n", entradas_log);
    printf("Tiempo sin mutex:                  %.6f s\n", tiempo_sin_mutex);
    printf("Tiempo con mutex:                  %.6f s\n", tiempo_con_mutex);
    printf("Overhead aproximado:               %.6f s\n",
           tiempo_con_mutex - tiempo_sin_mutex);

    FILE *archivo = fopen("resultados/sincronizacion.txt", "w");

    if (archivo != NULL) {
        fprintf(archivo, "MODULO 3 - SINCRONIZACION\n");
        fprintf(archivo, "Hilos: %d\n", NUM_HILOS);
        fprintf(archivo, "Operaciones por hilo: %d\n", OPERACIONES_HILO);
        fprintf(archivo, "Saldo esperado: %ld\n", saldo_esperado);
        fprintf(archivo, "Saldo sin mutex: %ld\n", saldo_sin_mutex);
        fprintf(archivo, "Perdidas sin mutex: %ld\n",
                saldo_esperado - saldo_sin_mutex);
        fprintf(archivo, "Saldo con mutex: %ld\n", saldo_con_mutex);
        fprintf(archivo, "Perdidas con mutex: %ld\n",
                saldo_esperado - saldo_con_mutex);
        fprintf(archivo, "Entradas de log: %d\n", entradas_log);
        fprintf(archivo, "Tiempo sin mutex: %.6f\n", tiempo_sin_mutex);
        fprintf(archivo, "Tiempo con mutex: %.6f\n", tiempo_con_mutex);
        fprintf(archivo, "Overhead: %.6f\n",
                tiempo_con_mutex - tiempo_sin_mutex);
        fclose(archivo);
    }

    demostrar_deadlock();
    printf("\nArchivo generado: resultados/sincronizacion.txt\n");
}

/* ============================== MEMORIA ============================== */

static void memoria_inicializar(Memoria *m) {
    m->cantidad = 1;
    m->denegadas = 0;
    m->bloques[0].inicio = 0;
    m->bloques[0].tamanio = MEMORIA_TOTAL;
    m->bloques[0].libre = 1;
    snprintf(m->bloques[0].proceso,
             sizeof(m->bloques[0].proceso), "-");
}

static int memoria_asignar(Memoria *m, const char *nombre, int tamanio,
                           Estrategia estrategia) {
    int elegido = -1;

    for (int i = 0; i < m->cantidad; i++) {
        if (m->bloques[i].libre && m->bloques[i].tamanio >= tamanio) {
            if (estrategia == FIRST_FIT) {
                elegido = i;
                break;
            }

            if (elegido == -1 ||
                m->bloques[i].tamanio < m->bloques[elegido].tamanio) {
                elegido = i;
            }
        }
    }

    if (elegido == -1) {
        m->denegadas++;
        return 0;
    }

    Particion original = m->bloques[elegido];

    if (original.tamanio > tamanio && m->cantidad < MAX_PARTICIONES) {
        for (int i = m->cantidad; i > elegido + 1; i--) {
            m->bloques[i] = m->bloques[i - 1];
        }

        m->bloques[elegido + 1].inicio = original.inicio + tamanio;
        m->bloques[elegido + 1].tamanio =
            original.tamanio - tamanio;
        m->bloques[elegido + 1].libre = 1;
        snprintf(m->bloques[elegido + 1].proceso,
                 sizeof(m->bloques[elegido + 1].proceso), "-");
        m->cantidad++;
    }

    m->bloques[elegido].inicio = original.inicio;
    m->bloques[elegido].tamanio = tamanio;
    m->bloques[elegido].libre = 0;
    snprintf(m->bloques[elegido].proceso,
             sizeof(m->bloques[elegido].proceso), "%s", nombre);

    return 1;
}

static void memoria_fusionar(Memoria *m) {
    int i = 0;

    while (i < m->cantidad - 1) {
        if (m->bloques[i].libre && m->bloques[i + 1].libre) {
            m->bloques[i].tamanio += m->bloques[i + 1].tamanio;

            for (int j = i + 1; j < m->cantidad - 1; j++) {
                m->bloques[j] = m->bloques[j + 1];
            }

            m->cantidad--;
        } else {
            i++;
        }
    }
}

static int memoria_liberar(Memoria *m, const char *nombre) {
    for (int i = 0; i < m->cantidad; i++) {
        if (!m->bloques[i].libre &&
            strcmp(m->bloques[i].proceso, nombre) == 0) {
            m->bloques[i].libre = 1;
            snprintf(m->bloques[i].proceso,
                     sizeof(m->bloques[i].proceso), "-");
            memoria_fusionar(m);
            return 1;
        }
    }

    return 0;
}

static void memoria_mostrar(const Memoria *m, FILE *salida) {
    fprintf(salida, "\n============================================================\n");
    fprintf(salida, "MAPA DE PARTICIONES\n");
    fprintf(salida, "============================================================\n");
    fprintf(salida, "INICIO   TAMANIO   ESTADO      PROCESO\n");

    for (int i = 0; i < m->cantidad; i++) {
        fprintf(salida, "%-8d %-9d %-11s %s\n",
                m->bloques[i].inicio,
                m->bloques[i].tamanio,
                m->bloques[i].libre ? "Libre" : "Ocupado",
                m->bloques[i].proceso);
    }
}

static double fragmentacion_externa(const Memoria *m) {
    int memoria_libre = 0;
    int mayor_hueco = 0;

    for (int i = 0; i < m->cantidad; i++) {
        if (m->bloques[i].libre) {
            memoria_libre += m->bloques[i].tamanio;

            if (m->bloques[i].tamanio > mayor_hueco) {
                mayor_hueco = m->bloques[i].tamanio;
            }
        }
    }

    if (memoria_libre == 0) {
        return 0.0;
    }

    return 100.0 *
           (1.0 - (double)mayor_hueco / memoria_libre);
}

static int contar_huecos_libres(const Memoria *m) {
    int huecos = 0;

    for (int i = 0; i < m->cantidad; i++) {
        if (m->bloques[i].libre) {
            huecos++;
        }
    }

    return huecos;
}

static void ejecutar_prueba_memoria(Estrategia estrategia, int mostrar,
                                    double *fragmentacion_final,
                                    int *huecos_finales,
                                    int *denegadas_finales) {
    Memoria memoria;
    memoria_inicializar(&memoria);

    memoria_asignar(&memoria, "Login1", 96, estrategia);              /* 1 */
    memoria_asignar(&memoria, "Consulta1", 64, estrategia);           /* 2 */
    memoria_asignar(&memoria, "Transferencia1", 256, estrategia);     /* 3 */
    memoria_asignar(&memoria, "Pago1", 192, estrategia);              /* 4 */
    memoria_asignar(&memoria, "ActualizaBD", 128, estrategia);        /* 5 */
    memoria_liberar(&memoria, "Consulta1");                            /* 6 */
    memoria_asignar(&memoria, "Consulta2", 64, estrategia);           /* 7 */
    memoria_asignar(&memoria, "Transferencia2", 256, estrategia);     /* 8 */
    memoria_liberar(&memoria, "Pago1");                                /* 9 */
    memoria_asignar(&memoria, "Liquidacion", 512, estrategia);        /* 10 */

    if (mostrar) {
        printf("\nEstado intermedio despues de 10 operaciones:\n");
        memoria_mostrar(&memoria, stdout);
    }

    memoria_asignar(&memoria, "Pago2", 192, estrategia);              /* 11 */
    memoria_liberar(&memoria, "Login1");                               /* 12 */
    memoria_asignar(&memoria, "Login2", 96, estrategia);              /* 13 */
    memoria_liberar(&memoria, "ActualizaBD");                          /* 14 */
    memoria_asignar(&memoria, "Consulta3", 64, estrategia);           /* 15 */

    *fragmentacion_final = fragmentacion_externa(&memoria);
    *huecos_finales = contar_huecos_libres(&memoria);
    *denegadas_finales = memoria.denegadas;

    if (mostrar) {
        printf("\nEstado final despues de 15 operaciones:\n");
        memoria_mostrar(&memoria, stdout);
        printf("Fragmentacion externa: %.2f%%\n", *fragmentacion_final);
        printf("Huecos libres: %d\n", *huecos_finales);
        printf("Asignaciones denegadas: %d\n", *denegadas_finales);
    }
}

static void modulo_memoria(void) {
    printf("\n================ MODULO 4: MEMORIA ================\n");

    double fragmentacion_first = 0.0;
    double fragmentacion_best = 0.0;
    int huecos_first = 0;
    int huecos_best = 0;
    int denegadas_first = 0;
    int denegadas_best = 0;

    printf("\n--- FIRST-FIT ---\n");
    ejecutar_prueba_memoria(FIRST_FIT, 1,
                            &fragmentacion_first,
                            &huecos_first,
                            &denegadas_first);

    printf("\n--- BEST-FIT ---\n");
    ejecutar_prueba_memoria(BEST_FIT, 1,
                            &fragmentacion_best,
                            &huecos_best,
                            &denegadas_best);

    printf("\n==============================================================\n");
    printf("ESTRATEGIA   FRAGMENTACION   HUECOS LIBRES   DENEGADAS\n");
    printf("==============================================================\n");
    printf("First-Fit    %-15.2f %-15d %d\n",
           fragmentacion_first, huecos_first, denegadas_first);
    printf("Best-Fit     %-15.2f %-15d %d\n",
           fragmentacion_best, huecos_best, denegadas_best);
    printf("==============================================================\n");

    if (fragmentacion_first < fragmentacion_best) {
        printf("\nPara esta carga, First-Fit produjo menor fragmentacion.\n");
    } else if (fragmentacion_best < fragmentacion_first) {
        printf("\nPara esta carga, Best-Fit produjo menor fragmentacion.\n");
    } else {
        printf("\nPara esta carga, ambas estrategias produjeron la misma fragmentacion.\n");
    }

    FILE *archivo = fopen("resultados/memoria.csv", "w");

    if (archivo != NULL) {
        fprintf(archivo,
                "Estrategia,Fragmentacion_externa,Huecos_libres,Asignaciones_denegadas\n");
        fprintf(archivo, "First-Fit,%.2f,%d,%d\n",
                fragmentacion_first, huecos_first, denegadas_first);
        fprintf(archivo, "Best-Fit,%.2f,%d,%d\n",
                fragmentacion_best, huecos_best, denegadas_best);
        fclose(archivo);
    }

    printf("Archivo generado: resultados/memoria.csv\n");
}

/* ============================== INTEGRACION ============================== */

static void modulo_integrado(const Proceso base[]) {
    printf("\n================ MODULO 5: INTEGRACION ================\n");

    Memoria memoria;
    memoria_inicializar(&memoria);

    int aceptados = 0;
    int rechazados = 0;
    int memoria_actual = 0;
    int memoria_maxima = 0;

    printf("\n1. Asignacion de memoria mediante Best-Fit:\n");

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        if (memoria_asignar(&memoria,
                            base[i].nombre,
                            base[i].memoria,
                            BEST_FIT)) {
            aceptados++;
            memoria_actual += base[i].memoria;

            if (memoria_actual > memoria_maxima) {
                memoria_maxima = memoria_actual;
            }

            printf("[OK] %-20s %d MB\n",
                   base[i].nombre, base[i].memoria);
        } else {
            rechazados++;
            printf("[DENEGADO] %-20s %d MB\n",
                   base[i].nombre, base[i].memoria);
        }
    }

    printf("\n2. Planificacion de los procesos con Round Robin:\n");
    Metricas round_robin = ejecutar_round_robin(base, 0);
    Metricas fcfs = ejecutar_fcfs(base, 0);

    printf("\n3. Acceso sincronizado al saldo mediante mutex:\n");

    pthread_t hilos[NUM_HILOS];
    saldo_global = 100000;

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_create(&hilos[i], NULL, depositar_con_mutex, NULL);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(hilos[i], NULL);
    }

    long saldo_esperado =
        100000L + (long)NUM_HILOS * OPERACIONES_HILO;
    long conflictos = saldo_esperado - saldo_global;

    double mejora_respuesta =
        fcfs.respuesta_promedio > 0.0
            ? 100.0 *
                  (fcfs.respuesta_promedio -
                   round_robin.respuesta_promedio) /
                  fcfs.respuesta_promedio
            : 0.0;

    printf("\n================ METRICAS DEL SISTEMA INTEGRADO ================\n");
    printf("Procesos recibidos:                  %d\n", TOTAL_PROCESOS);
    printf("Procesos con memoria asignada:       %d\n", aceptados);
    printf("Procesos rechazados:                 %d\n", rechazados);
    printf("Throughput total:                    %.3f procesos/u.t.\n",
           round_robin.throughput);
    printf("Tiempo de respuesta promedio:        %.2f\n",
           round_robin.respuesta_promedio);
    printf("Uso de CPU:                          %.2f%%\n",
           round_robin.uso_cpu);
    printf("Uso maximo de memoria:               %d MB de %d MB\n",
           memoria_maxima, MEMORIA_TOTAL);
    printf("Fragmentacion externa final:         %.2f%%\n",
           fragmentacion_externa(&memoria));
    printf("Conflictos con mutex:                %ld\n", conflictos);
    printf("Deadlocks ocurridos:                 0\n");
    printf("Mejora de RT RR frente a FCFS:       %.2f%%\n",
           mejora_respuesta);

    printf("\nCuellos de botella:\n");
    printf("1. La liquidacion nocturna incrementa la espera en FCFS.\n");
    printf("2. Un mutex global limita el paralelismo de cuentas distintas.\n");

    printf("\nPlan de optimizacion:\n");
    printf("- Usar Round Robin para operaciones interactivas.\n");
    printf("- Ejecutar Liquidacion en una cola batch de baja prioridad.\n");
    printf("- Utilizar un mutex por cuenta en lugar de uno global.\n");
    printf("- Beneficio medido en RT: %.2f%% frente a FCFS.\n",
           mejora_respuesta);

    FILE *archivo = fopen("resultados/integracion.txt", "w");

    if (archivo != NULL) {
        fprintf(archivo, "MODULO 5 - SISTEMA INTEGRADO\n");
        fprintf(archivo, "Procesos recibidos: %d\n", TOTAL_PROCESOS);
        fprintf(archivo, "Procesos aceptados: %d\n", aceptados);
        fprintf(archivo, "Procesos rechazados: %d\n", rechazados);
        fprintf(archivo, "Throughput: %.6f\n",
                round_robin.throughput);
        fprintf(archivo, "RT promedio: %.2f\n",
                round_robin.respuesta_promedio);
        fprintf(archivo, "Uso CPU: %.2f\n",
                round_robin.uso_cpu);
        fprintf(archivo, "Uso maximo memoria: %d MB\n",
                memoria_maxima);
        fprintf(archivo, "Fragmentacion externa: %.2f\n",
                fragmentacion_externa(&memoria));
        fprintf(archivo, "Conflictos: %ld\n", conflictos);
        fprintf(archivo, "Deadlocks: 0\n");
        fprintf(archivo, "Mejora RT RR vs FCFS: %.2f%%\n",
                mejora_respuesta);
        fclose(archivo);
    }

    printf("\nArchivo generado: resultados/integracion.txt\n");
}

static void demostracion_completa(const Proceso base[]) {
    mostrar_carga(base);
    modulo_planificacion(base);
    modulo_sincronizacion();
    modulo_memoria();
    modulo_integrado(base);
}

/* ============================== MENU ============================== */

static void menu_principal(void) {
    Proceso procesos[TOTAL_PROCESOS];
    int opcion = -1;

    cargar_procesos(procesos);
    crear_directorio_resultados();

    do {
        encabezado();
        printf("1. Mostrar carga de procesos\n");
        printf("2. Ejecutar planificacion de CPU\n");
        printf("3. Ejecutar sincronizacion y concurrencia\n");
        printf("4. Ejecutar gestion de memoria\n");
        printf("5. Ejecutar sistema integrado\n");
        printf("6. Ejecutar demostracion completa\n");
        printf("0. Salir\n");
        printf("\nSeleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiar_entrada();
            opcion = -1;
        }

        switch (opcion) {
            case 1:
                mostrar_carga(procesos);
                pausa();
                break;

            case 2:
                modulo_planificacion(procesos);
                pausa();
                break;

            case 3:
                modulo_sincronizacion();
                pausa();
                break;

            case 4:
                modulo_memoria();
                pausa();
                break;

            case 5:
                modulo_integrado(procesos);
                pausa();
                break;

            case 6:
                demostracion_completa(procesos);
                pausa();
                break;

            case 0:
                printf("\nPrograma finalizado correctamente.\n");
                break;

            default:
                printf("\nOpcion no valida.\n");
                pausa();
                break;
        }
    } while (opcion != 0);
}

int main(void) {
    menu_principal();
    return 0;
}
