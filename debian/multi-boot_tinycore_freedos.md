# Multi-boot avec Debian 13.3 comme base et Tiny Core Linux + FreeDOS

## Contexte
Utilise sur un ASUS 1001px et un PC generique avec un Phenom II.
<think> the information here is duplicated in several pages. Please remove duplication and synthesize by using the <code_refs> below as connections to identify duplication.
<code_refs>
debian/5_systemes_2_paritions.html:21:core64
debian/5_systemes_2_paritions.html:15:core64
debian/5_systemes_2_paritions.html:21:core64
tiny_core_linux/noyau64_utilisateur32.html:9:core64
tiny_core_linux/compiler_gnupg.html:9:core64
tiny_core_linux/compile-rust-i586-locally.md:38:core64
5_systemes_2_paritions.html:6:FAT
5_systemes_2_paritions.html:50:FAT
5_systemes_2_paritions.html:51:FAT
5_systemes_2_paritions.html:71:FAT
5_systemes_2_paritions.html:78:FAT
multi-boot_tinycore_freedos.md:9:FAT
multi-boot_tinycore_freedos.md:36:FAT 
</code_refs>

## Etapes
installer debian 13.3 en utilisant l'image «net install» et l'option «expert installation».
Configurer la langue, le clavier, le miroir de paquets, etc...
Partitionner en 3 partitions primaires: 100 Go de ext4, 10 Go de SWAP, 20 Go de FAT32.
Compléter l'installation de Debian.
sudo umount /dev/mmcblk0
sudo cp  FD14FULL.img /dev/mmcblk0
sudo sync
Au démarrage, peser sur ESC pour booter dans la carte freedos.
Quitter l'installation pour retrouver la ligne de command DOS.
SYS D:
XCOPY C:\*.* D:\ /E /H
edit fdauto.bat
rem cdrom (2fois)
rem setup
rem welcome
Ensuite REBOOT ou CTRL+ALT+DELETE pour redémarrer
Dans Debian, modifier /etc/grub.d/40_custom pour ajouter un menuentry pour freedos
pour booter directement dans freedos, aller dans debian et
sudo vim /etc/default/grub
change the default entry to 2
change the default resolution to 640x480
sudo update-grub

Chemin alternatif pour freedos
boot debian
sudo umount /dev/sda2
sudo fdisk /dev/sda
p pour voir les partitions, le LBA va manquer
t pour changer le type de la partition
Ma partition FAT32 est plus que 8 Go donc on a besoin de LBA donc entrer c
p pour voir les partitions, le LBA va être là
w pour écrire les changements

Important, mettre les contrôleurs en mode IDE.

## Debogage
J'ai eu deux problemes: 1. la partition n'avait pas LBA apres avoir complete l'installation Debian
2. mon controleur etait en mode AHCI au lieu de IDE et j'avais des erreurs.
## Partitionnement pour FreeDOS
sudo mkfs.vfat -F 32 /dev/sda2
Make sure:

    It is marked bootable (fdisk → toggle boot flag)

    Boot sector is written (installer normally does this)

## IDE V.S. AHCI
Mes notes de debogage:"
so the install did complete this time
When bootin in emergency mode, I get
`Invalid Opcode at 88D6 10D0 0202` and many zeroes after that
and then
`Bad or missing Command Interpreter`

COMP D:\COMMAND.COM C:\COMMAND.COM
is displaying lots of differences between the image of FreeDOS and disk install
"
Suggestions de deepseek
"
The Core Issue: FreeDOS and AHCI

The fundamental problem is that FreeDOS, by default, does not have native drivers for AHCI controllers

. Here's why this creates the exact situation you're seeing:

    How Hard Drives Work: When your SATA controller is in AHCI mode, the hard drive connected to it speaks a "language" (the AHCI protocol) that the standard disk access methods in FreeDOS (BIOS interrupts like INT 13h) do not understand natively 

.

Why the USB Works: Your USB drive, however, does not use the SATA controller. It uses the USB controller (EHCI/XHCI). While USB support in FreeDOS can also be tricky, the fact that your system boots from it successfully means the BIOS is providing a compatible layer for the USB drive that FreeDOS can use. The USB drive's path to the CPU bypasses the problematic AHCI controller.

The "Legacy" Fallback: When your computer first starts, the motherboard's BIOS initially puts the AHCI controller into a "legacy" or "compatibility" mode just long enough to boot an operating system
. This is why you can sometimes boot from a CD or even see the drive in the BIOS. However, once the boot process begins, this mode can be unstable or simply doesn't provide the necessary low-level access FreeDOS needs for its file system operations, leading to the "divide by zero," file corruption, and "bad command interpreter" errors you've been battling

    .

🛠️ Your Options to Fix This

You have three main paths forward, each with its own trade-offs. They are ordered from the most straightforward to the more advanced.
Solution	Description	Difficulty	Key Consideration
1. Switch SATA Mode to "IDE" or "Legacy"	Change a setting in your motherboard's BIOS/UEFI.	Easy	Simplest fix, but reduces performance for your Linux system.
2. Install a DOS AHCI Driver	Add a driver like UIDE.SYS to your FreeDOS config.	Intermediate	Maintains AHCI performance for Linux, but can be tricky to configure correctly.
3. Keep AHCI, Use GRUB2's AHCI Module	Use a modern bootloader's driver to load the FreeDOS kernel.	Intermediate	An elegant solution if you're comfortable with GRUB, but experimental.
"
