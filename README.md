# TCAN485 J1939 CAN Sniffer

Pre-configured for LilyGo T-CAN485 v1.1 on a J1939 truck network.

| Setting | Value |
|---|---|
| Baud rate | 250 kbps |
| Frame types | 11-bit + 29-bit extended |
| Mode | Listen only (passive, safe) |
| CAN TX | GPIO27 |
| CAN RX | GPIO26 |
| Enable | GPIO23 (auto LOW) |
| Serial output | 115200 baud |

## Download the compiled .bin

Go to **Actions** tab → latest run → **Artifacts** → download `TCAN485-firmware`

## Flash via Android

1. Download `TCAN485_J1939_Sniffer_250k.bin`
2. Connect TCAN485 via USB OTG
3. Use **ESP32 Flasher** app
4. Flash at offset `0x10000`, flash size `4MB`, mode `DIO`
