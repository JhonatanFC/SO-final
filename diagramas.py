from pathlib import Path

import matplotlib.pyplot as plt
import pandas as pd


# ============================================================
# CONFIGURACIÓN GENERAL
# ============================================================

CARPETA_SALIDA = Path("resultados_graficos")
CARPETA_SALIDA.mkdir(exist_ok=True)

plt.rcParams.update({
    "font.family": "DejaVu Sans",
    "font.size": 11,
    "axes.titlesize": 14,
    "axes.labelsize": 11,
    "figure.figsize": (10, 6),
})


# ============================================================
# DATOS DE PLANIFICACIÓN
# ============================================================

datos_planificacion = pd.DataFrame({
    "Algoritmo": [
        "FCFS",
        "SJF",
        "Round Robin",
        "Prioridades + Aging"
    ],
    "WT": [12.90, 10.70, 16.90, 13.60],
    "TAT": [18.20, 16.00, 22.20, 18.90],
    "RT": [12.90, 10.70, 8.50, 13.60],
    "Throughput": [0.189, 0.189, 0.189, 0.189],
    "CPU": [100, 100, 100, 100],
    "Makespan": [53, 53, 53, 53]
})


# ============================================================
# FUNCIÓN PARA COLOCAR VALORES SOBRE LAS BARRAS
# ============================================================

def colocar_valores(ax, decimales=2):
    for barra in ax.patches:
        altura = barra.get_height()

        if decimales == 0:
            texto = f"{altura:.0f}"
        elif decimales == 1:
            texto = f"{altura:.1f}"
        else:
            texto = f"{altura:.2f}"

        ax.annotate(
            texto,
            xy=(barra.get_x() + barra.get_width() / 2, altura),
            xytext=(0, 4),
            textcoords="offset points",
            ha="center",
            va="bottom",
            fontsize=9
        )


# ============================================================
# GRÁFICO 1: TIEMPO PROMEDIO DE ESPERA
# ============================================================

fig, ax = plt.subplots()

ax.bar(
    datos_planificacion["Algoritmo"],
    datos_planificacion["WT"]
)

ax.set_title("Tiempo promedio de espera por algoritmo")
ax.set_xlabel("Algoritmo de planificación")
ax.set_ylabel("Tiempo de espera promedio")
ax.set_ylim(0, 20)
ax.grid(axis="y", linestyle="--", alpha=0.4)

colocar_valores(ax)

plt.xticks(rotation=15)
plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "01_tiempo_espera.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 2: TIEMPO PROMEDIO DE RETORNO
# ============================================================

fig, ax = plt.subplots()

ax.bar(
    datos_planificacion["Algoritmo"],
    datos_planificacion["TAT"]
)

ax.set_title("Tiempo promedio de retorno por algoritmo")
ax.set_xlabel("Algoritmo de planificación")
ax.set_ylabel("Tiempo de retorno promedio")
ax.set_ylim(0, 25)
ax.grid(axis="y", linestyle="--", alpha=0.4)

colocar_valores(ax)

plt.xticks(rotation=15)
plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "02_tiempo_retorno.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 3: TIEMPO PROMEDIO DE RESPUESTA
# ============================================================

fig, ax = plt.subplots()

ax.bar(
    datos_planificacion["Algoritmo"],
    datos_planificacion["RT"]
)

ax.set_title("Tiempo promedio de respuesta por algoritmo")
ax.set_xlabel("Algoritmo de planificación")
ax.set_ylabel("Tiempo de respuesta promedio")
ax.set_ylim(0, 16)
ax.grid(axis="y", linestyle="--", alpha=0.4)

colocar_valores(ax)

plt.xticks(rotation=15)
plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "03_tiempo_respuesta.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 4: COMPARACIÓN GENERAL WT, TAT Y RT
# ============================================================

comparacion = datos_planificacion.set_index("Algoritmo")[["WT", "TAT", "RT"]]

fig, ax = plt.subplots(figsize=(11, 6))

comparacion.plot(
    kind="bar",
    ax=ax
)

ax.set_title("Comparación de métricas de planificación")
ax.set_xlabel("Algoritmo de planificación")
ax.set_ylabel("Unidades de tiempo")
ax.legend([
    "Tiempo de espera (WT)",
    "Tiempo de retorno (TAT)",
    "Tiempo de respuesta (RT)"
])
ax.grid(axis="y", linestyle="--", alpha=0.4)

plt.xticks(rotation=15)
plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "04_comparacion_metricas.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 5: UTILIZACIÓN DE CPU
# ============================================================

fig, ax = plt.subplots()

ax.bar(
    datos_planificacion["Algoritmo"],
    datos_planificacion["CPU"]
)

ax.set_title("Utilización del procesador por algoritmo")
ax.set_xlabel("Algoritmo de planificación")
ax.set_ylabel("Utilización de CPU (%)")
ax.set_ylim(0, 110)
ax.grid(axis="y", linestyle="--", alpha=0.4)

colocar_valores(ax, decimales=0)

