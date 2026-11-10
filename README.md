# WESTSIDE LEGACY — PC (Unreal Engine 5.8)

Código de Unreal para la versión de PC. La versión de iPhone (Godot) está en el repositorio
`westside-mobile`; ambas comparten diseño y reglas de juego.

## Plugin `WestsideCrime`

Sistema de policía con **testigos y reconocimiento por ropa y vehículo**, con la misma lógica y
los mismos valores que `scripts/world/crime.gd` de la versión móvil:

- La policía no sabe quién eres hasta que alguien te ve. Si un agente presencia el delito te
  identifica al momento; si no, los peatones cercanos llaman y tardan unos segundos en dar el aviso.
  Si interrumpes al testigo antes de que cuelgue, no hay denuncia.
- El aviso lleva una **descripción**: la ropa que llevabas y, si ibas en coche, el coche y su color.
- Los agentes acuden al último lugar conocido y **buscan**. A quien encaja con la descripción lo
  reconocen a 34 m a pie (42 m en coche); a quien se ha cambiado de ropa, de coche o lo ha
  repintado, solo a 7 m (9 m en coche).
- Cuando un agente te reconoce, la descripción se actualiza con lo que llevas en ese momento.
- Las estrellas bajan de una en una tras 18 s sin que nadie te reconozca.

> **Estado:** este código se ha escrito sin poder compilarlo (no hay Unreal en el entorno donde
> se generó). Usa solo API estándar de UE5, pero la primera compilación en tu equipo puede pedir
> algún ajuste. No incluye mapas, modelos ni Blueprints: es la lógica, lista para conectar.

### Instalación

1. Copia la carpeta `Plugins/WestsideCrime` dentro de la carpeta `Plugins` de tu proyecto de Unreal
   (créala junto al `.uproject` si no existe). El proyecto debe ser de C++ (si es solo Blueprint,
   añade antes una clase C++ vacía desde *Tools → New C++ Class*).
2. Abre el proyecto; Unreal pedirá compilar el plugin. Acepta.
3. Actívalo en *Edit → Plugins → Westside Crime* si no lo está.

### Conexión (Blueprint o C++)

| Dónde | Qué hacer |
|---|---|
| Personaje del jugador | Añade `WLAppearanceComponent`. En *BeginPlay*: `Get World Subsystem (WLCrimeSubsystem) → Set Suspect`. Llama a `Set Outfit` al cambiarse de ropa, `Enter Vehicle` / `Exit Vehicle` al subir o bajar de un coche (usa como `VehicleId` un nombre único por coche, como su matrícula) y `Set Vehicle Color` al repintar. |
| Peatones | Añade `WLWitnessComponent`. En `On Call Started` detén al peatón y reproduce la animación del móvil; en `On Call Finished` haz que huya. Llama a `Cancel Call` cuando muera o caiga. |
| Agentes | Añade `WLPoliceSenseComponent`. En su IA: si `Should Chase` → perseguir al jugador; si no → ir a `Get Search Location`. |
| Delitos (golpe, disparo, robo de coche, atropello) | `WLCrimeSubsystem → Commit Crime (Location, Severity, bLoud, bSeenByPolice)`. Para `bSeenByPolice` pregunta a los agentes cercanos con `Sees Suspect`. Los disparos van con `bLoud = true`. |
| HUD | Escucha `On Wanted Changed` (estrellas), `On Known Changed` (estrellas normales = te persiguen, grises = te buscan), `On Description Changed` (texto de la descripción) y `On No Longer Matches`. |
| Detención, hospital, nueva partida | `Clear Heat`. |

Todos los valores (tiempos, radios y distancias de reconocimiento) son propiedades editables del
subsistema.
