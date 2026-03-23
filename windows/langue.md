# Ajuster la langue lorsque l'interface mélange l'anglais et le français

## Contexte
Un ordinateur avec «Windows 11 US English» installé.
J'ai changé la langue pour `fr-CA` pendant les mises à jour.
L'interface était partiellement en anglais et en français.

## Solution
Ce qui a fonctionné je crois était de
1. laisser toutes les mises à jour se compléter
2. désinstaller tous les packs de lange
3. redémarrer.
4. réinstaller la langue française canadienne

## Autres commandes sans succès
Pour référence, les commandes que j'avais essayé:
```
$LangList = New-WinUserLanguageList fr-CA
Set-WinUserLanguageList $LangList -Force
Set-WinUILanguageOverride fr-CA
Set-Culture fr-CA
Set-WinSystemLocale fr-CA
```
| Command                     | Effect                                    |
| --------------------------- | ----------------------------------------- |
| `Set-WinUserLanguageList`   | Sets preferred language list for the user |
| `Set-WinUILanguageOverride` | Forces UI language                        |
| `Set-Culture`               | Date, number, formatting                  |
| `Set-WinSystemLocale`       | Language used by non-Unicode programs     |

`Set-WinHomeLocation -GeoId 39` pour changer la localisation pour le Canada.

`Get-WinUserLanguageList` pour voir si `fr-CA` est dans la liste.

```
DISM /Online /Cleanup-Image /StartComponentCleanup
DISM /Online /Cleanup-Image /RestoreHealth
dism /online /get-intl
```
Pour essayer de forcer l'enregistrement des resources du paquet pour la langue fr-CA.
La dernière ligne affiche quel paquet de langue est installé.


