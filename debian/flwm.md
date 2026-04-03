# Utiliser flwm dans Debian 13.3

## But

Obtenir un environnement de bureau simple pour naviguer avec `firefox-esr` sous Debian 13.3.

## Expérience 1 : Vérification et installation des paquets

**Résultat :** `flwm`, `xorg` et `firefox-esr` sont installés.

**Explication :**
- `sudo apt list flwm*` — vérifie si `flwm` est disponible dans les dépôts.
- `sudo apt list xorg*` — vérifie si `xorg` est disponible (nécessaire pour démarrer un serveur X avec `startx`).
- `sudo apt list firefox*` — vérifie si `firefox-esr` est disponible (pour les sites utilisant JavaScript, non supporté par `lynx`).
- `sudo apt install flwm xorg firefox-esr` — installe les trois paquets.

## Expérience 2 : Démarrage de flwm et ouverture de Firefox

**Résultat :** Firefox s'ouvre dans un bureau léger géré par flwm.

**Explication :**
1. Sur le premier terminal noyau, garder `tmux` ouvert.
2. Basculer vers un 2e terminal avec `Ctrl + Alt + F2`.
3. Se connecter.
4. Exécuter `startx`.
5. Clic droit (ou `Alt + Tab`) pour ouvrir le menu flwm, puis aller dans `Debian > Applications > Shells > Bash`.
6. Dans le terminal qui s'ouvre, exécuter `firefox`.