plt.xticks(rotation=15)
plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "05_utilizacion_cpu.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 6: THROUGHPUT
# ============================================================

fig, ax = plt.subplots()

ax.bar(
    datos_planificacion["Algoritmo"],
    datos_planificacion["Throughput"]
)

ax.set_title("Throughput por algoritmo de planificación")
ax.set_xlabel("Algoritmo de planificación")
ax.set_ylabel("Procesos por unidad de tiempo")
ax.set_ylim(0, 0.22)
ax.grid(axis="y", linestyle="--", alpha=0.4)

for barra in ax.patches:
    altura = barra.get_height()

    ax.annotate(
        f"{altura:.3f}",
        xy=(barra.get_x() + barra.get_width() / 2, altura),
        xytext=(0, 4),
        textcoords="offset points",
        ha="center",
        va="bottom",
        fontsize=9
    )

plt.xticks(rotation=15)
plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "06_throughput.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 7: USO DE MEMORIA
# ============================================================

datos_memoria = pd.DataFrame({
    "Estado": ["Memoria utilizada", "Memoria libre"],
    "Memoria_MB": [1824, 224]
})

fig, ax = plt.subplots(figsize=(8, 6))

ax.pie(
    datos_memoria["Memoria_MB"],
    labels=datos_memoria["Estado"],
    autopct="%1.2f%%",
    startangle=90
)

ax.set_title("Distribución de la memoria principal")

plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "07_distribucion_memoria.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 8: MEMORIA SOLICITADA POR PROCESO
# ============================================================

datos_procesos = pd.DataFrame({
    "Proceso": [
        "Consulta1",
        "Consulta2",
        "Login1",
        "Transferencia1",
        "Pago1",
        "Consulta3",
        "ActualizaBD",
        "Transferencia2",
        "Pago2",
        "Liquidación"
    ],
    "Memoria_MB": [
        64,
        64,
        96,
        256,
        192,
        64,
        128,
        256,
        192,
        512
    ]
})

fig, ax = plt.subplots(figsize=(11, 7))

ax.barh(
    datos_procesos["Proceso"],
    datos_procesos["Memoria_MB"]
)

ax.set_title("Memoria solicitada por cada proceso")
ax.set_xlabel("Memoria solicitada (MB)")
ax.set_ylabel("Proceso")
ax.grid(axis="x", linestyle="--", alpha=0.4)

for barra in ax.patches:
    ancho = barra.get_width()

    ax.annotate(
        f"{ancho:.0f} MB",
        xy=(ancho, barra.get_y() + barra.get_height() / 2),
        xytext=(5, 0),
        textcoords="offset points",
        va="center",
        fontsize=9
    )

plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "08_memoria_por_proceso.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 9: OPERACIONES CONCURRENTES
# ============================================================

datos_concurrencia = pd.DataFrame({
    "Indicador": [
        "Hilos",
        "Operaciones por hilo",
        "Operaciones totales",
        "Entradas de auditoría"
    ],
    "Cantidad": [
        8,
        2000,
        16000,
        800
    ]
})

fig, ax = plt.subplots(figsize=(10, 6))

ax.barh(
    datos_concurrencia["Indicador"],
    datos_concurrencia["Cantidad"]
)

ax.set_title("Indicadores del módulo de concurrencia")
ax.set_xlabel("Cantidad")
ax.set_ylabel("Indicador")
ax.grid(axis="x", linestyle="--", alpha=0.4)

for barra in ax.patches:
    ancho = barra.get_width()

    ax.annotate(
        f"{ancho:.0f}",
        xy=(ancho, barra.get_y() + barra.get_height() / 2),
        xytext=(5, 0),
        textcoords="offset points",
        va="center",
        fontsize=9
    )

plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "09_indicadores_concurrencia.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


# ============================================================
# GRÁFICO 10: SALDO ESPERADO Y SALDO CON MUTEX
# ============================================================

datos_saldo = pd.DataFrame({
    "Resultado": [
        "Saldo inicial",
        "Saldo esperado",
        "Saldo con mutex"
    ],
    "Monto": [
        100000,
        116000,
        116000
    ]
})

fig, ax = plt.subplots()

ax.bar(
    datos_saldo["Resultado"],
    datos_saldo["Monto"]
)

ax.set_title("Validación del saldo mediante sincronización")
ax.set_xlabel("Resultado")
ax.set_ylabel("Saldo simulado")
ax.set_ylim(0, 130000)
ax.grid(axis="y", linestyle="--", alpha=0.4)

for barra in ax.patches:
    altura = barra.get_height()

    ax.annotate(
        f"S/ {altura:,.0f}",
        xy=(barra.get_x() + barra.get_width() / 2, altura),
        xytext=(0, 4),
        textcoords="offset points",
        ha="center",
        va="bottom",
        fontsize=9
    )

plt.tight_layout()

plt.savefig(
    CARPETA_SALIDA / "10_validacion_saldo.png",
    dpi=300,
    bbox_inches="tight"
)

plt.close()


print("Gráficos generados correctamente.")
print(f"Ubicación: {CARPETA_SALIDA.resolve()}")