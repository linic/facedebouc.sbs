# Requirements
Install libsodium on your dev environment. Either by following
https://doc.libsodium.org/installation
or by `sudo apt update` and maybe 
`sudo apt install libsodium-dev build-essential g++ autoconf autotools-dev`

# Usage
`make`
`make run1`
`make run2` in another terminal

You'll have debug information printed to the screen so you'll be able to follow the logic.
Enter some words in the terminal to see them be encrypted and sent over to the other running
instance.

