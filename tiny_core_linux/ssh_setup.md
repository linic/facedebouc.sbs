# SSH setup on TinyCore Linux 17.x

One-time setup to install an SSH server on the machine and connect to it from a
Debian machine using key-based authentication.

---

## Overview

The persistence method depends on how TinyCore was installed. Choose your path
before starting:

```
            Boot TCL
               │
               ▼
    ┌──────────────────────────┐
    │  Frugal install?         │
    │  (home= and opt= in      │
    │   APPEND line)           │
    └──────────────────────────┘
          │            │
         No            Yes
    (RAM only)    (files on disk)
          │            │
          ▼            ▼
    filetool.sh    home/tc restore
    method         method
    Steps 1–5      Steps 1–5
    then Step 6a   then Step 6b
          │            │
          └─────┬──────┘
                ▼
      SSH works after reboot
```

**Not sure which you have?** Run:
```sh
cat /proc/cmdline
```
If the output contains `home=` and `opt=`, you have a frugal install. If not,
you are booting from RAM only.

---

## Prerequisites

- The machine is running TinyCore Linux 17.x (Core, no GUI).
- The machine has network access. Verify with `ifconfig eth0`. If no IP is shown,
  configure manually:

  ```sh
  sudo ifconfig eth0 192.168.0.180   # adjust to your network
  sudo route add default gw 192.168.0.1
  echo "nameserver 1.1.1.1" | sudo tee /etc/resolv.conf
  ```

- You have physical or keyboard access to the machine for this first-time setup.

---

## Step 1 — Install openssh

On the machine:

```sh
tce-load -wi openssh
```

| Extension | Version | Dependency |
|-----------|---------|------------|
| `openssh.tcz` | 10.0p1 | `openssl.tcz` (auto-installed) |

After installation:
- Init script: `/usr/local/etc/init.d/openssh`
- Server binary: `/usr/local/sbin/sshd`
- Config templates: `/usr/local/etc/ssh/sshd_config.orig`, `ssh_config.orig`

---

## Step 2 — Configure sshd

Copy the template to activate the config:

```sh
sudo cp /usr/local/etc/ssh/sshd_config.orig /usr/local/etc/ssh/sshd_config
```

The defaults are usable as-is. Optionally harden by disabling password
authentication — do this **after** verifying key-based login works (Step 5):

```sh
sudo vi /usr/local/etc/ssh/sshd_config
```

Uncomment or set:

```
PermitRootLogin prohibit-password
PasswordAuthentication no
```

---

## Step 3 — Generate an ed25519 key pair on the Debian machine

On the Debian machine (not the machine):

```sh
ssh-keygen -t ed25519
```

This creates:
- `~/.ssh/id_ed25519` — private key (keep secret)
- `~/.ssh/id_ed25519.pub` — public key (to be placed on the machine)

---

## Step 4 — Authorize the key on the machine

**Option A — copy over the network (if password auth is still on):**

On the Debian machine:

```sh
scp ~/.ssh/id_ed25519.pub tc@<machine-ip>:~/id_ed25519.pub
```

Then on the machine:

```sh
mkdir -p ~/.ssh
chmod 700 ~/.ssh
cat ~/id_ed25519.pub >> ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys
rm ~/id_ed25519.pub
```

**Option B — type or paste directly on the machine:**

```sh
mkdir -p ~/.ssh && chmod 700 ~/.ssh
vi ~/.ssh/authorized_keys   # paste the contents of id_ed25519.pub
chmod 600 ~/.ssh/authorized_keys
```

**Option C — transfer with netcat (no SSH, no password auth required):**

TinyCore's BusyBox includes `nc`, so no extra extension is needed.

First, start the listener on the machine:

```sh
nc -l -p 9999 > ~/id_ed25519.pub
```

Then, on the Debian machine, send the key:

```sh
cat ~/.ssh/id_ed25519.pub | nc <machine-ip> 9999
```

`nc` exits on both sides once the transfer completes. Then on the machine:

```sh
mkdir -p ~/.ssh
chmod 700 ~/.ssh
cat ~/id_ed25519.pub >> ~/.ssh/authorized_keys
chmod 600 ~/.ssh/authorized_keys
rm ~/id_ed25519.pub
```

---

## Step 5 — Start sshd and verify

On the machine:

```sh
sudo /usr/local/etc/init.d/openssh start
```

On the Debian machine:

```sh
ssh -i ~/.ssh/id_ed25519 tc@<machine-ip>
```

**First connection:** SSH will display a fingerprint and ask you to confirm.
To verify it beforehand on the machine:

```sh
ssh-keygen -lf /usr/local/etc/ssh/ssh_host_ed25519_key.pub
```

Compare the output to what the Debian client shows before accepting.

---

## Step 6 — Persist across reboots

TinyCore resets to a clean state on each boot. Choose the method that matches
your install (see the Overview flowchart).

---

### Step 6a — filetool.sh method (RAM-only install)

In a RAM-only install, no directory is backed by a disk partition. The
`filetool.sh` backup saves a snapshot of selected files into `mydata.tgz` on
the tce partition, which is restored at the next boot.

**Add config files and host keys to the backup list:**

```sh
cat >> /opt/.filetool.lst << 'EOF'
usr/local/etc/ssh/sshd_config
usr/local/etc/ssh/ssh_host_rsa_key
usr/local/etc/ssh/ssh_host_rsa_key.pub
usr/local/etc/ssh/ssh_host_ecdsa_key
usr/local/etc/ssh/ssh_host_ecdsa_key.pub
usr/local/etc/ssh/ssh_host_ed25519_key
usr/local/etc/ssh/ssh_host_ed25519_key.pub
home/tc/.ssh/authorized_keys
EOF
```

