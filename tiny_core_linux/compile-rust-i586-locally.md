# Compiler Rust pour i586 localement sur Tiny Core Linux

## Contexte

PC avec CPU Phenom II 64 bits, Debian 13.3 installé sur `/dev/sda1`, Tiny Core Linux dans `/boottcl` en multi-boot via GRUB.
Voir [5 systèmes sur 2 partitions](../debian/5_systemes_2_paritions.html) pour la configuration multi-boot.

**Objectif :** `make build-locally` depuis [rust-i586](https://github.com/linic/rust-i586) doit s'exécuter sans erreur et produire les outils Rust ciblant i586.

## Problème : `-sh: ./cargo: not found` sur CorePure64

**Résultat :** Tentative de compilation sur CorePure64 → erreur `-sh: ./cargo: not found`.

**Explication :** Ce message ne signifie pas que le fichier est manquant. Sur un système 64 bits pur, les binaires 32 bits comme `cargo` nécessitent l'interpréteur ELF 32 bits (`/lib/ld-linux.so.2`). Absent sur CorePure64, le kernel ne peut pas démarrer le binaire.

Outils de diagnostic :
- `uname -m` : architecture du système
- `file cargo` : type du binaire (ex. `ELF 32-bit LSB executable, Intel 80386, interpreter /lib/ld-linux.so.2`)
- `ldd cargo` : librairies requises (affiche `not found` si manquantes)

**Solution :** Utiliser [core64 avec espace utilisateur 32 bits](./noyau64_utilisateur32.html) — kernel 64 bits + userspace 32 bits. Ajouter dans `/etc/grub.d/40_custom` sous Debian :

```
menuentry "core64 17.0 with 32 bits user space" {
	insmod ext2
	set root='hd0,1'
	linux /boottcl/vmlinuz64-17.0 vga=795 syslog showapps tce=UUID="ab289eda-4e92-4c53-994d-1b7cfdef0d8e" home=UUID="ab289eda-4e92-4c53-994d-1b7cfdef0d8e" opt=UUID="ab289eda-4e92-4c53-994d-1b7cfdef0d8e" lang=fr_CA.UTF-8
	initrd /boottcl/rootfs-17.0.gz /boottcl/modules64-17.0.gz
}
```
