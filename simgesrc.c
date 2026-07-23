/*
===============================================================================
SIMGESRC - SISTEMA DE GESTION DE RECURSOS COMPUTACIONALES
Escenario: Plataforma Bancaria Digital de Caja Huancayo
Lenguaje: C11
Sistema objetivo: Ubuntu Desktop 24.04 LTS

Contenidos aplicados:
- FCFS, SJF no apropiativo, Round Robin y Prioridades con Aging.
- Seccion critica, condicion de carrera, mutex y prevencion de deadlock.
- Particiones variables, First-Fit, Best-Fit y fragmentacion externa.
- Integracion de operaciones bancarias, planificacion, concurrencia y memoria.

Compilacion:
gcc -std=c11 -Wall -Wextra -O2 simgesrc.c -o simgesrc -pthread

Ejecucion:
./simgesrc
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
#define TOTAL_CUENTAS 4

#define QUANTUM 4
#define AGING_INTERVALO 5

#define NUM_HILOS 8
#define OPERACIONES_HILO 2000

#define MEMORIA_TOTAL 2048
#define MAX_PARTICIONES 64
#define MAX_GANTT 256

#define LONGITUD_NOMBRE 64
#define LONGITUD_USUARIO 32
#define LONGITUD_CLAVE 32

typedef struct {
    int id;
    char titular[LONGITUD_NOMBRE];
    char usuario[LONGITUD_USUARIO];
    char clave[LONGITUD_CLAVE];
    long saldo_centimos;
    pthread_mutex_t mutex;
} CuentaBancaria;

typedef struct {
    int pid;
    char nombre[32];
    char funcion[80];

    int llegada;
    int rafaga;
    int prioridad;
    int memoria;
    int tipo;

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

typedef struct {
    CuentaBancaria *origen;
    CuentaBancaria *destino;
    long monto_centimos;
    int realizada;
} TransferenciaArgs;

/* ============================================================================
   VARIABLES GLOBALES
   ========================================================================== */

static CuentaBancaria cuentas[TOTAL_CUENTAS];

static long saldo_global = 100000;

static pthread_mutex_t mutex_saldo =
    PTHREAD_MUTEX_INITIALIZER;

static pthread_mutex_t mutex_log =
    PTHREAD_MUTEX_INITIALIZER;

static int entradas_log = 0;

/* ============================================================================
   PROTOTIPOS: UTILIDADES
   ========================================================================== */

static void limpiar_entrada(void);
static void pausa(void);
static void encabezado(void);
static void crear_directorio_resultados(void);

static int leer_entero(
    const char *mensaje,
    int minimo,
    int maximo
);

static long leer_monto_centimos(
    const char *mensaje
);

static void imprimir_monto(
    long centimos
);

/* ============================================================================
   PROTOTIPOS: MODULO BANCARIO
   ========================================================================== */

static void inicializar_cuentas(void);
static void destruir_cuentas(void);

static int buscar_cuenta_por_id(
    int id
);

static int autenticar_cuenta(void);

static void listar_cuentas(void);
static void consultar_saldo(void);
static void depositar(void);
static void retirar(void);
static void transferir(void);
static void pagar_servicio(void);
static void modulo_bancario(void);

/* ============================================================================
   PROTOTIPOS: PROCESOS
   ========================================================================== */

static void cargar_procesos(
    Proceso procesos[]
);

static void copiar_procesos(
    Proceso destino[],
    const Proceso origen[]
);

static void mostrar_carga(
    const Proceso procesos[]
);

/* ============================================================================
   PROTOTIPOS: PLANIFICACION
   ========================================================================== */

static Metricas calcular_metricas(
    const char *nombre,
    const Proceso procesos[],
    int makespan,
    int ocupado
);

static void mostrar_resultados(
    const char *algoritmo,
    const Proceso procesos[],
    const Metricas *metricas
);

static void mostrar_gantt(
    const BloqueGantt gantt[],
    int cantidad
);

static Metricas ejecutar_fcfs(
    const Proceso base[],
    int mostrar
);

static Metricas ejecutar_sjf(
    const Proceso base[],
    int mostrar
);

static Metricas ejecutar_round_robin(
    const Proceso base[],
    int mostrar
);

static Metricas ejecutar_prioridad_aging(
    const Proceso base[],
    int mostrar
);

static void guardar_planificacion_csv(
    const Metricas resultados[],
    int cantidad
);

static void modulo_planificacion(
    const Proceso base[]
);

/* ============================================================================
   PROTOTIPOS: SINCRONIZACION
   ========================================================================== */

static double reloj_segundos(void);

static void *depositar_sin_mutex(
    void *arg
);

static void *depositar_con_mutex(
    void *arg
);

static void *escribir_log_con_mutex(
    void *arg
);

static void *transferencia_segura(
    void *arg
);

static void demostrar_deadlock(void);
static void modulo_sincronizacion(void);

/* ============================================================================
   PROTOTIPOS: MEMORIA
   ========================================================================== */

static void memoria_inicializar(
    Memoria *memoria
);

static int memoria_asignar(
    Memoria *memoria,
    const char *nombre,
    int tamanio,
    Estrategia estrategia
);

static int memoria_liberar(
    Memoria *memoria,
    const char *nombre
);

static void memoria_fusionar(
    Memoria *memoria
);

static void memoria_mostrar(
    const Memoria *memoria,
    FILE *salida
);

static double fragmentacion_externa(
    const Memoria *memoria
);

static int contar_huecos_libres(
    const Memoria *memoria
);

static void ejecutar_prueba_memoria(
    Estrategia estrategia,
    int mostrar,
    double *fragmentacion_final,
    int *huecos_finales,
    int *denegadas_finales
);

static void modulo_memoria(void);

/* ============================================================================
   PROTOTIPOS: INTEGRACION Y MENU
   ========================================================================== */

static void modulo_integrado(
    const Proceso base[]
);

static void demostracion_completa(
    const Proceso base[]
);

static void menu_principal(void);

/* ============================================================================
   UTILIDADES
   ========================================================================== */

static void limpiar_entrada(void) {
    int caracter;

    while (
        (caracter = getchar()) != '\n' &&
        caracter != EOF
    ) {
    }
}

static void pausa(void) {
    printf("\nPresione ENTER para continuar...");
    fflush(stdout);
    getchar();
}

static void encabezado(void) {
    printf("\n");
    printf("============================================================\n");
    printf(" SIMGESRC - PLATAFORMA BANCARIA DIGITAL CAJA HUANCAYO\n");
    printf(" Sistema de Gestion de Recursos Computacionales\n");
    printf("============================================================\n");
}

static void crear_directorio_resultados(void) {
    int resultado;

    resultado = system("mkdir -p resultados");

    if (resultado == -1) {
        fprintf(
            stderr,
            "Advertencia: no se pudo crear el directorio resultados.\n"
        );
    }
}

static int leer_entero(
    const char *mensaje,
    int minimo,
    int maximo
) {
    int valor;
    int resultado;

    for (;;) {
        printf("%s", mensaje);
        fflush(stdout);

        resultado = scanf("%d", &valor);
        limpiar_entrada();

        if (
            resultado == 1 &&
            valor >= minimo &&
            valor <= maximo
        ) {
            return valor;
        }

        printf(
            "Entrada no valida. Ingrese un numero entre %d y %d.\n",
            minimo,
            maximo
        );
    }
}

static long leer_monto_centimos(
    const char *mensaje
) {
    double monto;
    int resultado;

    for (;;) {
        printf("%s", mensaje);
        fflush(stdout);

        resultado = scanf("%lf", &monto);
        limpiar_entrada();

        if (
            resultado == 1 &&
            monto > 0.0 &&
            monto <= 1000000.0
        ) {
            return (long)(monto * 100.0 + 0.5);
        }

        printf(
            "Monto no valido. Ingrese un valor numerico positivo.\n"
        );
    }
}

static void imprimir_monto(
    long centimos
) {
    long parte_entera;
    long parte_decimal;

    parte_entera = centimos / 100;
    parte_decimal = labs(centimos % 100);

    printf(
        "S/ %ld.%02ld",
        parte_entera,
        parte_decimal
    );
}

/* ============================================================================
   MODULO BANCARIO
   ========================================================================== */