**Add sshd autostart and the backup list itself to the backup:**

```sh
echo '/usr/local/etc/init.d/openssh start' | sudo tee -a /opt/bootlocal.sh

cat >> /opt/.filetool.lst << 'EOF'
opt/bootlocal.sh
opt/.filetool.lst
EOF
```

**Add openssh to the onboot extension list:**

```sh
echo 'openssh.tcz' | sudo tee -a "$(tce-status -d)/onboot.lst"
```

**Save the backup:**

```sh
filetool.sh -b
```

Reboot and verify:

```sh
sudo reboot
# then from the Debian machine:
ssh -i ~/.ssh/id_ed25519 tc@<machine-ip> 'uname -a'
```

---

### Step 6b — home/tc restore method (frugal install)

In a frugal install with `home=` and `opt=` in the boot parameters, `/home/tc/`
and `/opt/` are mounted from disk and persist automatically. However, SSH host
keys and `sshd_config` live under `/usr/local/etc/ssh/`, which is in RAM and
resets on each boot. The solution is to store copies in `/home/tc/` and restore
them at boot via a script.

**Create the restore directory and copy the files into it:**

```sh
mkdir -p /home/tc/openssh-setup/etc/ssh

sudo cp /usr/local/etc/ssh/sshd_config        /home/tc/openssh-setup/etc/ssh/
sudo cp /usr/local/etc/ssh/ssh_host_rsa_key     /home/tc/openssh-setup/etc/ssh/
sudo cp /usr/local/etc/ssh/ssh_host_rsa_key.pub /home/tc/openssh-setup/etc/ssh/
sudo cp /usr/local/etc/ssh/ssh_host_ecdsa_key     /home/tc/openssh-setup/etc/ssh/
sudo cp /usr/local/etc/ssh/ssh_host_ecdsa_key.pub /home/tc/openssh-setup/etc/ssh/
sudo cp /usr/local/etc/ssh/ssh_host_ed25519_key     /home/tc/openssh-setup/etc/ssh/
sudo cp /usr/local/etc/ssh/ssh_host_ed25519_key.pub /home/tc/openssh-setup/etc/ssh/

sudo chmod 600 /home/tc/openssh-setup/etc/ssh/ssh_host_*_key
```

**Create the restore script:**

```sh
cat > /home/tc/openssh-setup/restore.sh << 'EOF'
#!/bin/sh
# Restore SSH host keys and config from home/tc, then start sshd.
SRC=/home/tc/openssh-setup/etc/ssh
DST=/usr/local/etc/ssh

cp "$SRC/sshd_config"               "$DST/"
cp "$SRC/ssh_host_rsa_key"           "$DST/" && chmod 600 "$DST/ssh_host_rsa_key"
cp "$SRC/ssh_host_rsa_key.pub"       "$DST/"
cp "$SRC/ssh_host_ecdsa_key"         "$DST/" && chmod 600 "$DST/ssh_host_ecdsa_key"
cp "$SRC/ssh_host_ecdsa_key.pub"     "$DST/"
cp "$SRC/ssh_host_ed25519_key"       "$DST/" && chmod 600 "$DST/ssh_host_ed25519_key"
cp "$SRC/ssh_host_ed25519_key.pub"   "$DST/"

/usr/local/etc/init.d/openssh start
EOF

chmod +x /home/tc/openssh-setup/restore.sh
```

**Register the restore script in bootlocal.sh:**

```sh
echo '/home/tc/openssh-setup/restore.sh' | sudo tee -a /opt/bootlocal.sh
```

Because `opt=` is set, `/opt/bootlocal.sh` is on disk and this change persists
automatically — no backup step needed.

**Add openssh to the onboot extension list:**

```sh
echo 'openssh.tcz' | sudo tee -a "$(tce-status -d)/onboot.lst"
```

Reboot and verify:

```sh
sudo reboot
# then from the Debian machine:
ssh -i ~/.ssh/id_ed25519 tc@<machine-ip> 'uname -a'
```

---

## Troubleshooting

| Symptom | Likely cause | Fix |
|---------|-------------|-----|
| `Connection refused` | sshd not running | `sudo /usr/local/etc/init.d/openssh start` |
| `Permission denied (publickey)` | Key not in authorized_keys or wrong permissions | `~/.ssh` → 700, `authorized_keys` → 600 |
| No IP after reboot | USB-Ethernet auto-detection failed | Configure manually (see Prerequisites) |
| sshd not starting at boot | Not in onboot.lst or bootlocal.sh not saved | Re-run Step 6 and reboot |
| Host key changed warning | New keys generated after a RAM boot | Replace with backed-up keys (Step 6b) or update `known_hosts` on Debian |
| `tmux attach` fails: `missing or unsuitable terminal: tmux-256color` | Debian is inside tmux, so `$TERM=tmux-256color`; that terminfo entry doesn't exist on the machine | Connect with `TERM=xterm-256color ssh tc@<machine-ip>`, or see Appendix |

---

## Appendix — SSH client config on the Debian machine

Optional. Adding a `Host` entry to `~/.ssh/config` on the Debian machine lets
you type `ssh machine` instead of the full command, and handles `TERM` automatically.

```
Host machine
    HostName <machine-ip>
    User tc
    IdentityFile ~/.ssh/id_ed25519
    SetEnv TERM=xterm-256color
```

**Note:** `SetEnv` forwards the variable via environment forwarding, which
requires `AcceptEnv TERM` in the machine's `sshd_config`. If that line is absent,
the server silently ignores it and the `tmux-256color` error will reappear.
In that case, use the inline form instead — it sets `TERM` before the PTY
is negotiated and works unconditionally:

```sh
TERM=xterm-256color ssh machine
```
