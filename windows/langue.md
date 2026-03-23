# Ajuster la langue lorsque l'interface mélange l'anglais et le français

## Contexte

Windows 11 US English installé, langue changée pour `fr-CA` pendant les mises à jour. L'interface était partiellement en anglais et en français.

## Solution

Note : je ne suis pas certain que ce soit exactement ce qui a fonctionné.

1. Laisser toutes les mises à jour se compléter.
2. Désinstaller tous les packs de langue.
3. Redémarrer.
4. Réinstaller la langue française canadienne.

## Commandes essayées sans succès

Pour référence, les commandes PowerShell essayées :

```powershell
$LangList = New-WinUserLanguageList fr-CA
Set-WinUserLanguageList $LangList -Force
Set-WinUILanguageOverride fr-CA
Set-Culture fr-CA
Set-WinSystemLocale fr-CA
```

| Commande                    | Effet                                              |
| --------------------------- | -------------------------------------------------- |
| `Set-WinUserLanguageList`   | Définit la liste de langues préférées de l'usager  |
| `Set-WinUILanguageOverride` | Force la langue de l'interface                     |
| `Set-Culture`               | Dates, nombres, formatage                          |
| `Set-WinSystemLocale`       | Langue pour les programmes non-Unicode             |

`Set-WinHomeLocation -GeoId 39` pour changer la localisation pour le Canada.

`Get-WinUserLanguageList` pour vérifier que `fr-CA` est dans la liste.

Commandes DISM essayées :

```
DISM /Online /Cleanup-Image /StartComponentCleanup
DISM /Online /Cleanup-Image /RestoreHealth
dism /online /get-intl
```

La dernière ligne affiche quel paquet de langue est installé.