static void inicializar_cuentas(void) {
    cuentas[0].id = 1001;

    snprintf(
        cuentas[0].titular,
        sizeof(cuentas[0].titular),
        "%s",
        "Juan Perez"
    );

    snprintf(
        cuentas[0].usuario,
        sizeof(cuentas[0].usuario),
        "%s",
        "juan"
    );

    snprintf(
        cuentas[0].clave,
        sizeof(cuentas[0].clave),
        "%s",
        "1234"
    );

    cuentas[0].saldo_centimos = 250000;

    cuentas[1].id = 1002;

    snprintf(
        cuentas[1].titular,
        sizeof(cuentas[1].titular),
        "%s",
        "Maria Lopez"
    );

    snprintf(
        cuentas[1].usuario,
        sizeof(cuentas[1].usuario),
        "%s",
        "maria"
    );

    snprintf(
        cuentas[1].clave,
        sizeof(cuentas[1].clave),
        "%s",
        "1234"
    );

    cuentas[1].saldo_centimos = 180000;

    cuentas[2].id = 1003;

    snprintf(
        cuentas[2].titular,
        sizeof(cuentas[2].titular),
        "%s",
        "Carlos Ruiz"
    );

    snprintf(
        cuentas[2].usuario,
        sizeof(cuentas[2].usuario),
        "%s",
        "carlos"
    );

    snprintf(
        cuentas[2].clave,
        sizeof(cuentas[2].clave),
        "%s",
        "1234"
    );

    cuentas[2].saldo_centimos = 500000;

    cuentas[3].id = 1004;

    snprintf(
        cuentas[3].titular,
        sizeof(cuentas[3].titular),
        "%s",
        "Ana Torres"
    );

    snprintf(
        cuentas[3].usuario,
        sizeof(cuentas[3].usuario),
        "%s",
        "ana"
    );

    snprintf(
        cuentas[3].clave,
        sizeof(cuentas[3].clave),
        "%s",
        "1234"
    );

    cuentas[3].saldo_centimos = 320000;

    for (int i = 0; i < TOTAL_CUENTAS; i++) {
        if (
            pthread_mutex_init(
                &cuentas[i].mutex,
                NULL
            ) != 0
        ) {
            fprintf(
                stderr,
                "Error al inicializar el mutex de la cuenta %d.\n",
                cuentas[i].id
            );

            exit(EXIT_FAILURE);
        }
    }
}

static void destruir_cuentas(void) {
    for (int i = 0; i < TOTAL_CUENTAS; i++) {
        pthread_mutex_destroy(
            &cuentas[i].mutex
        );
    }
}

static int buscar_cuenta_por_id(
    int id
) {
    for (int i = 0; i < TOTAL_CUENTAS; i++) {
        if (cuentas[i].id == id) {
            return i;
        }
    }

    return -1;
}

static int autenticar_cuenta(void) {
    char usuario[LONGITUD_USUARIO];
    char clave[LONGITUD_CLAVE];

    printf("Usuario: ");

    if (
        scanf(
            "%31s",
            usuario
        ) != 1
    ) {
        limpiar_entrada();

        printf(
            "No se pudo leer el usuario.\n"
        );

        return -1;
    }

    printf("Clave: ");

    if (
        scanf(
            "%31s",
            clave
        ) != 1
    ) {
        limpiar_entrada();

        printf(
            "No se pudo leer la clave.\n"
        );

        return -1;
    }

    limpiar_entrada();

    for (int i = 0; i < TOTAL_CUENTAS; i++) {
        if (
            strcmp(
                cuentas[i].usuario,
                usuario
            ) == 0 &&
            strcmp(
                cuentas[i].clave,
                clave
            ) == 0
        ) {
            printf(
                "Autenticacion correcta. Bienvenido, %s.\n",
                cuentas[i].titular
            );

            return i;
        }
    }

    printf(
        "Credenciales incorrectas.\n"
    );

    return -1;
}

static void listar_cuentas(void) {
    printf("\n");
    printf("============================================================\n");
    printf(" CUENTAS BANCARIAS DE PRUEBA\n");
    printf("============================================================\n");
    printf("CUENTA   TITULAR                 USUARIO       SALDO\n");
    printf("------------------------------------------------------------\n");

    for (int i = 0; i < TOTAL_CUENTAS; i++) {
        printf(
            "%-8d %-23s %-13s ",
            cuentas[i].id,
            cuentas[i].titular,
            cuentas[i].usuario
        );

        imprimir_monto(
            cuentas[i].saldo_centimos
        );

        printf("\n");
    }

    printf("------------------------------------------------------------\n");
    printf("Clave academica para todas las cuentas: 1234\n");
}

static void consultar_saldo(void) {
    int indice;
    long saldo;

    printf("\n");
    printf("================ CONSULTA DE SALDO ================\n");

    indice = autenticar_cuenta();

    if (indice < 0) {
        return;
    }

    pthread_mutex_lock(
        &cuentas[indice].mutex
    );

    saldo = cuentas[indice].saldo_centimos;

    pthread_mutex_unlock(
        &cuentas[indice].mutex
    );

    printf(
        "\nCuenta: %d\n",
        cuentas[indice].id
    );

    printf(
        "Titular: %s\n",
        cuentas[indice].titular
    );

    printf(
        "Saldo disponible: "
    );

    imprimir_monto(saldo);

    printf("\n");
}

static void depositar(void) {
    int numero_cuenta;
    int indice;

    long monto;
    long saldo_anterior;
    long saldo_nuevo;

    printf("\n");
    printf("================ DEPOSITO BANCARIO ================\n");

    numero_cuenta = leer_entero(
        "Numero de cuenta: ",
        1001,
        9999
    );

    indice = buscar_cuenta_por_id(
        numero_cuenta
    );

    if (indice < 0) {
        printf(
            "La cuenta ingresada no existe.\n"
        );

        return;
    }

    monto = leer_monto_centimos(
        "Monto a depositar: S/ "
    );

    pthread_mutex_lock(
        &cuentas[indice].mutex
    );

    saldo_anterior =
        cuentas[indice].saldo_centimos;

    cuentas[indice].saldo_centimos +=
        monto;

    saldo_nuevo =
        cuentas[indice].saldo_centimos;

    pthread_mutex_unlock(
        &cuentas[indice].mutex
    );

    printf("\nDeposito realizado correctamente.\n");

    printf("Titular: %s\n",
           cuentas[indice].titular);

    printf("Saldo anterior: ");
    imprimir_monto(saldo_anterior);

    printf("\nMonto depositado: ");
    imprimir_monto(monto);

    printf("\nSaldo nuevo: ");
    imprimir_monto(saldo_nuevo);

    printf("\n");
}

static void retirar(void) {
    int indice;

    long monto;
    long saldo_nuevo;

    printf("\n");
    printf("================ RETIRO BANCARIO ================\n");

    indice = autenticar_cuenta();

    if (indice < 0) {
        return;
    }

    monto = leer_monto_centimos(
        "Monto a retirar: S/ "
    );

    pthread_mutex_lock(
        &cuentas[indice].mutex
    );

    if (
        cuentas[indice].saldo_centimos <
        monto
    ) {
        printf(
            "Operacion rechazada: fondos insuficientes.\n"
        );

        pthread_mutex_unlock(
            &cuentas[indice].mutex
        );

        return;
    }

    cuentas[indice].saldo_centimos -=
        monto;

    saldo_nuevo =
        cuentas[indice].saldo_centimos;

    pthread_mutex_unlock(
        &cuentas[indice].mutex
    );

    printf(
        "\nRetiro realizado correctamente.\n"
    );

    printf(
        "Monto retirado: "
    );

    imprimir_monto(monto);

    printf(
        "\nSaldo disponible: "
    );

    imprimir_monto(saldo_nuevo);

    printf("\n");
}

static void transferir(void) {
    int indice_origen;
    int numero_destino;
    int indice_destino;

    long monto;

    TransferenciaArgs argumentos;

    printf("\n");
    printf("================ TRANSFERENCIA BANCARIA ================\n");

    indice_origen =
        autenticar_cuenta();

    if (indice_origen < 0) {
        return;
    }

    numero_destino = leer_entero(
        "Numero de cuenta de destino: ",
        1001,
        9999
    );

    indice_destino =
        buscar_cuenta_por_id(
            numero_destino
        );

    if (indice_destino < 0) {
        printf(
            "La cuenta de destino no existe.\n"
        );

        return;
    }

    if (
        indice_origen ==
        indice_destino
    ) {
        printf(
            "La cuenta de destino debe ser diferente de la cuenta de origen.\n"
        );

        return;
    }

    monto = leer_monto_centimos(
        "Monto a transferir: S/ "
    );

    argumentos.origen =
        &cuentas[indice_origen];

    argumentos.destino =
        &cuentas[indice_destino];

    argumentos.monto_centimos =
        monto;

    argumentos.realizada = 0;

    transferencia_segura(
        &argumentos
    );

    if (argumentos.realizada) {
        printf(
            "\nTransferencia realizada correctamente.\n"
        );

        printf(
            "Cuenta origen: %d - %s\n",
            cuentas[indice_origen].id,
            cuentas[indice_origen].titular
        );

        printf(
            "Cuenta destino: %d - %s\n",
            cuentas[indice_destino].id,
            cuentas[indice_destino].titular
        );

        printf(
            "Monto transferido: "
        );

        imprimir_monto(monto);

        printf(
            "\nSaldo disponible: "
        );

        imprimir_monto(
            cuentas[indice_origen].saldo_centimos
        );

        printf("\n");
    } else {
        printf(
            "Transferencia rechazada por fondos insuficientes.\n"
        );
    }
}

