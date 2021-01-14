Projet S5 Projet

Installation du projet:

Placez-vous dans le repertoire qui contiendra le simulateur.
git clone https://github.com/TarJimmy/Projet-S5-PROG.git
cd Projet-S5-PROG/

si vous êtes sur une VM: chmod u+x configure

./configure CFLAGS="-Wall -Werror -g"
make

Vous aurez ensuite besoin de package linux additionnel:

gdb-multiarch: apt install gdb-multiarch
un compilateur: apt install gcc-arm-none-gdnuabi
crée automatiquement buildables: apt install autoreconf

Lancer ensuite: autoreconf -vif

Ouvrir 2 terminaux.

Dans le 1er terminal:

Le programme principal, arm_simulator, doit utiliser une session, donc dans un 1er terminal lancer :
./arm_simulator
Cela permet l'ouverture de 2 connexions: le port gdb client et le port pour irq (inutile pour nous)

Dans le 2ème terminal: gdb-multiarch

- file path/fichier_executable, pour charger le fichier que vous voulez tester
- target remote localhost:"port de arm_simulator"
- load

Ensuite vous pouvez utiliser les commandes classiques de gdb pour lancer le programme:

- step,
- break,
- cont ...
