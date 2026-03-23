# Interactions ACPI et AMDGPU dans Debian 13.3

## Expérience 1 : Démarrage normal (ACPI activé)

**Résultat :** La résolution de l'écran change automatiquement environ 10 secondes après le démarrage.

**Explication :** Le processus se déroule en plusieurs étapes :
1. GRUB initialise un mode vidéo basique (ex. 1024×768).
2. Le kernel Linux charge et utilise ACPI pour détecter le GPU AMD.
3. Le module `amdgpu` est chargé (inclus par défaut dans le kernel — aucune installation séparée nécessaire).
4. Le Kernel Mode-Setting (KMS) s'active : il interroge le moniteur (via EDID) et règle la résolution native.

## Expérience 2 : Démarrage avec `acpi=off`

**Résultat :** La résolution reste faible et ne change plus automatiquement. Les messages d'erreur ACPI disparaissent.

**Explication :** Sans ACPI, le kernel ne peut pas découvrir correctement les périphériques PCIe. Le module `amdgpu` ne se charge pas ou ne s'initialise pas complètement, donc KMS ne s'active pas. Le système reste bloqué dans le mode vidéo basique de GRUB. C'est un compromis : les erreurs ACPI disparaissent mais la détection matérielle du GPU est cassée.

## Recommandations

Note: Je n'ai pas testé le contenu ci-dessous qui est potentiellement inexact.

Préférer des paramètres ciblés :

- **`loglevel=3`** — Masque les messages non critiques au démarrage sans désactiver ACPI. Solution recommandée.
- **`acpi=strict`** — Force le kernel à respecter strictement la spec ACPI, ce qui peut contourner certains bugs firmware.
- **`pci=noacpi`** — Désactive uniquement la partie ACPI du routage PCI, possible source des erreurs.

Pour vérifier que le driver `amdgpu` est actif :
```bash
lspci -k | grep -A 3 -E "(VGA|3D)"
```
La sortie doit contenir `Kernel driver in use: amdgpu`.