static void pagar_servicio(void) {
    int indice;
    int servicio;

    long monto;
    long saldo_final;

    const char *nombres_servicios[] = {
        "",
        "Energia electrica",
        "Agua potable",
        "Internet",
        "Telefonia"
    };

    printf("\n");
    printf("================ PAGO DE SERVICIO ================\n");

    indice = autenticar_cuenta();

    if (indice < 0) {
        return;
    }

    printf("\n");
    printf("1. Energia electrica\n");
    printf("2. Agua potable\n");
    printf("3. Internet\n");
    printf("4. Telefonia\n");

    servicio = leer_entero(
        "Seleccione el servicio: ",
        1,
        4
    );

    monto = leer_monto_centimos(
        "Monto del pago: S/ "
    );

    pthread_mutex_lock(
        &cuentas[indice].mutex
    );

    if (
        cuentas[indice].saldo_centimos <
        monto
    ) {
        printf(
            "Fondos insuficientes para procesar el pago.\n"
        );

        pthread_mutex_unlock(
            &cuentas[indice].mutex
        );

        return;
    }

    cuentas[indice].saldo_centimos -=
        monto;

    saldo_final =
        cuentas[indice].saldo_centimos;

    pthread_mutex_unlock(
        &cuentas[indice].mutex
    );

    printf(
        "\nPago procesado correctamente.\n"
    );

    printf(
        "Servicio: %s\n",
        nombres_servicios[servicio]
    );

    printf(
        "Monto pagado: "
    );

    imprimir_monto(monto);

    printf(
        "\nSaldo disponible: "
    );

    imprimir_monto(saldo_final);

    printf("\n");
}

static void modulo_bancario(void) {
    int opcion;

    do {
        printf("\n");
        printf("============================================================\n");
        printf(" MODULO DE OPERACIONES BANCARIAS\n");
        printf("============================================================\n");
        printf("1. Listar cuentas de prueba\n");
        printf("2. Consultar saldo\n");
        printf("3. Realizar deposito\n");
        printf("4. Realizar retiro\n");
        printf("5. Realizar transferencia\n");
        printf("6. Pagar servicio\n");
        printf("0. Volver al menu principal\n");

        opcion = leer_entero(
            "\nSeleccione una opcion: ",
            0,
            6
        );

        switch (opcion) {
            case 1:
                listar_cuentas();
                pausa();
                break;

            case 2:
                consultar_saldo();
                pausa();
                break;

            case 3:
                depositar();
                pausa();
                break;

            case 4:
                retirar();
                pausa();
                break;

            case 5:
                transferir();
                pausa();
                break;

            case 6:
                pagar_servicio();
                pausa();
                break;

            case 0:
                break;

            default:
                break;
        }
    } while (opcion != 0);
}

/* ============================================================================
   CARGA DE PROCESOS
   ========================================================================== */

static void cargar_procesos(
    Proceso procesos[]
) {
    Proceso carga[TOTAL_PROCESOS] = {
        {
            1,
            "Consulta1",
            "Consultar el saldo de una cuenta",
            0,
            2,
            2,
            64,
            0,
            2,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            2,
            "Consulta2",
            "Consultar el saldo de una cuenta",
            1,
            2,
            2,
            64,
            0,
            2,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            3,
            "Login1",
            "Validar credenciales del cliente",
            2,
            3,
            1,
            96,
            0,
            3,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            4,
            "Transferencia1",
            "Transferir dinero entre dos cuentas",
            3,
            8,
            1,
            256,
            0,
            8,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            5,
            "Pago1",
            "Procesar el pago de un servicio",
            4,
            6,
            2,
            192,
            0,
            6,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            6,
            "Consulta3",
            "Consultar el saldo de una cuenta",
            5,
            2,
            2,
            64,
            0,
            2,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            7,
            "ActualizaBD",
            "Actualizar registros de la base de datos",
            6,
            4,
            3,
            128,
            0,
            4,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            8,
            "Transferencia2",
            "Transferir dinero entre dos cuentas",
            7,
            8,
            1,
            256,
            0,
            8,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            9,
            "Pago2",
            "Procesar el pago de un servicio",
            8,
            6,
            2,
            192,
            0,
            6,
            -1,
            0,
            0,
            0,
            0,
            0
        },
        {
            10,
            "Liquidacion",
            "Ejecutar la liquidacion nocturna por lotes",
            10,
            12,
            5,
            512,
            1,
            12,
            -1,
            0,
            0,
            0,
            0,
            0
        }
    };

    memcpy(
        procesos,
        carga,
        sizeof(carga)
    );
}

static void copiar_procesos(
    Proceso destino[],
    const Proceso origen[]
) {
    memcpy(
        destino,
        origen,
        sizeof(Proceso) * TOTAL_PROCESOS
    );
}

static void mostrar_carga(
    const Proceso procesos[]
) {
    printf("\n");
    printf("====================================================================================================\n");
    printf("PID  PROCESO              LLEGADA  RAFAGA  PRIORIDAD  MEMORIA  TIPO        FUNCION\n");
    printf("====================================================================================================\n");

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        printf(
            "%-4d %-20s %-8d %-7d %-10d %-8d %-11s %s\n",
            procesos[i].pid,
            procesos[i].nombre,
            procesos[i].llegada,
            procesos[i].rafaga,
            procesos[i].prioridad,
            procesos[i].memoria,
            procesos[i].tipo == 0
                ? "I/O-bound"
                : "CPU-bound",
            procesos[i].funcion
        );
    }

    printf("====================================================================================================\n");
}

/* ============================================================================
   MODULO DE PLANIFICACION DE CPU
   ========================================================================== */

static Metricas calcular_metricas(
    const char *nombre,
    const Proceso procesos[],
    int makespan,
    int ocupado
) {
    Metricas metricas = {0};

    snprintf(
        metricas.algoritmo,
        sizeof(metricas.algoritmo),
        "%s",
        nombre
    );

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        metricas.espera_promedio +=
            procesos[i].espera;

        metricas.retorno_promedio +=
            procesos[i].retorno;

        metricas.respuesta_promedio +=
            procesos[i].respuesta;
    }

    metricas.espera_promedio /=
        TOTAL_PROCESOS;

    metricas.retorno_promedio /=
        TOTAL_PROCESOS;

    metricas.respuesta_promedio /=
        TOTAL_PROCESOS;

    metricas.makespan =
        makespan;

    metricas.throughput =
        makespan > 0
            ? (double)TOTAL_PROCESOS / makespan
            : 0.0;

    metricas.uso_cpu =
        makespan > 0
            ? 100.0 * (double)ocupado / makespan
            : 0.0;

    return metricas;
}

static void mostrar_resultados(
    const char *algoritmo,
    const Proceso procesos[],
    const Metricas *metricas
) {
    printf("\n");
    printf("===============================================================\n");
    printf(" RESULTADOS: %s\n", algoritmo);
    printf("===============================================================\n");
    printf("PID  PROCESO              INICIO  FIN   WT    TAT   RT\n");
    printf("---------------------------------------------------------------\n");

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        printf(
            "%-4d %-20s %-7d %-5d %-5d %-5d %-5d\n",
            procesos[i].pid,
            procesos[i].nombre,
            procesos[i].inicio,
            procesos[i].fin,
            procesos[i].espera,
            procesos[i].retorno,
            procesos[i].respuesta
        );
    }

    printf("---------------------------------------------------------------\n");
    printf(
        "Waiting Time promedio:       %.2f\n",
        metricas->espera_promedio
    );

    printf(
        "Turnaround Time promedio:    %.2f\n",
        metricas->retorno_promedio
    );

    printf(
        "Response Time promedio:      %.2f\n",
        metricas->respuesta_promedio
    );

    printf(
        "Throughput:                  %.3f procesos/u.t.\n",
        metricas->throughput
    );

    printf(
        "Uso de CPU:                  %.2f%%\n",
        metricas->uso_cpu
    );

    printf(
        "Makespan:                    %d unidades de tiempo\n",
        metricas->makespan
    );
}

static void mostrar_gantt(
    const BloqueGantt gantt[],
    int cantidad
) {
    if (cantidad <= 0) {
        printf("\nNo existen bloques para mostrar.\n");
        return;
    }

    printf("\nDiagrama de Gantt:\n\n");

    for (int i = 0; i < cantidad; i++) {
        printf(
            "| %-15s ",
            gantt[i].nombre
        );
    }

    printf("|\n");

    printf(
        "%d",
        gantt[0].inicio
    );

    for (int i = 0; i < cantidad; i++) {
        int fin =
            gantt[i].inicio +
            gantt[i].duracion;

        printf(
            " -> %d",
            fin
        );
    }

    printf("\n");
}

