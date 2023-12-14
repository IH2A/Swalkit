# 1. - Table des matières
- [1. - Table des matières](#1---table-des-matières)
- [2. Swalkit: "Smart Walker Kit"](#2-swalkit-smart-walker-kit)
- [3. Fabriquer son kit en quelques étapes](#3-fabriquer-son-kit-en-quelques-étapes)
  - [3.1. Les outils](#31-les-outils)
  - [le materiel](#le-materiel)
- [4. Publications](#4-publications)
- [5. Experimentations](#5-experimentations)
- [6. Credits (clic sur image)](#6-credits-clic-sur-image)

# 2. Swalkit: "Smart Walker Kit"

<img style="float:right" src="https://user-images.githubusercontent.com/100538879/157646210-4b248532-0007-4f7d-b5d4-7d8bc51b2a63.png" alt="walker" height="500"/>

Le Swalkit est un kit open source de déambulateur intelligent pour fournir une aide à la navigation par retour haptique aux personnes déficientes visuelles et motrices

Le kit se compose:
- d'un ensemble de capteurs positionnés près des roues avant du déambulateur. Ceux-ci detectent une information simple: la distance entre l'avant du capteur et un éventuel obstacle jusqu'à 80 cm devant. Ils permettent de détecter les obstacles devant et sur les côtés du déambulateur.

- de deux moteurs vibrants, au niveau des poignées. Ceux-ci sont contrôllés en fonction de l'environment detecté par les capteurs. Ils offrent un retour haptique par vibrations sur les poignées du déambulateur afin d’informer l’usager de la position et la distance des obstacles.

Le kit proposé est:
- Reproductible et adaptable sur des aides techniques existantes
- Intuitif dans sont utilisation
- Personnalisable
- à faible coût

Une application android permet de configurer le comportement des moteurs. Ainsi, il est possible de définir différents profils pour différents utilisateurs et différents contextes.
On définit d'abord quatres zones de detection: loin, proche, très proche, et frontal. Ces zones de detection sont définies par des distances pramétrées par l'application android.
Il est possible ensuite de paramétrer le comportement des  moteurs en fonction de chaque zone de detection, en choisissant une intensité de vibration et une periode de pulsation. Par exemple, on peut définir uen faible intensité et pas de pulsation dans une zone lointaine, qu'on peut définir à au delà de 70cm par exemple. On peut aussi définir une pulsation rapide, de l'ordre de 100ms, pour le cas frontale, et une pulsation plus longue, par exemple 200ms, pour le cas très proche.

<p align="center"><img src="retour-adapte.png" alt="schema" height="200"/>

https://user-images.githubusercontent.com/100538879/157646195-c86fd216-0dd4-412f-9931-ac7addbbb123.mp4

Le reste de la page indique comment fabriquer son kit, et les publications dont le kit à fait l'objet.
L'ensemble des sources sont disponible sur ce dépot.

> **Attention: clause de non-responsabilité
> La chaire IH2A décline toute responsabilité pour les dommages ou les pertes causés par la fabrication de votre kit. Nous ne fabriquons pas de kit et ne fournissons pas de kit. Nous fournissons uniquement les indications de montage et d'assemblage, ainsi qu'une liste de composants. Nous ne pouvons en aucun cas être responsable des malfaçons ou des problèmes liés à la malfaçon de votre kit.**


# 3. Fabriquer son kit en quelques étapes

## 3.1. Les outils

La table suivante indique la liste des outils premettant la fabrication d'un kit :

|   Nom             |     Pour quoi faire?                                                |    Remarques   |
|-----------|-|-|
|   Un ordinateur (windows de preference)   |  Téléverser le programme dans le micro-controlleur M5Stack AtomS3   |  utiliser l'outil de flashage fourni                                    |
|                   |    Adapter les modèles 3D                                           |  utliliser un logiciel de modelisation 3d (fusion360, solidworks...)    |
|   Une imprimante 3D    |   Imprimer les boitiers et interfaces mécanique                |  une imprimante petit format fera l'affaire         |
|   Un fer à souder    |   Il y a des points de soudure à faire sur une carte pcb pour définir des addresses différentes aux bus i2c intelligent  |           |
|    |   Il faut souder des câbles sur les moteurs  |           |
|   Un petit tournevis cruciforme |   Pour ouvrir et ferme les différents boitiers  |           |

## le materiel

La table suivante indique la liste du materiel à commander, necessaire à la fabrication d'un kit :

|   Nom     |    Pour quoi faire?   |    fournisseur    |   quantité nécéssaire    |     Aperçu     |
|-|-|-|-|-|
|M5Stack AtomS3|C'est le micro-controlleur central qui gère les capteurs, les moteurs, la logique, la communication bluetooth...|[shop.m5stack](https://shop.m5stack.com/products/atoms3-dev-kit-w-0-85-inch-screen);[gotronic](https://www.gotronic.fr/art-module-atoms3-dev-kit-c123-37089.htm)|1| <img src="m5atoms3.jpg" alt="AtomS3" style="max-width: 100px;"/> |
|Batterie|C'est l'alimentation de tout le système|[gotronic](https://www.gotronic.fr/art-batterie-externe-usb-57975-35475.htm)|1| <img src="ori-batterie-externe-usb-57975-35475.jpg" alt="Batterie" style="max-width: 100px;"/>|
|Smart Hub i2c|Ces hub gèrent les capteurs et permet de les différencier sur le bus i2c|[gotronic](https://www.gotronic.fr/art-hub-8-ports-i2c-grove-103020293-31770.htm)|2| <img src="ori-hub-8-ports-i2c-grove-103020293-31770.jpg" alt="Smart Hub" style="max-width: 100px;"/>|
|Hub i2c simple|Ce hub permet de connecter l'ensemble des dispositifs sur le même bus i2c|[gotronic](https://www.gotronic.fr/art-hub-6-ports-i2c-grove-103020272-31348.htm)|1| <img src="ar-hub-6-ports-i2c-grove-103020272-31348.jpg" alt="Hub i2c simple" style="max-width: 100px;"/>|
|Pont en H|C'est un circuit électronique utilisé pour contrôler l'intensité du courant traversant un moteur|[shop.m5stack](https://shop.m5stack.com/products/h-bridge-unit-v1-1-stm32f030)|2|<img src="hbridge.png" alt="Pont en H" style="max-width: 100px;"/>|
|Moteur LMA|Ce sont les moteur vibrants a positionner près des poignées du déambulateur|[digikey](https://www.digikey.fr/en/products/detail/vybronics-inc/VG2230001H/16719289)|2|<img src="MFG_VG2230001H.jpg" alt="Moteur LMA" style="max-width: 100px;"/>|
|Capteurs TOF|Capteur "Time of flight" VL53L0X permettant la détection d'un objet dans une plage de 3 à 100 cm|[gotronic](https://www.gotronic.fr/art-module-grove-time-of-flight-101020532-28252.htm#complte_desc)|2|<img src="grove-vl53l0x.jpg" alt="Capteurs" style="max-width: 100px;"/>|
|Cable grove court|Câbles courts pour connecter les composants sur le bus i2c|[digikey](https://www.digikey.fr/en/products/detail/seeed-technology-co-ltd/110990036/5482563)|3 lots de 5|<img src="110990036.jpg" alt="grove court" style="max-width: 100px;"/>|
|Câble grove long|Câbles longs pour connecter les composants sur le bus i2c|[digikey](https://www.digikey.fr/en/products/detail/seeed-technology-co-ltd/110990027/5482567)|1 lot de 5|<img src="MFG_110990027.jpg" alt="grove long" style="max-width: 100px;"/>|
|Câble usbc|Pour televerser le programme, alimenter l'atomS3 et recharger la batterie|[gotronic](https://www.gotronic.fr/art-cordon-10-cm-usbc-0-1-33653.htm)|1|<img src="ar-cordon-10-cm-usbc-0-1-33653.jpg" alt="usbc" style="max-width: 100px;"/>|
|Cable d'alimentation moteur|Pour alimenter les moteurs vibrants|à titre indicatif: [gotronic](https://www.gotronic.fr/art-cable-blinde-cbp225-5075.htm)|2 mètres|<img src="ar-cable-blinde-cbp225-5075.jpg" alt="Cable" style="max-width: 100px;"/>|
|Bobine PLA|Pour imprimer les boitiers|à titre indicatif: [arianeplast](https://www.arianeplast.com/51-pla-format-1-kg)|<1kg|<img src="pla-format-1-kg.jpg" alt="PLA" style="max-width: 100px;"/>|
|Visserie|Pour l'assemblage|M2 et M3|-||
|Colle forte|Pour coller les moteurs||-||
|Déambulateur|||1|<img src="gemino-20-rollator-galley-main.jpg" alt="rollator" style="max-width: 100px;"/>|

# 4. Publications

# 5. Experimentations

# 6. Credits (clic sur image)

[![](https://ih2a.insa-rennes.fr/fileadmin/_processed_/0/f/csm_schema_chaire_a0628d819e.png)](https://ih2a.insa-rennes.fr/)
