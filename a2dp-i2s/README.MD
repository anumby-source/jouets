# Réception bluetooth avec esp32

<img width="249" height="328" alt="image" src="https://github.com/user-attachments/assets/29a3d41a-9778-4663-8314-1f28cf3a3e3c" />

Pour comprendre le fonctionnement, voici un montage avec 2 composants esp32 et un lecteur PCM5100 soudé sur un connecteur femelle ; le montage est réalisé en reliant mal <> femelle

<img width="310" height="314" alt="image" src="https://github.com/user-attachments/assets/35eb8d4b-0b39-405d-8306-2b49f69c2e07" />


ESP Pin	I2S Signal	PCM5102 Pin
* GPIO15	LRCK	LRCK
* GPIO2	DATA	DIN
* GPIO4	BCK	BCK

# Test complet CROUS

LIRE https://github.com/arnaudrco/CROUS-micro-python/blob/main/inmp-rms/README.MD