static Metricas ejecutar_fcfs(
    const Proceso base[],
    int mostrar
) {
    Proceso procesos[TOTAL_PROCESOS];
    BloqueGantt gantt[TOTAL_PROCESOS];

    int tiempo = 0;
    int ocupado = 0;

    copiar_procesos(
        procesos,
        base
    );

    for (int i = 0; i < TOTAL_PROCESOS - 1; i++) {
        for (int j = i + 1; j < TOTAL_PROCESOS; j++) {
            if (
                procesos[j].llegada <
                procesos[i].llegada
                ||
                (
                    procesos[j].llegada ==
                    procesos[i].llegada
                    &&
                    procesos[j].pid <
                    procesos[i].pid
                )
            ) {
                Proceso temporal =
                    procesos[i];

                procesos[i] =
                    procesos[j];

                procesos[j] =
                    temporal;
            }
        }
    }

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        if (
            tiempo <
            procesos[i].llegada
        ) {
            tiempo =
                procesos[i].llegada;
        }

        procesos[i].inicio =
            tiempo;

        procesos[i].respuesta =
            procesos[i].inicio -
            procesos[i].llegada;

        procesos[i].espera =
            procesos[i].respuesta;

        procesos[i].fin =
            tiempo +
            procesos[i].rafaga;

        procesos[i].retorno =
            procesos[i].fin -
            procesos[i].llegada;

        procesos[i].restante =
            0;

        procesos[i].completado =
            1;

        snprintf(
            gantt[i].nombre,
            sizeof(gantt[i].nombre),
            "%s",
            procesos[i].nombre
        );

        gantt[i].inicio =
            tiempo;

        gantt[i].duracion =
            procesos[i].rafaga;

        tiempo =
            procesos[i].fin;

        ocupado +=
            procesos[i].rafaga;
    }

    Metricas metricas =
        calcular_metricas(
            "FCFS",
            procesos,
            tiempo,
            ocupado
        );

    if (mostrar) {
        mostrar_resultados(
            "FCFS",
            procesos,
            &metricas
        );

        mostrar_gantt(
            gantt,
            TOTAL_PROCESOS
        );
    }

    return metricas;
}

static Metricas ejecutar_sjf(
    const Proceso base[],
    int mostrar
) {
    Proceso procesos[TOTAL_PROCESOS];
    BloqueGantt gantt[TOTAL_PROCESOS];

    int tiempo = 0;
    int completados = 0;
    int ocupado = 0;
    int cantidad_gantt = 0;

    copiar_procesos(
        procesos,
        base
    );

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        procesos[i].inicio = -1;
        procesos[i].fin = 0;
        procesos[i].espera = 0;
        procesos[i].retorno = 0;
        procesos[i].respuesta = 0;
        procesos[i].completado = 0;
    }

    while (
        completados <
        TOTAL_PROCESOS
    ) {
        int elegido = -1;

        for (int i = 0; i < TOTAL_PROCESOS; i++) {
            if (
                !procesos[i].completado
                &&
                procesos[i].llegada <= tiempo
            ) {
                if (
                    elegido == -1
                    ||
                    procesos[i].rafaga <
                    procesos[elegido].rafaga
                    ||
                    (
                        procesos[i].rafaga ==
                        procesos[elegido].rafaga
                        &&
                        procesos[i].llegada <
                        procesos[elegido].llegada
                    )
                    ||
                    (
                        procesos[i].rafaga ==
                        procesos[elegido].rafaga
                        &&
                        procesos[i].llegada ==
                        procesos[elegido].llegada
                        &&
                        procesos[i].pid <
                        procesos[elegido].pid
                    )
                ) {
                    elegido = i;
                }
            }
        }

        if (elegido == -1) {
            tiempo++;
            continue;
        }

        procesos[elegido].inicio =
            tiempo;

        procesos[elegido].respuesta =
            tiempo -
            procesos[elegido].llegada;

        procesos[elegido].espera =
            procesos[elegido].respuesta;

        procesos[elegido].fin =
            tiempo +
            procesos[elegido].rafaga;

        procesos[elegido].retorno =
            procesos[elegido].fin -
            procesos[elegido].llegada;

        procesos[elegido].restante =
            0;

        procesos[elegido].completado =
            1;

        snprintf(
            gantt[cantidad_gantt].nombre,
            sizeof(gantt[cantidad_gantt].nombre),
            "%s",
            procesos[elegido].nombre
        );

        gantt[cantidad_gantt].inicio =
            tiempo;

        gantt[cantidad_gantt].duracion =
            procesos[elegido].rafaga;

        cantidad_gantt++;

        tiempo =
            procesos[elegido].fin;

        ocupado +=
            procesos[elegido].rafaga;

        completados++;
    }

    Metricas metricas =
        calcular_metricas(
            "SJF",
            procesos,
            tiempo,
            ocupado
        );

    if (mostrar) {
        mostrar_resultados(
            "SJF NO APROPIATIVO",
            procesos,
            &metricas
        );

        mostrar_gantt(
            gantt,
            cantidad_gantt
        );
    }

    return metricas;
}

static Metricas ejecutar_round_robin(
    const Proceso base[],
    int mostrar
) {
    Proceso procesos[TOTAL_PROCESOS];
    BloqueGantt gantt[MAX_GANTT];

    int cola[MAX_GANTT];
    int en_cola[TOTAL_PROCESOS] = {0};

    int frente = 0;
    int final = 0;

    int tiempo = 0;
    int completados = 0;
    int ocupado = 0;
    int cantidad_gantt = 0;

    copiar_procesos(
        procesos,
        base
    );

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        procesos[i].restante =
            procesos[i].rafaga;

        procesos[i].inicio = -1;
        procesos[i].fin = 0;
        procesos[i].espera = 0;
        procesos[i].retorno = 0;
        procesos[i].respuesta = 0;
        procesos[i].completado = 0;
    }

    while (
        completados <
        TOTAL_PROCESOS
    ) {
        for (int i = 0; i < TOTAL_PROCESOS; i++) {
            if (
                !en_cola[i]
                &&
                !procesos[i].completado
                &&
                procesos[i].llegada <= tiempo
            ) {
                if (final >= MAX_GANTT) {
                    fprintf(
                        stderr,
                        "Error: la cola de Round Robin excedio su capacidad.\n"
                    );

                    exit(EXIT_FAILURE);
                }

                cola[final++] = i;
                en_cola[i] = 1;
            }
        }

        if (frente == final) {
            tiempo++;
            continue;
        }

        int indice =
            cola[frente++];

        if (
            procesos[indice].inicio ==
            -1
        ) {
            procesos[indice].inicio =
                tiempo;

            procesos[indice].respuesta =
                tiempo -
                procesos[indice].llegada;
        }

        int ejecucion =
            procesos[indice].restante <
            QUANTUM
                ? procesos[indice].restante
                : QUANTUM;

        if (
            cantidad_gantt >=
            MAX_GANTT
        ) {
            fprintf(
                stderr,
                "Error: el diagrama de Gantt excedio su capacidad.\n"
            );

            exit(EXIT_FAILURE);
        }

        snprintf(
            gantt[cantidad_gantt].nombre,
            sizeof(gantt[cantidad_gantt].nombre),
            "%s",
            procesos[indice].nombre
        );

        gantt[cantidad_gantt].inicio =
            tiempo;

        gantt[cantidad_gantt].duracion =
            ejecucion;

        cantidad_gantt++;

        tiempo +=
            ejecucion;

        ocupado +=
            ejecucion;

        procesos[indice].restante -=
            ejecucion;

        for (int i = 0; i < TOTAL_PROCESOS; i++) {
            if (
                !en_cola[i]
                &&
                !procesos[i].completado
                &&
                procesos[i].llegada <= tiempo
            ) {
                if (final >= MAX_GANTT) {
                    fprintf(
                        stderr,
                        "Error: la cola de Round Robin excedio su capacidad.\n"
                    );

                    exit(EXIT_FAILURE);
                }

                cola[final++] = i;
                en_cola[i] = 1;
            }
        }

        if (
            procesos[indice].restante >
            0
        ) {
            if (final >= MAX_GANTT) {
                fprintf(
                    stderr,
                    "Error: la cola de Round Robin excedio su capacidad.\n"
                );

                exit(EXIT_FAILURE);
            }

            cola[final++] =
                indice;
        } else {
            procesos[indice].completado =
                1;

            procesos[indice].fin =
                tiempo;

            procesos[indice].retorno =
                procesos[indice].fin -
                procesos[indice].llegada;

            procesos[indice].espera =
                procesos[indice].retorno -
                procesos[indice].rafaga;

            completados++;
        }
    }

    Metricas metricas =
        calcular_metricas(
            "Round Robin",
            procesos,
            tiempo,
            ocupado
        );

    if (mostrar) {
        mostrar_resultados(
            "ROUND ROBIN - QUANTUM 4",
            procesos,
            &metricas
        );

        mostrar_gantt(
            gantt,
            cantidad_gantt
        );
    }

    return metricas;
}

static Metricas ejecutar_prioridad_aging(
    const Proceso base[],
    int mostrar
) {
    Proceso procesos[TOTAL_PROCESOS];
    BloqueGantt gantt[TOTAL_PROCESOS];

    int tiempo = 0;
    int completados = 0;
    int ocupado = 0;
    int cantidad_gantt = 0;

    copiar_procesos(
        procesos,
        base
    );

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        procesos[i].inicio = -1;
        procesos[i].fin = 0;
        procesos[i].espera = 0;
        procesos[i].retorno = 0;
        procesos[i].respuesta = 0;
        procesos[i].completado = 0;
    }

    while (
        completados <
        TOTAL_PROCESOS
    ) {
        int elegido = -1;
        int mejor_prioridad = 999;

        for (int i = 0; i < TOTAL_PROCESOS; i++) {
            if (
                !procesos[i].completado
                &&
                procesos[i].llegada <= tiempo
            ) {
                int espera_actual =
                    tiempo -
                    procesos[i].llegada;

                int prioridad_dinamica =
                    procesos[i].prioridad -
                    (
                        espera_actual /
                        AGING_INTERVALO
                    );

                if (
                    prioridad_dinamica <
                    1
                ) {
                    prioridad_dinamica = 1;
                }

                if (
                    elegido == -1
                    ||
                    prioridad_dinamica <
                    mejor_prioridad
                    ||
                    (
                        prioridad_dinamica ==
                        mejor_prioridad
                        &&
                        procesos[i].llegada <
                        procesos[elegido].llegada
                    )
                    ||
                    (
                        prioridad_dinamica ==
                        mejor_prioridad
                        &&
                        procesos[i].llegada ==
                        procesos[elegido].llegada
                        &&
                        procesos[i].pid <
                        procesos[elegido].pid
                    )
                ) {
                    elegido = i;
                    mejor_prioridad =
                        prioridad_dinamica;
                }
            }
        }

        if (elegido == -1) {
            tiempo++;
            continue;
        }

        procesos[elegido].inicio =
            tiempo;

        procesos[elegido].respuesta =
            tiempo -
            procesos[elegido].llegada;

        procesos[elegido].espera =
            procesos[elegido].respuesta;

        procesos[elegido].fin =
            tiempo +
            procesos[elegido].rafaga;

        procesos[elegido].retorno =
            procesos[elegido].fin -
            procesos[elegido].llegada;

        procesos[elegido].restante =
            0;

        procesos[elegido].completado =
            1;

        snprintf(
            gantt[cantidad_gantt].nombre,
            sizeof(gantt[cantidad_gantt].nombre),
            "%s",
            procesos[elegido].nombre
        );

        gantt[cantidad_gantt].inicio =
            tiempo;

        gantt[cantidad_gantt].duracion =
            procesos[elegido].rafaga;

        cantidad_gantt++;

        tiempo =
            procesos[elegido].fin;

        ocupado +=
            procesos[elegido].rafaga;

        completados++;
    }

    Metricas metricas =
        calcular_metricas(
            "Prioridad + Aging",
            procesos,
            tiempo,
            ocupado
        );

    if (mostrar) {
        mostrar_resultados(
            "PRIORIDADES CON AGING",
            procesos,
            &metricas
        );

        mostrar_gantt(
            gantt,
            cantidad_gantt
        );
    }

    return metricas;
}

static void guardar_planificacion_csv(
    const Metricas resultados[],
    int cantidad
) {
    FILE *archivo =
        fopen(
            "resultados/planificacion.csv",
            "w"
        );

    if (archivo == NULL) {
        perror(
            "No se pudo crear resultados/planificacion.csv"
        );

        return;
    }

    fprintf(
        archivo,
        "Algoritmo,WT_promedio,TAT_promedio,RT_promedio,Throughput,Uso_CPU,Makespan\n"
    );

    for (int i = 0; i < cantidad; i++) {
        fprintf(
            archivo,
            "%s,%.2f,%.2f,%.2f,%.6f,%.2f,%d\n",
            resultados[i].algoritmo,
            resultados[i].espera_promedio,
            resultados[i].retorno_promedio,
            resultados[i].respuesta_promedio,
            resultados[i].throughput,
            resultados[i].uso_cpu,
            resultados[i].makespan
        );
    }

    fclose(archivo);
}

static void modulo_planificacion(
    const Proceso base[]
) {
    Metricas resultados[4];

    printf("\n");
    printf("============================================================\n");
    printf(" MODULO 2: PLANIFICACION DE CPU\n");
    printf("============================================================\n");

    resultados[0] =
        ejecutar_fcfs(
            base,
            1
        );

    resultados[1] =
        ejecutar_sjf(
            base,
            1
        );

    resultados[2] =
        ejecutar_round_robin(
            base,
            1
        );

    resultados[3] =
        ejecutar_prioridad_aging(
            base,
            1
        );

    printf("\n");
    printf("====================================================================================\n");
    printf("ALGORITMO             WT PROM.   TAT PROM.  RT PROM.   THROUGHPUT   CPU\n");
    printf("====================================================================================\n");

    for (int i = 0; i < 4; i++) {
        printf(
            "%-21s %-10.2f %-10.2f %-10.2f %-12.3f %.2f%%\n",
            resultados[i].algoritmo,
            resultados[i].espera_promedio,
            resultados[i].retorno_promedio,
            resultados[i].respuesta_promedio,
            resultados[i].throughput,
            resultados[i].uso_cpu
        );
    }

    printf("====================================================================================\n");

    double reduccion_respuesta =
        resultados[0].respuesta_promedio -
        resultados[2].respuesta_promedio;

    double mejora_respuesta =
        resultados[0].respuesta_promedio > 0.0
            ? 100.0 *
              reduccion_respuesta /
              resultados[0].respuesta_promedio
            : 0.0;

    printf("\nAnalisis de resultados:\n");
    printf(
        "- FCFS atiende por orden de llegada, pero puede generar efecto convoy.\n"
    );

    printf(
        "- SJF reduce los tiempos de espera cuando las rafagas cortas son atendidas primero.\n"
    );

    printf(
        "- Round Robin mejora la equidad y la respuesta de operaciones interactivas.\n"
    );

    printf(
        "- Prioridades con Aging evita la inanicion de procesos de baja prioridad.\n"
    );

    printf(
        "- Mejora del RT de Round Robin frente a FCFS: %.2f%%\n",
        mejora_respuesta
    );

    guardar_planificacion_csv(
        resultados,
        4
    );

    printf(
        "\nArchivo generado: resultados/planificacion.csv\n"
    );
}

/* ============================================================================
   MODULO DE SINCRONIZACION Y CONCURRENCIA
   ========================================================================== */

static double reloj_segundos(void) {
    struct timespec tiempo;

    if (
        clock_gettime(
            CLOCK_MONOTONIC,
            &tiempo
        ) != 0
    ) {
        return 0.0;
    }

    return
        (double)tiempo.tv_sec +
        (double)tiempo.tv_nsec /
        1000000000.0;
}

static void *depositar_sin_mutex(
    void *arg
) {
    (void)arg;

    for (
        int i = 0;
        i < OPERACIONES_HILO;
        i++
    ) {
        long temporal =
            saldo_global;

        sched_yield();

        saldo_global =
            temporal + 1;
    }

    return NULL;
}

static void *depositar_con_mutex(
    void *arg
) {
    (void)arg;

    for (
        int i = 0;
        i < OPERACIONES_HILO;
        i++
    ) {
        pthread_mutex_lock(
            &mutex_saldo
        );

        saldo_global++;

        pthread_mutex_unlock(
            &mutex_saldo
        );
    }

    return NULL;
}

static void *escribir_log_con_mutex(
    void *arg
) {
    (void)arg;

    for (
        int i = 0;
        i < 100;
        i++
    ) {
        pthread_mutex_lock(
            &mutex_log
        );

        entradas_log++;

        pthread_mutex_unlock(
            &mutex_log
        );
    }

    return NULL;
}

static void *transferencia_segura(
    void *arg
) {
    TransferenciaArgs *transferencia =
        (TransferenciaArgs *)arg;

    if (
        transferencia == NULL
        ||
        transferencia->origen == NULL
        ||
        transferencia->destino == NULL
        ||
        transferencia->monto_centimos <= 0
    ) {
        return NULL;
    }

    CuentaBancaria *primera;
    CuentaBancaria *segunda;

    if (
        transferencia->origen->id <
        transferencia->destino->id
    ) {
        primera =
            transferencia->origen;

        segunda =
            transferencia->destino;
    } else {
        primera =
            transferencia->destino;

        segunda =
            transferencia->origen;
    }

    pthread_mutex_lock(
        &primera->mutex
    );

    pthread_mutex_lock(
        &segunda->mutex
    );

    if (
        transferencia->origen->saldo_centimos >=
        transferencia->monto_centimos
    ) {
        transferencia->origen->saldo_centimos -=
            transferencia->monto_centimos;

        transferencia->destino->saldo_centimos +=
            transferencia->monto_centimos;

        transferencia->realizada = 1;
    } else {
        transferencia->realizada = 0;
    }

    pthread_mutex_unlock(
        &segunda->mutex
    );

    pthread_mutex_unlock(
        &primera->mutex
    );

    return NULL;
}

static void demostrar_deadlock(void) {
    pthread_t hilo_1;
    pthread_t hilo_2;

    TransferenciaArgs transferencia_1;
    TransferenciaArgs transferencia_2;

    long total_inicial;
    long total_final;

    printf("\n");
    printf("============================================================\n");
    printf(" ANALISIS Y PREVENCION DE DEADLOCK\n");
    printf("============================================================\n");

    printf(
        "Escenario peligroso:\n"
    );

    printf(
        "Hilo 1 bloquea la Cuenta A y espera la Cuenta B.\n"
    );

    printf(
        "Hilo 2 bloquea la Cuenta B y espera la Cuenta A.\n"
    );

    printf("\nCondiciones de Coffman:\n");
    printf("1. Exclusion mutua.\n");
    printf("2. Retencion y espera.\n");
    printf("3. No apropiacion.\n");
    printf("4. Espera circular.\n");

    total_inicial =
        cuentas[0].saldo_centimos +
        cuentas[1].saldo_centimos;

    transferencia_1.origen =
        &cuentas[0];

    transferencia_1.destino =
        &cuentas[1];

    transferencia_1.monto_centimos =
        10000;

    transferencia_1.realizada =
        0;

    transferencia_2.origen =
        &cuentas[1];

    transferencia_2.destino =
        &cuentas[0];

    transferencia_2.monto_centimos =
        5000;

    transferencia_2.realizada =
        0;

    if (
        pthread_create(
            &hilo_1,
            NULL,
            transferencia_segura,
            &transferencia_1
        ) != 0
    ) {
        fprintf(
            stderr,
            "Error al crear el primer hilo de transferencia.\n"
        );

        return;
    }

    if (
        pthread_create(
            &hilo_2,
            NULL,
            transferencia_segura,
            &transferencia_2
        ) != 0
    ) {
        fprintf(
            stderr,
            "Error al crear el segundo hilo de transferencia.\n"
        );

        pthread_join(
            hilo_1,
            NULL
        );

        return;
    }

    pthread_join(
        hilo_1,
        NULL
    );

    pthread_join(
        hilo_2,
        NULL
    );

    total_final =
        cuentas[0].saldo_centimos +
        cuentas[1].saldo_centimos;

    printf("\nPrevencion aplicada:\n");
    printf(
        "Los mutex se adquieren siempre en orden ascendente de numero de cuenta.\n"
    );

    printf(
        "Esta politica rompe la condicion de espera circular.\n"
    );

    printf(
        "Saldo final de la cuenta %d: ",
        cuentas[0].id
    );

    imprimir_monto(
        cuentas[0].saldo_centimos
    );

    printf("\n");

    printf(
        "Saldo final de la cuenta %d: ",
        cuentas[1].id
    );

    imprimir_monto(
        cuentas[1].saldo_centimos
    );

    printf("\n");

    printf(
        "Total antes de las transferencias: "
    );

    imprimir_monto(
        total_inicial
    );

    printf("\n");

    printf(
        "Total despues de las transferencias: "
    );

    imprimir_monto(
        total_final
    );

    printf("\n");

    printf(
        "Deadlocks ocurridos: 0\n"
    );
}

static void modulo_sincronizacion(void) {
    pthread_t hilos[NUM_HILOS];

    long saldo_esperado;
    long saldo_sin_mutex;
    long saldo_con_mutex;

    double inicio;
    double tiempo_sin_mutex;
    double tiempo_con_mutex;
    double overhead;

    printf("\n");
    printf("============================================================\n");
    printf(" MODULO 3: SINCRONIZACION Y CONCURRENCIA\n");
    printf("============================================================\n");

    saldo_esperado =
        100000L +
        (long)NUM_HILOS *
        OPERACIONES_HILO;

    saldo_global =
        100000;

    inicio =
        reloj_segundos();

    for (int i = 0; i < NUM_HILOS; i++) {
        if (
            pthread_create(
                &hilos[i],
                NULL,
                depositar_sin_mutex,
                NULL
            ) != 0
        ) {
            fprintf(
                stderr,
                "Error al crear el hilo %d sin mutex.\n",
                i + 1
            );

            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(
            hilos[i],
            NULL
        );
    }

    tiempo_sin_mutex =
        reloj_segundos() -
        inicio;

    saldo_sin_mutex =
        saldo_global;

    saldo_global =
        100000;

    inicio =
        reloj_segundos();

    for (int i = 0; i < NUM_HILOS; i++) {
        if (
            pthread_create(
                &hilos[i],
                NULL,
                depositar_con_mutex,
                NULL
            ) != 0
        ) {
            fprintf(
                stderr,
                "Error al crear el hilo %d con mutex.\n",
                i + 1
            );

            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(
            hilos[i],
            NULL
        );
    }

    tiempo_con_mutex =
        reloj_segundos() -
        inicio;

    saldo_con_mutex =
        saldo_global;

    entradas_log =
        0;

    for (int i = 0; i < NUM_HILOS; i++) {
        if (
            pthread_create(
                &hilos[i],
                NULL,
                escribir_log_con_mutex,
                NULL
            ) != 0
        ) {
            fprintf(
                stderr,
                "Error al crear el hilo de auditoria %d.\n",
                i + 1
            );

            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(
            hilos[i],
            NULL
        );
    }

    overhead =
        tiempo_con_mutex -
        tiempo_sin_mutex;

    printf("\nSecciones criticas identificadas:\n");
    printf(
        "1. Actualizacion concurrente del saldo compartido.\n"
    );

    printf(
        "2. Escritura concurrente del log de auditoria.\n"
    );

    printf(
        "3. Transferencias simultaneas entre cuentas bancarias.\n"
    );

    printf(
        "\nPrueba realizada con %d hilos y %d operaciones por hilo:\n",
        NUM_HILOS,
        OPERACIONES_HILO
    );

    printf(
        "Saldo esperado:                    S/ %ld\n",
        saldo_esperado
    );

    printf(
        "Saldo obtenido sin mutex:          S/ %ld\n",
        saldo_sin_mutex
    );

    printf(
        "Operaciones perdidas sin mutex:    %ld\n",
        saldo_esperado -
        saldo_sin_mutex
    );

    printf(
        "Saldo obtenido con mutex:          S/ %ld\n",
        saldo_con_mutex
    );

    printf(
        "Operaciones perdidas con mutex:    %ld\n",
        saldo_esperado -
        saldo_con_mutex
    );

    printf(
        "Entradas esperadas en el log:      %d\n",
        NUM_HILOS * 100
    );

    printf(
        "Entradas registradas con mutex:    %d\n",
        entradas_log
    );

    printf(
        "Tiempo sin mutex:                  %.6f segundos\n",
        tiempo_sin_mutex
    );

    printf(
        "Tiempo con mutex:                  %.6f segundos\n",
        tiempo_con_mutex
    );

    printf(
        "Overhead aproximado del mutex:     %.6f segundos\n",
        overhead
    );

    FILE *archivo =
        fopen(
            "resultados/sincronizacion.txt",
            "w"
        );

    if (archivo == NULL) {
        perror(
            "No se pudo crear resultados/sincronizacion.txt"
        );
    } else {
        fprintf(
            archivo,
            "MODULO 3 - SINCRONIZACION Y CONCURRENCIA\n"
        );

        fprintf(
            archivo,
            "Hilos: %d\n",
            NUM_HILOS
        );

        fprintf(
            archivo,
            "Operaciones por hilo: %d\n",
            OPERACIONES_HILO
        );

        fprintf(
            archivo,
            "Saldo esperado: %ld\n",
            saldo_esperado
        );

        fprintf(
            archivo,
            "Saldo sin mutex: %ld\n",
            saldo_sin_mutex
        );

        fprintf(
            archivo,
            "Operaciones perdidas sin mutex: %ld\n",
            saldo_esperado -
            saldo_sin_mutex
        );

        fprintf(
            archivo,
            "Saldo con mutex: %ld\n",
            saldo_con_mutex
        );

        fprintf(
            archivo,
            "Operaciones perdidas con mutex: %ld\n",
            saldo_esperado -
            saldo_con_mutex
        );

        fprintf(
            archivo,
            "Entradas de auditoria: %d\n",
            entradas_log
        );

        fprintf(
            archivo,
            "Tiempo sin mutex: %.6f\n",
            tiempo_sin_mutex
        );

        fprintf(
            archivo,
            "Tiempo con mutex: %.6f\n",
            tiempo_con_mutex
        );

        fprintf(
            archivo,
            "Overhead: %.6f\n",
            overhead
        );

        fclose(archivo);
    }

    demostrar_deadlock();

    printf(
        "\nArchivo generado: resultados/sincronizacion.txt\n"
    );
}

/* ============================================================================
   MODULO DE GESTION DE MEMORIA
   ========================================================================== */

static void memoria_inicializar(
    Memoria *memoria
) {
    memoria->cantidad = 1;
    memoria->denegadas = 0;

    memoria->bloques[0].inicio = 0;
    memoria->bloques[0].tamanio = MEMORIA_TOTAL;
    memoria->bloques[0].libre = 1;

    snprintf(
        memoria->bloques[0].proceso,
        sizeof(memoria->bloques[0].proceso),
        "%s",
        "-"
    );
}

static int memoria_asignar(
    Memoria *memoria,
    const char *nombre,
    int tamanio,
    Estrategia estrategia
) {
    int elegido = -1;

    for (int i = 0; i < memoria->cantidad; i++) {
        if (
            memoria->bloques[i].libre
            &&
            memoria->bloques[i].tamanio >= tamanio
        ) {
            if (estrategia == FIRST_FIT) {
                elegido = i;
                break;
            }

            if (
                elegido == -1
                ||
                memoria->bloques[i].tamanio <
                memoria->bloques[elegido].tamanio
            ) {
                elegido = i;
            }
        }
    }

    if (elegido == -1) {
        memoria->denegadas++;
        return 0;
    }

    Particion original =
        memoria->bloques[elegido];

    if (
        original.tamanio > tamanio
        &&
        memoria->cantidad < MAX_PARTICIONES
    ) {
        for (
            int i = memoria->cantidad;
            i > elegido + 1;
            i--
        ) {
            memoria->bloques[i] =
                memoria->bloques[i - 1];
        }

        memoria->bloques[elegido + 1].inicio =
            original.inicio + tamanio;

        memoria->bloques[elegido + 1].tamanio =
            original.tamanio - tamanio;

        memoria->bloques[elegido + 1].libre =
            1;

        snprintf(
            memoria->bloques[elegido + 1].proceso,
            sizeof(memoria->bloques[elegido + 1].proceso),
            "%s",
            "-"
        );

        memoria->cantidad++;
    }

    memoria->bloques[elegido].inicio =
        original.inicio;

    memoria->bloques[elegido].tamanio =
        tamanio;

    memoria->bloques[elegido].libre =
        0;

    snprintf(
        memoria->bloques[elegido].proceso,
        sizeof(memoria->bloques[elegido].proceso),
        "%s",
        nombre
    );

    return 1;
}

static void memoria_fusionar(
    Memoria *memoria
) {
    int i = 0;

    while (
        i <
        memoria->cantidad - 1
    ) {
        if (
            memoria->bloques[i].libre
            &&
            memoria->bloques[i + 1].libre
        ) {
            memoria->bloques[i].tamanio +=
                memoria->bloques[i + 1].tamanio;

            for (
                int j = i + 1;
                j < memoria->cantidad - 1;
                j++
            ) {
                memoria->bloques[j] =
                    memoria->bloques[j + 1];
            }

            memoria->cantidad--;
        } else {
            i++;
        }
    }
}

static int memoria_liberar(
    Memoria *memoria,
    const char *nombre
) {
    for (int i = 0; i < memoria->cantidad; i++) {
        if (
            !memoria->bloques[i].libre
            &&
            strcmp(
                memoria->bloques[i].proceso,
                nombre
            ) == 0
        ) {
            memoria->bloques[i].libre =
                1;

            snprintf(
                memoria->bloques[i].proceso,
                sizeof(memoria->bloques[i].proceso),
                "%s",
                "-"
            );

            memoria_fusionar(
                memoria
            );

            return 1;
        }
    }

    return 0;
}

static void memoria_mostrar(
    const Memoria *memoria,
    FILE *salida
) {
    fprintf(
        salida,
        "\n============================================================\n"
    );

    fprintf(
        salida,
        " MAPA DE PARTICIONES\n"
    );

    fprintf(
        salida,
        "============================================================\n"
    );

    fprintf(
        salida,
        "INICIO   TAMANIO   ESTADO      PROCESO\n"
    );

    fprintf(
        salida,
        "------------------------------------------------------------\n"
    );

    for (int i = 0; i < memoria->cantidad; i++) {
        fprintf(
            salida,
            "%-8d %-9d %-11s %s\n",
            memoria->bloques[i].inicio,
            memoria->bloques[i].tamanio,
            memoria->bloques[i].libre
                ? "Libre"
                : "Ocupado",
            memoria->bloques[i].proceso
        );
    }
}

static double fragmentacion_externa(
    const Memoria *memoria
) {
    int memoria_libre = 0;
    int mayor_hueco = 0;

    for (int i = 0; i < memoria->cantidad; i++) {
        if (memoria->bloques[i].libre) {
            memoria_libre +=
                memoria->bloques[i].tamanio;

            if (
                memoria->bloques[i].tamanio >
                mayor_hueco
            ) {
                mayor_hueco =
                    memoria->bloques[i].tamanio;
            }
        }
    }

    if (memoria_libre == 0) {
        return 0.0;
    }

    return
        100.0 *
        (
            1.0 -
            (double)mayor_hueco /
            memoria_libre
        );
}

static int contar_huecos_libres(
    const Memoria *memoria
) {
    int huecos = 0;

    for (int i = 0; i < memoria->cantidad; i++) {
        if (memoria->bloques[i].libre) {
            huecos++;
        }
    }

    return huecos;
}

static void ejecutar_prueba_memoria(
    Estrategia estrategia,
    int mostrar,
    double *fragmentacion_final,
    int *huecos_finales,
    int *denegadas_finales
) {
    Memoria memoria;

    memoria_inicializar(
        &memoria
    );

    memoria_asignar(
        &memoria,
        "Login1",
        96,
        estrategia
    );

    memoria_asignar(
        &memoria,
        "Consulta1",
        64,
        estrategia
    );

    memoria_asignar(
        &memoria,
        "Transferencia1",
        256,
        estrategia
    );

    memoria_asignar(
        &memoria,
        "Pago1",
        192,
        estrategia
    );

    memoria_asignar(
        &memoria,
        "ActualizaBD",
        128,
        estrategia
    );

    memoria_liberar(
        &memoria,
        "Consulta1"
    );

    memoria_asignar(
        &memoria,
        "Consulta2",
        64,
        estrategia
    );

    memoria_asignar(
        &memoria,
        "Transferencia2",
        256,
        estrategia
    );

    memoria_liberar(
        &memoria,
        "Pago1"
    );

    memoria_asignar(
        &memoria,
        "Liquidacion",
        512,
        estrategia
    );

    if (mostrar) {
        printf(
            "\nEstado intermedio despues de 10 operaciones:\n"
        );

        memoria_mostrar(
            &memoria,
            stdout
        );
    }

    memoria_asignar(
        &memoria,
        "Pago2",
        192,
        estrategia
    );

    memoria_liberar(
        &memoria,
        "Login1"
    );

    memoria_asignar(
        &memoria,
        "Login2",
        96,
        estrategia
    );

    memoria_liberar(
        &memoria,
        "ActualizaBD"
    );

    memoria_asignar(
        &memoria,
        "Consulta3",
        64,
        estrategia
    );

    *fragmentacion_final =
        fragmentacion_externa(
            &memoria
        );

    *huecos_finales =
        contar_huecos_libres(
            &memoria
        );

    *denegadas_finales =
        memoria.denegadas;

    if (mostrar) {
        printf(
            "\nEstado final despues de 15 operaciones:\n"
        );

        memoria_mostrar(
            &memoria,
            stdout
        );

        printf(
            "Fragmentacion externa: %.2f%%\n",
            *fragmentacion_final
        );

        printf(
            "Huecos libres: %d\n",
            *huecos_finales
        );

        printf(
            "Asignaciones denegadas: %d\n",
            *denegadas_finales
        );
    }
}

static void modulo_memoria(void) {
    double fragmentacion_first = 0.0;
    double fragmentacion_best = 0.0;

    int huecos_first = 0;
    int huecos_best = 0;

    int denegadas_first = 0;
    int denegadas_best = 0;

    printf("\n");
    printf("============================================================\n");
    printf(" MODULO 4: GESTION DE MEMORIA\n");
    printf("============================================================\n");

    printf("\n--- FIRST-FIT ---\n");

    ejecutar_prueba_memoria(
        FIRST_FIT,
        1,
        &fragmentacion_first,
        &huecos_first,
        &denegadas_first
    );

    printf("\n--- BEST-FIT ---\n");

    ejecutar_prueba_memoria(
        BEST_FIT,
        1,
        &fragmentacion_best,
        &huecos_best,
        &denegadas_best
    );

    printf("\n");
    printf("==============================================================\n");
    printf("ESTRATEGIA   FRAGMENTACION   HUECOS LIBRES   DENEGADAS\n");
    printf("==============================================================\n");

    printf(
        "First-Fit    %-15.2f %-15d %d\n",
        fragmentacion_first,
        huecos_first,
        denegadas_first
    );

    printf(
        "Best-Fit     %-15.2f %-15d %d\n",
        fragmentacion_best,
        huecos_best,
        denegadas_best
    );

    printf("==============================================================\n");

    if (
        fragmentacion_first <
        fragmentacion_best
    ) {
        printf(
            "\nPara esta carga, First-Fit produjo menor fragmentacion externa.\n"
        );
    } else if (
        fragmentacion_best <
        fragmentacion_first
    ) {
        printf(
            "\nPara esta carga, Best-Fit produjo menor fragmentacion externa.\n"
        );
    } else {
        printf(
            "\nPara esta carga, ambas estrategias produjeron la misma fragmentacion.\n"
        );
    }

    FILE *archivo =
        fopen(
            "resultados/memoria.csv",
            "w"
        );

    if (archivo == NULL) {
        perror(
            "No se pudo crear resultados/memoria.csv"
        );
    } else {
        fprintf(
            archivo,
            "Estrategia,Fragmentacion_externa,Huecos_libres,Asignaciones_denegadas\n"
        );

        fprintf(
            archivo,
            "First-Fit,%.2f,%d,%d\n",
            fragmentacion_first,
            huecos_first,
            denegadas_first
        );

        fprintf(
            archivo,
            "Best-Fit,%.2f,%d,%d\n",
            fragmentacion_best,
            huecos_best,
            denegadas_best
        );

        fclose(archivo);
    }

    printf(
        "\nArchivo generado: resultados/memoria.csv\n"
    );
}

/* ============================================================================
   MODULO INTEGRADO
   ========================================================================== */

static void modulo_integrado(
    const Proceso base[]
) {
    Memoria memoria;

    int aceptados = 0;
    int rechazados = 0;

    int memoria_actual = 0;
    int memoria_maxima = 0;

    pthread_t hilos[NUM_HILOS];

    printf("\n");
    printf("============================================================\n");
    printf(" MODULO 5: SISTEMA INTEGRADO\n");
    printf("============================================================\n");

    memoria_inicializar(
        &memoria
    );

    printf(
        "\n1. Asignacion de memoria mediante Best-Fit:\n"
    );

    for (int i = 0; i < TOTAL_PROCESOS; i++) {
        if (
            memoria_asignar(
                &memoria,
                base[i].nombre,
                base[i].memoria,
                BEST_FIT
            )
        ) {
            aceptados++;

            memoria_actual +=
                base[i].memoria;

            if (
                memoria_actual >
                memoria_maxima
            ) {
                memoria_maxima =
                    memoria_actual;
            }

            printf(
                "[OK] %-20s %d MB\n",
                base[i].nombre,
                base[i].memoria
            );
        } else {
            rechazados++;

            printf(
                "[DENEGADO] %-20s %d MB\n",
                base[i].nombre,
                base[i].memoria
            );
        }
    }

    printf(
        "\n2. Planificacion de procesos con Round Robin:\n"
    );

    Metricas round_robin =
        ejecutar_round_robin(
            base,
            0
        );

    Metricas fcfs =
        ejecutar_fcfs(
            base,
            0
        );

    printf(
        "Round Robin finalizado con quantum = %d.\n",
        QUANTUM
    );

    printf(
        "\n3. Acceso sincronizado al saldo mediante mutex:\n"
    );

    saldo_global =
        100000;

    for (int i = 0; i < NUM_HILOS; i++) {
        if (
            pthread_create(
                &hilos[i],
                NULL,
                depositar_con_mutex,
                NULL
            ) != 0
        ) {
            fprintf(
                stderr,
                "Error al crear el hilo integrado %d.\n",
                i + 1
            );

            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(
            hilos[i],
            NULL
        );
    }

    long saldo_esperado =
        100000L +
        (long)NUM_HILOS *
        OPERACIONES_HILO;

    long conflictos =
        saldo_esperado -
        saldo_global;

    double mejora_respuesta =
        fcfs.respuesta_promedio > 0.0
            ? 100.0 *
              (
                  fcfs.respuesta_promedio -
                  round_robin.respuesta_promedio
              )
              /
              fcfs.respuesta_promedio
            : 0.0;

    printf("\n");
    printf("============================================================\n");
    printf(" METRICAS DEL SISTEMA INTEGRADO\n");
    printf("============================================================\n");

    printf(
        "Procesos recibidos:                  %d\n",
        TOTAL_PROCESOS
    );

    printf(
        "Procesos con memoria asignada:       %d\n",
        aceptados
    );

    printf(
        "Procesos rechazados:                 %d\n",
        rechazados
    );

    printf(
        "Throughput total:                    %.3f procesos/u.t.\n",
        round_robin.throughput
    );

    printf(
        "Tiempo de respuesta promedio:        %.2f\n",
        round_robin.respuesta_promedio
    );

    printf(
        "Uso de CPU:                          %.2f%%\n",
        round_robin.uso_cpu
    );

    printf(
        "Uso maximo de memoria:               %d MB de %d MB\n",
        memoria_maxima,
        MEMORIA_TOTAL
    );

    printf(
        "Fragmentacion externa final:         %.2f%%\n",
        fragmentacion_externa(
            &memoria
        )
    );

    printf(
        "Conflictos con mutex:                %ld\n",
        conflictos
    );

    printf(
        "Deadlocks ocurridos:                 0\n"
    );

    printf(
        "Mejora de RT RR frente a FCFS:       %.2f%%\n",
        mejora_respuesta
    );

    printf("\nCuellos de botella identificados:\n");

    printf(
        "1. La liquidacion nocturna incrementa la espera en FCFS.\n"
    );

    printf(
        "2. Un mutex global limita el paralelismo entre operaciones independientes.\n"
    );

    printf(
        "3. La memoria total puede impedir que todos los procesos residan simultaneamente.\n"
    );

    printf("\nPlan de optimizacion:\n");

    printf(
        "- Usar Round Robin para operaciones bancarias interactivas.\n"
    );

    printf(
        "- Ejecutar Liquidacion en una cola batch de baja prioridad.\n"
    );

    printf(
        "- Usar un mutex por cuenta para permitir mayor paralelismo.\n"
    );

    printf(
        "- Liberar memoria cuando cada proceso termina su ejecucion.\n"
    );

    printf(
        "- Beneficio medido en RT: %.2f%% frente a FCFS.\n",
        mejora_respuesta
    );

    FILE *archivo =
        fopen(
            "resultados/integracion.txt",
            "w"
        );

    if (archivo == NULL) {
        perror(
            "No se pudo crear resultados/integracion.txt"
        );
    } else {
        fprintf(
            archivo,
            "MODULO 5 - SISTEMA INTEGRADO\n"
        );

        fprintf(
            archivo,
            "Procesos recibidos: %d\n",
            TOTAL_PROCESOS
        );

        fprintf(
            archivo,
            "Procesos aceptados: %d\n",
            aceptados
        );

        fprintf(
            archivo,
            "Procesos rechazados: %d\n",
            rechazados
        );

        fprintf(
            archivo,
            "Throughput: %.6f\n",
            round_robin.throughput
        );

        fprintf(
            archivo,
            "RT promedio: %.2f\n",
            round_robin.respuesta_promedio
        );

        fprintf(
            archivo,
            "Uso CPU: %.2f\n",
            round_robin.uso_cpu
        );

        fprintf(
            archivo,
            "Uso maximo memoria: %d MB\n",
            memoria_maxima
        );

        fprintf(
            archivo,
            "Fragmentacion externa: %.2f\n",
            fragmentacion_externa(
                &memoria
            )
        );

        fprintf(
            archivo,
            "Conflictos con mutex: %ld\n",
            conflictos
        );

        fprintf(
            archivo,
            "Deadlocks: 0\n"
        );

        fprintf(
            archivo,
            "Mejora RT RR vs FCFS: %.2f%%\n",
            mejora_respuesta
        );

        fclose(archivo);
    }

    printf(
        "\nArchivo generado: resultados/integracion.txt\n"
    );
}

/* ============================================================================
   DEMOSTRACION COMPLETA
   ========================================================================== */

static void demostracion_completa(
    const Proceso base[]
) {
    printf("\n");
    printf("============================================================\n");
    printf(" DEMOSTRACION COMPLETA DEL PROYECTO SIMGESRC\n");
    printf("============================================================\n");

    mostrar_carga(
        base
    );

    modulo_planificacion(
        base
    );

    modulo_sincronizacion();

    modulo_memoria();

    modulo_integrado(
        base
    );

    printf("\n");
    printf("============================================================\n");
    printf(" DEMOSTRACION COMPLETA FINALIZADA\n");
    printf("============================================================\n");
}

/* ============================================================================
   MENU PRINCIPAL
   ========================================================================== */

static void menu_principal(void) {
    Proceso procesos[TOTAL_PROCESOS];

    int opcion;

    cargar_procesos(
        procesos
    );

    inicializar_cuentas();

    crear_directorio_resultados();

    do {
        encabezado();

        printf(
            "1. Operaciones bancarias\n"
        );

        printf(
            "2. Mostrar carga de procesos\n"
        );

        printf(
            "3. Ejecutar planificacion de CPU\n"
        );

        printf(
            "4. Ejecutar sincronizacion y concurrencia\n"
        );

        printf(
            "5. Ejecutar gestion de memoria\n"
        );

        printf(
            "6. Ejecutar sistema integrado\n"
        );

        printf(
            "7. Ejecutar demostracion completa\n"
        );

        printf(
            "0. Salir\n"
        );

        opcion = leer_entero(
            "\nSeleccione una opcion: ",
            0,
            7
        );

        switch (opcion) {
            case 1:
                modulo_bancario();
                break;

            case 2:
                mostrar_carga(
                    procesos
                );

                pausa();
                break;

            case 3:
                modulo_planificacion(
                    procesos
                );

                pausa();
                break;

            case 4:
                modulo_sincronizacion();

                pausa();
                break;

            case 5:
                modulo_memoria();

                pausa();
                break;

            case 6:
                modulo_integrado(
                    procesos
                );

                pausa();
                break;

            case 7:
                demostracion_completa(
                    procesos
                );

                pausa();
                break;

            case 0:
                printf(
                    "\nPrograma finalizado correctamente.\n"
                );
                break;

            default:
                printf(
                    "\nOpcion no valida.\n"
                );

                pausa();
                break;
        }
    } while (opcion != 0);

    destruir_cuentas();
}

/* ============================================================================
   FUNCION PRINCIPAL
   ========================================================================== */

int main(void) {
    menu_principal();

    return EXIT_SUCCESS;
}
